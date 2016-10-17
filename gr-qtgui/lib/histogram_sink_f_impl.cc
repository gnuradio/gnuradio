/* -*- c++ -*- */
/*
 * Copyright 2013,2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "histogram_sink_f_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/prefs.h>
#include <string.h>
#include <volk/volk.h>
#include <qwt_symbol.h>

namespace gr {
  namespace qtgui {

    histogram_sink_f::sptr
    histogram_sink_f::make(int size, int bins,
                           double xmin, double xmax,
                           const std::string &name,
                           int nconnections,
                           QWidget *parent)
    {
      return gnuradio::get_initial_sptr
	(new histogram_sink_f_impl(size, bins, xmin, xmax, name,
                                   nconnections, parent));
    }

    histogram_sink_f_impl::histogram_sink_f_impl(int size, int bins,
                                                 double xmin, double xmax,
                                                 const std::string &name,
                                                 int nconnections,
                                                 QWidget *parent)
      : sync_block("histogram_sink_f",
                   io_signature::make(0, nconnections, sizeof(float)),
                   io_signature::make(0, 0, 0)),
	d_size(size), d_bins(bins), d_xmin(xmin), d_xmax(xmax), d_name(name),
	d_nconnections(nconnections), d_parent(parent)
    {
      // Required now for Qt; argc must be greater than 0 and argv
      // must have at least one valid character. Must be valid through
      // life of the qApplication:
      // http://harmattan-dev.nokia.com/docs/library/html/qt4/qapplication.html
      d_argc = 1;
      d_argv = new char;
      d_argv[0] = '\0';

      d_main_gui = NULL;

      d_index = 0;

      // setup PDU handling input port
      message_port_register_in(pmt::mp("in"));
      set_msg_handler(pmt::mp("in"),
                      boost::bind(&histogram_sink_f_impl::handle_pdus, this, _1));

      // +1 for the PDU buffer
      for(int i = 0; i < d_nconnections+1; i++) {
	d_residbufs.push_back((double*)volk_malloc(d_size*sizeof(double),
                                                   volk_get_alignment()));
	memset(d_residbufs[i], 0, d_size*sizeof(double));
      }

      // Set alignment properties for VOLK
      const int alignment_multiple =
	volk_get_alignment() / sizeof(gr_complex);
      set_alignment(std::max(1,alignment_multiple));

      initialize();
    }

    histogram_sink_f_impl::~histogram_sink_f_impl()
    {
      if(!d_main_gui->isClosed())
        d_main_gui->close();

      // d_main_gui is a qwidget destroyed with its parent
      for(int i = 0; i < d_nconnections+1; i++) {
	volk_free(d_residbufs[i]);
      }

      delete d_argv;
    }

    bool
    histogram_sink_f_impl::check_topology(int ninputs, int noutputs)
    {
      return ninputs == d_nconnections;
    }

    void
    histogram_sink_f_impl::initialize()
    {
      if(qApp != NULL) {
	d_qApplication = qApp;
      }
      else {
#if QT_VERSION >= 0x040500
        std::string style = prefs::singleton()->get_string("qtgui", "style", "raster");
        QApplication::setGraphicsSystem(QString(style.c_str()));
#endif
	d_qApplication = new QApplication(d_argc, &d_argv);
      }

      // If a style sheet is set in the prefs file, enable it here.
      check_set_qss(d_qApplication);

      int numplots = (d_nconnections > 0) ? d_nconnections : 1;
      d_main_gui = new HistogramDisplayForm(numplots, d_parent);
      d_main_gui->setNumBins(d_bins);
      d_main_gui->setNPoints(d_size);
      d_main_gui->setXaxis(d_xmin, d_xmax);

      if(d_name.size() > 0)
        set_title(d_name);

      // initialize update time to 10 times a second
      set_update_time(0.1);
    }

    void
    histogram_sink_f_impl::exec_()
    {
      d_qApplication->exec();
    }

    QWidget*
    histogram_sink_f_impl::qwidget()
    {
      return d_main_gui;
    }

#ifdef ENABLE_PYTHON
    PyObject*
    histogram_sink_f_impl::pyqwidget()
    {
      PyObject *w = PyLong_FromVoidPtr((void*)d_main_gui);
      PyObject *retarg = Py_BuildValue("N", w);
      return retarg;
    }
#else
    void *
    histogram_sink_f_impl::pyqwidget()
    {
      return NULL;
    }
#endif

    void
    histogram_sink_f_impl::set_y_axis(double min, double max)
    {
      d_main_gui->setYaxis(min, max);
    }

    void
    histogram_sink_f_impl::set_x_axis(double min, double max)
    {
      d_main_gui->setXaxis(min, max);
    }

    void
    histogram_sink_f_impl::set_update_time(double t)
    {
      //convert update time to ticks
      gr::high_res_timer_type tps = gr::high_res_timer_tps();
      d_update_time = t * tps;
      d_main_gui->setUpdateTime(t);
      d_last_time = 0;
    }

    void
    histogram_sink_f_impl::set_title(const std::string &title)
    {
      d_main_gui->setTitle(title.c_str());
    }

    void
    histogram_sink_f_impl::set_line_label(int which, const std::string &label)
    {
      d_main_gui->setLineLabel(which, label.c_str());
    }

    void
    histogram_sink_f_impl::set_line_color(int which, const std::string &color)
    {
      d_main_gui->setLineColor(which, color.c_str());
    }

    void
    histogram_sink_f_impl::set_line_width(int which, int width)
    {
      d_main_gui->setLineWidth(which, width);
    }

    void
    histogram_sink_f_impl::set_line_style(int which, int style)
    {
      d_main_gui->setLineStyle(which, (Qt::PenStyle)style);
    }

    void
    histogram_sink_f_impl::set_line_marker(int which, int marker)
    {
      d_main_gui->setLineMarker(which, (QwtSymbol::Style)marker);
    }

    void
    histogram_sink_f_impl::set_line_alpha(int which, double alpha)
    {
      d_main_gui->setMarkerAlpha(which, (int)(255.0*alpha));
    }

    void
    histogram_sink_f_impl::set_size(int width, int height)
    {
      d_main_gui->resize(QSize(width, height));
    }

    std::string
    histogram_sink_f_impl::title()
    {
      return d_main_gui->title().toStdString();
    }

    std::string
    histogram_sink_f_impl::line_label(int which)
    {
      return d_main_gui->lineLabel(which).toStdString();
    }

    std::string
    histogram_sink_f_impl::line_color(int which)
    {
      return d_main_gui->lineColor(which).toStdString();
    }

    int
    histogram_sink_f_impl::line_width(int which)
    {
      return d_main_gui->lineWidth(which);
    }

    int
    histogram_sink_f_impl::line_style(int which)
    {
      return d_main_gui->lineStyle(which);
    }

    int
    histogram_sink_f_impl::line_marker(int which)
    {
      return d_main_gui->lineMarker(which);
    }

    double
    histogram_sink_f_impl::line_alpha(int which)
    {
      return (double)(d_main_gui->markerAlpha(which))/255.0;
    }

    void
    histogram_sink_f_impl::set_nsamps(const int newsize)
    {
      gr::thread::scoped_lock lock(d_setlock);

      if(newsize != d_size) {
	// Resize residbuf and replace data
	for(int i = 0; i < d_nconnections+1; i++) {
	  volk_free(d_residbufs[i]);
	  d_residbufs[i] = (double*)volk_malloc(newsize*sizeof(double),
                                                volk_get_alignment());

	  memset(d_residbufs[i], 0, newsize*sizeof(double));
	}

	// Set new size and reset buffer index
	// (throws away any currently held data, but who cares?)
	d_size = newsize;
	d_index = 0;

	d_main_gui->setNPoints(d_size);
      }
    }

    void
    histogram_sink_f_impl::set_bins(const int bins)
    {
      gr::thread::scoped_lock lock(d_setlock);
      d_bins = bins;
      d_main_gui->setNumBins(d_bins);
    }

    int
    histogram_sink_f_impl::nsamps() const
    {
      return d_size;
    }

    int
    histogram_sink_f_impl::bins() const
    {
      return d_bins;
    }

    void
    histogram_sink_f_impl::npoints_resize()
    {
      int newsize = d_main_gui->getNPoints();
      set_nsamps(newsize);
    }

    void
    histogram_sink_f_impl::enable_menu(bool en)
    {
      d_main_gui->enableMenu(en);
    }

    void
    histogram_sink_f_impl::enable_grid(bool en)
    {
      d_main_gui->setGrid(en);
    }

    void
    histogram_sink_f_impl::enable_axis_labels(bool en)
    {
        d_main_gui->setAxisLabels(en);
    }

    void
    histogram_sink_f_impl::enable_autoscale(bool en)
    {
      d_main_gui->autoScale(en);
    }

    void
    histogram_sink_f_impl::enable_semilogx(bool en)
    {
      d_main_gui->setSemilogx(en);
    }

    void
    histogram_sink_f_impl::enable_semilogy(bool en)
    {
      d_main_gui->setSemilogy(en);
    }

    void
    histogram_sink_f_impl::enable_accumulate(bool en)
    {
      d_main_gui->setAccumulate(en);
    }

    void
    histogram_sink_f_impl::disable_legend()
    {
      d_main_gui->disableLegend();
    }

    void
    histogram_sink_f_impl::autoscalex()
    {
      d_main_gui->autoScaleX();
    }

    void
    histogram_sink_f_impl::reset()
    {
      d_index = 0;
    }

    int
    histogram_sink_f_impl::work(int noutput_items,
                                gr_vector_const_void_star &input_items,
                                gr_vector_void_star &output_items)
    {
      int n=0, j=0, idx=0;
      const float *in = (const float*)input_items[idx];

      npoints_resize();

      for(int i=0; i < noutput_items; i+=d_size) {
	unsigned int datasize = noutput_items - i;
	unsigned int resid = d_size-d_index;
	idx = 0;

	// If we have enough input for one full plot, do it
	if(datasize >= resid) {

	  // Fill up residbufs with d_size number of items
	  for(n = 0; n < d_nconnections; n++) {
	    in = (const float*)input_items[idx++];
	    volk_32f_convert_64f_u(&d_residbufs[n][d_index],
				   &in[j], resid);
	  }

	  // Update the plot if its time
	  if(gr::high_res_timer_now() - d_last_time > d_update_time) {
	    d_last_time = gr::high_res_timer_now();
	    d_qApplication->postEvent(d_main_gui,
				      new HistogramUpdateEvent(d_residbufs, d_size));
	  }

	  d_index = 0;
	  j += resid;
	}
	// Otherwise, copy what we received into the residbufs for next time
	// because we set the output_multiple, this should never need to be called
	else {
	  for(n = 0; n < d_nconnections; n++) {
	    in = (const float*)input_items[idx++];
	    volk_32f_convert_64f_u(&d_residbufs[n][d_index],
				   &in[j], datasize);
	  }
	  d_index += datasize;
	  j += datasize;
	}
      }

      return j;
    }

    void
    histogram_sink_f_impl::handle_pdus(pmt::pmt_t msg)
    {
      size_t len;
      pmt::pmt_t dict, samples;

      // Test to make sure this is either a PDU or a uniform vector of
      // samples. Get the samples PMT and the dictionary if it's a PDU.
      // If not, we throw an error and exit.
      if(pmt::is_pair(msg)) {
        dict = pmt::car(msg);
        samples = pmt::cdr(msg);
      }
      else if(pmt::is_uniform_vector(msg)) {
        samples = msg;
      }
      else {
        throw std::runtime_error("time_sink_c: message must be either "
                                 "a PDU or a uniform vector of samples.");
      }

      len = pmt::length(samples);

      const float *in;
      if(pmt::is_f32vector(samples)) {
        in = (const float*)pmt::f32vector_elements(samples, len);
      }
      else {
        throw std::runtime_error("histogram_sink_f: unknown data type "
                                 "of samples; must be float.");
      }

      // Plot if we're past the last update time
      if(gr::high_res_timer_now() - d_last_time > d_update_time) {
        d_last_time = gr::high_res_timer_now();

        npoints_resize();

        // Clear the histogram
        if(!d_main_gui->getAccumulate()) {
          d_qApplication->postEvent(d_main_gui, new HistogramClearEvent());

          // Set to accumulate over length of the current PDU
          d_qApplication->postEvent(d_main_gui, new HistogramSetAccumulator(true));
        }

        float nplots_f = static_cast<float>(len) / static_cast<float>(d_size);
        int nplots = static_cast<int>(ceilf(nplots_f));
        int idx = 0;
        for(int n = 0; n < nplots; n++) {
          int size = std::min(d_size, (int)(len - idx));
          volk_32f_convert_64f_u(d_residbufs[d_nconnections], &in[idx], size);

          d_qApplication->postEvent(d_main_gui,
                                    new HistogramUpdateEvent(d_residbufs, size));

          idx += size;
        }

        if(!d_main_gui->getAccumulate()) {
          // Turn accumulation off
          d_qApplication->postEvent(d_main_gui, new HistogramSetAccumulator(false));
        }
      }
    }

  } /* namespace qtgui */
} /* namespace gr */

/* -*- c++ -*- */
/*
 * Copyright 2012,2013,2015 Free Software Foundation, Inc.
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

#include "time_raster_sink_f_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/prefs.h>
#include <string.h>
#include <volk/volk.h>

namespace gr {
  namespace qtgui {

    time_raster_sink_f::sptr
    time_raster_sink_f::make(double samp_rate,
			     double rows, double cols,
			     const std::vector<float> &mult,
			     const std::vector<float> &offset,
			     const std::string &name,
			     int nconnections,
			     QWidget *parent)
    {
      return gnuradio::get_initial_sptr
	(new time_raster_sink_f_impl(samp_rate,
				     rows, cols,
				     mult, offset,
				     name,
				     nconnections,
				     parent));
    }

    time_raster_sink_f_impl::time_raster_sink_f_impl(double samp_rate,
						     double rows, double cols,
						     const std::vector<float> &mult,
						     const std::vector<float> &offset,
						     const std::string &name,
						     int nconnections,
						     QWidget *parent)
      : sync_block("time_raster_sink_f",
                   io_signature::make(0, nconnections, sizeof(float)),
                   io_signature::make(0, 0, 0)),
	d_name(name), d_nconnections(nconnections), d_parent(parent),
	d_rows(rows), d_cols(cols),
	d_mult(std::vector<float>(nconnections+1,1)),
	d_offset(std::vector<float>(nconnections+1,0)),
	d_samp_rate(samp_rate)
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
                      boost::bind(&time_raster_sink_f_impl::handle_pdus, this, _1));

      d_icols = static_cast<int>(ceil(d_cols));
      d_tmpflt = (float*)volk_malloc(d_icols*sizeof(float),
                                     volk_get_alignment());
      memset(d_tmpflt, 0, d_icols*sizeof(float));

      // +1 for the PDU buffer
      for(int i = 0; i < d_nconnections+1; i++) {
	d_residbufs.push_back((double*)volk_malloc(d_icols*sizeof(double),
                                                   volk_get_alignment()));
	memset(d_residbufs[i], 0, d_icols*sizeof(double));
      }

      set_multiplier(mult);
      set_offset(offset);

      initialize();
    }

    time_raster_sink_f_impl::~time_raster_sink_f_impl()
    {
      if(!d_main_gui->isClosed())
        d_main_gui->close();

      volk_free(d_tmpflt);
      for(int i = 0; i < d_nconnections+1; i++) {
	volk_free(d_residbufs[i]);
      }

      delete d_argv;
    }

    bool
    time_raster_sink_f_impl::check_topology(int ninputs, int noutputs)
    {
      return ninputs == d_nconnections;
    }

    void
    time_raster_sink_f_impl::initialize()
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

      // Create time raster plot; as a bit input, we expect to see 1's
      // and 0's from each stream, so we set the maximum intensity
      // (zmax) to the number of connections so after adding the
      // streams, the max will the the max of 1's from all streams.
      int numplots = (d_nconnections > 0) ? d_nconnections : 1;
      d_main_gui = new TimeRasterDisplayForm(numplots,
					     d_samp_rate,
					     d_rows, d_cols,
					     1, d_parent);

      if(d_name.size() > 0)
        set_title(d_name);

      // initialize update time to 10 times a second
      set_update_time(0.1);
    }

    void
    time_raster_sink_f_impl::exec_()
    {
      d_qApplication->exec();
    }

    QWidget*
    time_raster_sink_f_impl::qwidget()
    {
      return d_main_gui;
    }

#ifdef ENABLE_PYTHON
    PyObject*
    time_raster_sink_f_impl::pyqwidget()
    {
      PyObject *w = PyLong_FromVoidPtr((void*)d_main_gui);
      PyObject *retarg = Py_BuildValue("N", w);
      return retarg;
    }
#else
    void *
    time_raster_sink_f_impl::pyqwidget()
    {
      return NULL;
    }
#endif

    void
    time_raster_sink_f_impl::set_update_time(double t)
    {
      //convert update time to ticks
      gr::high_res_timer_type tps = gr::high_res_timer_tps();
      d_update_time = t * tps;
      d_main_gui->setUpdateTime(t);
      d_last_time = 0;
    }

    void
    time_raster_sink_f_impl::set_title(const std::string &title)
    {
      d_main_gui->setTitle(title.c_str());
    }

    void
    time_raster_sink_f_impl::set_line_label(int which, const std::string &label)
    {
      d_main_gui->setLineLabel(which, label.c_str());
    }

    void
    time_raster_sink_f_impl::set_line_color(int which, const std::string &color)
    {
      d_main_gui->setLineColor(which, color.c_str());
    }

    void
    time_raster_sink_f_impl::set_line_width(int which, int width)
    {
      d_main_gui->setLineWidth(which, width);
    }

    void
    time_raster_sink_f_impl::set_line_style(int which, Qt::PenStyle style)
    {
      d_main_gui->setLineStyle(which, style);
    }

    void
    time_raster_sink_f_impl::set_line_marker(int which, QwtSymbol::Style marker)
    {
      d_main_gui->setLineMarker(which, marker);
    }

    void
    time_raster_sink_f_impl::set_color_map(int which, const int color)
    {
      d_main_gui->setColorMap(which, color);
    }

    void
    time_raster_sink_f_impl::set_line_alpha(int which, double alpha)
    {
      d_main_gui->setAlpha(which, (int)(255.0*alpha));
    }

    void
    time_raster_sink_f_impl::set_size(int width, int height)
    {
      d_main_gui->resize(QSize(width, height));
    }

    void
    time_raster_sink_f_impl::set_samp_rate(const double samp_rate)
    {
      d_samp_rate = samp_rate;
      d_main_gui->setSampleRate(d_samp_rate);
    }

    void
    time_raster_sink_f_impl::set_num_rows(double rows)
    {
      gr::thread::scoped_lock lock(d_setlock);
      d_rows = rows;
      d_main_gui->setNumRows(rows);
    }

    void
    time_raster_sink_f_impl::set_num_cols(double cols)
    {
      if(d_cols != cols) {
        gr::thread::scoped_lock lock(d_setlock);

        d_qApplication->postEvent(d_main_gui,
                                  new TimeRasterSetSize(d_rows, cols));

        d_cols = cols;
        d_icols = static_cast<int>(ceil(d_cols));

        volk_free(d_tmpflt);
        d_tmpflt = (float*)volk_malloc(d_icols*sizeof(float),
                                       volk_get_alignment());
        memset(d_tmpflt, 0, d_icols*sizeof(float));

        for(int i = 0; i < d_nconnections+1; i++) {
          volk_free(d_residbufs[i]);
          d_residbufs[i] = (double*)volk_malloc(d_icols*sizeof(double),
                                                volk_get_alignment());
          memset(d_residbufs[i], 0, d_icols*sizeof(double));
        }
        reset();
      }
    }

    std::string
    time_raster_sink_f_impl::title()
    {
      return d_main_gui->title().toStdString();
    }

    std::string
    time_raster_sink_f_impl::line_label(int which)
    {
      return d_main_gui->lineLabel(which).toStdString();
    }

    std::string
    time_raster_sink_f_impl::line_color(int which)
    {
      return d_main_gui->lineColor(which).toStdString();
    }

    int
    time_raster_sink_f_impl::line_width(int which)
    {
      return d_main_gui->lineWidth(which);
    }

    int
    time_raster_sink_f_impl::line_style(int which)
    {
      return d_main_gui->lineStyle(which);
    }

    int
    time_raster_sink_f_impl::line_marker(int which)
    {
      return d_main_gui->lineMarker(which);
    }

    int
    time_raster_sink_f_impl::color_map(int which)
    {
      return d_main_gui->getColorMap(which);
    }

    double
    time_raster_sink_f_impl::line_alpha(int which)
    {
      return (double)(d_main_gui->markerAlpha(which))/255.0;
    }

    double
    time_raster_sink_f_impl::num_rows()
    {
      return d_main_gui->numRows();
    }

    double
    time_raster_sink_f_impl::num_cols()
    {
      return d_main_gui->numCols();
    }

    void
    time_raster_sink_f_impl::set_multiplier(const std::vector<float> &mult)
    {
      if(mult.size() == 0) {
	for(int i = 0; i < d_nconnections; i++) {
	  d_mult[i] = 1.0f;
	}
      }
      else if(mult.size() == (size_t)d_nconnections) {
	for(int i = 0; i < d_nconnections; i++) {
	  d_mult[i] = mult[i];
	}
      }
      else {
	throw std::runtime_error("time_raster_sink_f_impl::set_multiplier incorrect dimensions.\n");
      }
    }

    void
    time_raster_sink_f_impl::set_offset(const std::vector<float> &offset)
    {
      if(offset.size() == 0) {
	for(int i = 0; i < d_nconnections; i++) {
	  d_offset[i] = 0.0f;
	}
      }
      else if(offset.size() == (size_t)d_nconnections) {
	for(int i = 0; i < d_nconnections; i++) {
	  d_offset[i] = offset[i];
	}
      }
      else {
	throw std::runtime_error("time_raster_sink_f_impl::set_offset incorrect dimensions.\n");
      }
    }

    void
    time_raster_sink_f_impl::set_intensity_range(float min, float max)
    {
      d_main_gui->setIntensityRange(min, max);
    }

    void
    time_raster_sink_f_impl::enable_menu(bool en)
    {
      d_main_gui->enableMenu(en);
    }

    void
    time_raster_sink_f_impl::enable_grid(bool en)
    {
      d_main_gui->setGrid(en);
    }

    void
    time_raster_sink_f_impl::enable_axis_labels(bool en)
    {
        d_main_gui->setAxisLabels(en);
    }

    void
    time_raster_sink_f_impl::enable_autoscale(bool en)
    {
      d_main_gui->autoScale(en);
    }

    void
    time_raster_sink_f_impl::reset()
    {
      d_index = 0;
    }

    void
    time_raster_sink_f_impl::_ncols_resize()
    {
      double cols = d_main_gui->numCols();
      set_num_cols(cols);
    }

    int
    time_raster_sink_f_impl::work(int noutput_items,
				  gr_vector_const_void_star &input_items,
				  gr_vector_void_star &output_items)
    {
      int n=0, j=0, idx=0;
      const float *in = (const float*)input_items[0];

      _ncols_resize();

      for(int i = 0; i < noutput_items; i += d_icols) {
	unsigned int datasize = noutput_items - i;
	unsigned int resid = d_icols-d_index;
	idx = 0;

	// If we have enough input for one full plot, do it
	if(datasize >= resid) {

	  // Fill up residbufs with d_size number of items
	  for(n = 0; n < d_nconnections; n++) {
	    in = (const float*)input_items[idx++];

	    // Scale and add offset
	    volk_32f_s32f_multiply_32f(d_tmpflt, &in[j], d_mult[n], resid);
	    for(unsigned int s = 0; s < resid; s++)
	      d_tmpflt[s] = d_tmpflt[s] + d_offset[n];

	    volk_32f_convert_64f_u(&d_residbufs[n][d_index],
				   d_tmpflt, resid);
	  }

	  // Update the plot if its time
	  if(gr::high_res_timer_now() - d_last_time > d_update_time) {
	    d_last_time = gr::high_res_timer_now();
	    d_qApplication->postEvent(d_main_gui,
				      new TimeRasterUpdateEvent(d_residbufs, d_cols));
	  }

	  d_index = 0;
	  j += resid;
	}
	// Otherwise, copy what we received into the residbufs for next time
	// because we set the output_multiple, this should never need to be called
	else {
	  for(n = 0; n < d_nconnections; n++) {
	    in = (const float*)input_items[idx++];

	    // Scale and add offset
	    volk_32f_s32f_multiply_32f(d_tmpflt, &in[j], d_mult[n], datasize);
	    for(unsigned int s = 0; s < datasize; s++)
	      d_tmpflt[s] = d_tmpflt[s] + d_offset[n];

	    volk_32f_convert_64f(&d_residbufs[n][d_index],
				 d_tmpflt, datasize);
	  }
	  d_index += datasize;
	  j += datasize;
	}
      }

      return j;
    }

    void
    time_raster_sink_f_impl::handle_pdus(pmt::pmt_t msg)
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
        throw std::runtime_error("time_raster_sink_f: unknown data type "
                                 "of samples; must be float.");
      }

      // Plot if we're past the last update time
      if(gr::high_res_timer_now() - d_last_time > d_update_time) {
        d_last_time = gr::high_res_timer_now();

        _ncols_resize();

        d_rows = ceil(static_cast<double>(len) / static_cast<double>(d_cols));
        int irows = static_cast<int>(d_rows);

        d_qApplication->postEvent(d_main_gui,
                                  new TimeRasterSetSize(d_rows, d_cols));

        int idx = 0;
        for(int r = 0; r < irows; r++) {
          // Scale and add offset
          int cpy_len = std::min(static_cast<size_t>(d_cols), len - idx);
          memset(d_residbufs[d_nconnections], 0, d_cols*sizeof(double));
          volk_32f_s32f_multiply_32f(d_tmpflt, &in[idx], d_mult[d_nconnections], cpy_len);
          for(int c = 0; c < cpy_len; c++) {
            d_tmpflt[c] = d_tmpflt[c] + d_offset[d_nconnections];
          }

          volk_32f_convert_64f_u(d_residbufs[d_nconnections], d_tmpflt, cpy_len);

          d_qApplication->postEvent(d_main_gui,
                                    new TimeRasterUpdateEvent(d_residbufs, d_cols));
          idx += d_cols;
        }
      }
    }

  } /* namespace qtgui */
} /* namespace gr */

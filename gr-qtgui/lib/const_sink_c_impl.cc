/* -*- c++ -*- */
/*
 * Copyright 2012,2014-2015 Free Software Foundation, Inc.
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

#include "const_sink_c_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/prefs.h>
#include <string.h>
#include <volk/volk.h>
#include <qwt_symbol.h>

namespace gr {
  namespace qtgui {

    const_sink_c::sptr
    const_sink_c::make(int size,
		       const std::string &name,
		       int nconnections,
		       QWidget *parent)
    {
      return gnuradio::get_initial_sptr
	(new const_sink_c_impl(size, name, nconnections, parent));
    }

    const_sink_c_impl::const_sink_c_impl(int size,
					 const std::string &name,
					 int nconnections,
					 QWidget *parent)
      : sync_block("const_sink_c",
		   io_signature::make(0, nconnections, sizeof(gr_complex)),
		   io_signature::make(0, 0, 0)),
	d_size(size), d_buffer_size(2*size), d_name(name),
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
                      boost::bind(&const_sink_c_impl::handle_pdus, this, _1));

      for(int i = 0; i < d_nconnections; i++) {
	d_residbufs_real.push_back((double*)volk_malloc(d_buffer_size*sizeof(double),
                                                        volk_get_alignment()));
	d_residbufs_imag.push_back((double*)volk_malloc(d_buffer_size*sizeof(double),
                                                        volk_get_alignment()));
	memset(d_residbufs_real[i], 0, d_buffer_size*sizeof(double));
	memset(d_residbufs_imag[i], 0, d_buffer_size*sizeof(double));
      }

      // Used for PDU message input
      d_residbufs_real.push_back((double*)volk_malloc(d_buffer_size*sizeof(double),
                                                      volk_get_alignment()));
      d_residbufs_imag.push_back((double*)volk_malloc(d_buffer_size*sizeof(double),
                                                      volk_get_alignment()));
      memset(d_residbufs_real[d_nconnections], 0, d_buffer_size*sizeof(double));
      memset(d_residbufs_imag[d_nconnections], 0, d_buffer_size*sizeof(double));

      // Set alignment properties for VOLK
      const int alignment_multiple =
	volk_get_alignment() / sizeof(gr_complex);
      set_alignment(std::max(1,alignment_multiple));

      initialize();

      set_trigger_mode(TRIG_MODE_FREE, TRIG_SLOPE_POS, 0, 0);

      set_history(2);          // so we can look ahead for the trigger slope
   }

    const_sink_c_impl::~const_sink_c_impl()
    {
      if(!d_main_gui->isClosed())
        d_main_gui->close();

      // d_main_gui is a qwidget destroyed with its parent
      for(int i = 0; i < d_nconnections+1; i++) {
	volk_free(d_residbufs_real[i]);
	volk_free(d_residbufs_imag[i]);
      }

      delete d_argv;
    }

    bool
    const_sink_c_impl::check_topology(int ninputs, int noutputs)
    {
      return ninputs == d_nconnections;
    }

    void
    const_sink_c_impl::initialize()
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
      d_main_gui = new ConstellationDisplayForm(numplots, d_parent);
      d_main_gui->setNPoints(d_size);

      if(d_name.size() > 0)
        set_title(d_name);

      // initialize update time to 10 times a second
      set_update_time(0.1);
    }

    void
    const_sink_c_impl::exec_()
    {
      d_qApplication->exec();
    }

    QWidget*
    const_sink_c_impl::qwidget()
    {
      return d_main_gui;
    }

#ifdef ENABLE_PYTHON
    PyObject*
    const_sink_c_impl::pyqwidget()
    {
      PyObject *w = PyLong_FromVoidPtr((void*)d_main_gui);
      PyObject *retarg = Py_BuildValue("N", w);
      return retarg;
    }
#else
    void *
    const_sink_c_impl::pyqwidget()
    {
      return NULL;
    }
#endif

    void
    const_sink_c_impl::set_y_axis(double min, double max)
    {
      d_main_gui->setYaxis(min, max);
    }

    void
    const_sink_c_impl::set_x_axis(double min, double max)
    {
      d_main_gui->setXaxis(min, max);
    }

    void
    const_sink_c_impl::set_update_time(double t)
    {
      //convert update time to ticks
      gr::high_res_timer_type tps = gr::high_res_timer_tps();
      d_update_time = t * tps;
      d_main_gui->setUpdateTime(t);
      d_last_time = 0;
    }

    void
    const_sink_c_impl::set_title(const std::string &title)
    {
      d_main_gui->setTitle(title.c_str());
    }

    void
    const_sink_c_impl::set_line_label(int which, const std::string &label)
    {
      d_main_gui->setLineLabel(which, label.c_str());
    }

    void
    const_sink_c_impl::set_line_color(int which, const std::string &color)
    {
      d_main_gui->setLineColor(which, color.c_str());
    }

    void
    const_sink_c_impl::set_line_width(int which, int width)
    {
      d_main_gui->setLineWidth(which, width);
    }

    void
    const_sink_c_impl::set_line_style(int which, int style)
    {
      d_main_gui->setLineStyle(which, (Qt::PenStyle)style);
    }

    void
    const_sink_c_impl::set_line_marker(int which, int marker)
    {
      d_main_gui->setLineMarker(which, (QwtSymbol::Style)marker);
    }

    void
    const_sink_c_impl::set_line_alpha(int which, double alpha)
    {
      d_main_gui->setMarkerAlpha(which, (int)(255.0*alpha));
    }

    void
    const_sink_c_impl::set_trigger_mode(trigger_mode mode,
					trigger_slope slope,
					float level,
					int channel,
					const std::string &tag_key)
    {
      gr::thread::scoped_lock lock(d_setlock);

      d_trigger_mode = mode;
      d_trigger_slope = slope;
      d_trigger_level = level;
      d_trigger_channel = channel;
      d_trigger_tag_key = pmt::intern(tag_key);
      d_triggered = false;
      d_trigger_count = 0;

      d_main_gui->setTriggerMode(d_trigger_mode);
      d_main_gui->setTriggerSlope(d_trigger_slope);
      d_main_gui->setTriggerLevel(d_trigger_level);
      d_main_gui->setTriggerChannel(d_trigger_channel);
      d_main_gui->setTriggerTagKey(tag_key);

      _reset();
    }

    void
    const_sink_c_impl::set_size(int width, int height)
    {
      d_main_gui->resize(QSize(width, height));
    }

    std::string
    const_sink_c_impl::title()
    {
      return d_main_gui->title().toStdString();
    }

    std::string
    const_sink_c_impl::line_label(int which)
    {
      return d_main_gui->lineLabel(which).toStdString();
    }

    std::string
    const_sink_c_impl::line_color(int which)
    {
      return d_main_gui->lineColor(which).toStdString();
    }

    int
    const_sink_c_impl::line_width(int which)
    {
      return d_main_gui->lineWidth(which);
    }

    int
    const_sink_c_impl::line_style(int which)
    {
      return d_main_gui->lineStyle(which);
    }

    int
    const_sink_c_impl::line_marker(int which)
    {
      return d_main_gui->lineMarker(which);
    }

    double
    const_sink_c_impl::line_alpha(int which)
    {
      return (double)(d_main_gui->markerAlpha(which))/255.0;
    }

    void
    const_sink_c_impl::set_nsamps(const int newsize)
    {
      gr::thread::scoped_lock lock(d_setlock);

      if(newsize != d_size) {
	// Set new size and reset buffer index
	// (throws away any currently held data, but who cares?)
	d_size = newsize;
        d_buffer_size = 2*d_size;
	d_index = 0;

	// Resize residbuf and replace data
        // +1 to handle PDU message input buffers
	for(int i = 0; i < d_nconnections+1; i++) {
	  volk_free(d_residbufs_real[i]);
	  volk_free(d_residbufs_imag[i]);
	  d_residbufs_real[i] = (double*)volk_malloc(d_buffer_size*sizeof(double),
                                                     volk_get_alignment());
	  d_residbufs_imag[i] = (double*)volk_malloc(d_buffer_size*sizeof(double),
                                                     volk_get_alignment());

	  memset(d_residbufs_real[i], 0, d_buffer_size*sizeof(double));
	  memset(d_residbufs_imag[i], 0, d_buffer_size*sizeof(double));
	}

	d_main_gui->setNPoints(d_size);
        _reset();
      }
    }

    int
    const_sink_c_impl::nsamps() const
    {
      return d_size;
    }

    void
    const_sink_c_impl::enable_menu(bool en)
    {
      d_main_gui->enableMenu(en);
    }

    void
    const_sink_c_impl::enable_autoscale(bool en)
    {
      d_main_gui->autoScale(en);
    }

    void
    const_sink_c_impl::enable_grid(bool en)
    {
      d_main_gui->setGrid(en);
    }

    void
    const_sink_c_impl::enable_axis_labels(bool en)
    {
        d_main_gui->setAxisLabels(en);
    }

    void
    const_sink_c_impl::disable_legend()
    {
      d_main_gui->disableLegend();
    }

    void
    const_sink_c_impl::reset()
    {
      gr::thread::scoped_lock lock(d_setlock);
      _reset();
    }

    void
    const_sink_c_impl::_reset()
    {
      // Reset the start and end indices.
      d_start = 0;
      d_end = d_size;
      d_index = 0;

      // Reset the trigger.
      if(d_trigger_mode == TRIG_MODE_FREE) {
        d_triggered = true;
      }
      else {
        d_triggered = false;
      }
    }

    void
    const_sink_c_impl::_npoints_resize()
    {
      int newsize = d_main_gui->getNPoints();
      set_nsamps(newsize);
    }

    void
    const_sink_c_impl::_gui_update_trigger()
    {
      d_trigger_mode = d_main_gui->getTriggerMode();
      d_trigger_slope = d_main_gui->getTriggerSlope();
      d_trigger_level = d_main_gui->getTriggerLevel();
      d_trigger_channel = d_main_gui->getTriggerChannel();
      d_trigger_count = 0;

      std::string tagkey = d_main_gui->getTriggerTagKey();
      d_trigger_tag_key = pmt::intern(tagkey);
    }

    void
    const_sink_c_impl::_test_trigger_tags(int nitems)
    {
      int trigger_index;

      uint64_t nr = nitems_read(d_trigger_channel);
      std::vector<gr::tag_t> tags;
      get_tags_in_range(tags, d_trigger_channel,
                        nr, nr + nitems,
                        d_trigger_tag_key);
      if(tags.size() > 0) {
        d_triggered = true;
        trigger_index = tags[0].offset - nr;
        d_start = d_index + trigger_index;
        d_end = d_start + d_size;
        d_trigger_count = 0;
      }
    }

    void
    const_sink_c_impl::_test_trigger_norm(int nitems, gr_vector_const_void_star inputs)
    {
      int trigger_index;
      const gr_complex *in = (const gr_complex*)inputs[d_trigger_channel];
      for(trigger_index = 0; trigger_index < nitems; trigger_index++) {
        d_trigger_count++;

        // Test if trigger has occurred based on the input stream,
        // channel number, and slope direction
        if(_test_trigger_slope(&in[trigger_index])) {
          d_triggered = true;
          d_start = d_index + trigger_index;
          d_end = d_start + d_size;
          d_trigger_count = 0;
          break;
        }
      }

      // If using auto trigger mode, trigger periodically even
      // without a trigger event.
      if((d_trigger_mode == TRIG_MODE_AUTO) && (d_trigger_count > d_size)) {
        d_triggered = true;
        d_trigger_count = 0;
      }
    }

    bool
    const_sink_c_impl::_test_trigger_slope(const gr_complex *in) const
    {
      float x0, x1;
      x0 = abs(in[0]);
      x1 = abs(in[1]);

      if(d_trigger_slope == TRIG_SLOPE_POS)
        return ((x0 <= d_trigger_level) && (x1 > d_trigger_level));
      else
        return ((x0 >= d_trigger_level) && (x1 < d_trigger_level));
    }

    int
    const_sink_c_impl::work(int noutput_items,
			    gr_vector_const_void_star &input_items,
			    gr_vector_void_star &output_items)
    {
      int n=0;
      const gr_complex *in;

      _npoints_resize();
      _gui_update_trigger();

      int nfill = d_end - d_index;                 // how much room left in buffers
      int nitems = std::min(noutput_items, nfill); // num items we can put in buffers

      // If auto, normal, or tag trigger, look for the trigger
      if((d_trigger_mode != TRIG_MODE_FREE) && !d_triggered) {
        // trigger off a tag key (first one found)
        if(d_trigger_mode == TRIG_MODE_TAG) {
          _test_trigger_tags(nitems);
        }
        // Normal or Auto trigger
        else {
          _test_trigger_norm(nitems, input_items);
        }
      }

      // Copy data into the buffers.
      for(n = 0; n < d_nconnections; n++) {
        in = (const gr_complex*)input_items[n];
        volk_32fc_deinterleave_64f_x2(&d_residbufs_real[n][d_index],
                                      &d_residbufs_imag[n][d_index],
                                      &in[history()-1], nitems);
      }
      d_index += nitems;


      // If we have a trigger and a full d_size of items in the buffers, plot.
      if((d_triggered) && (d_index == d_end)) {
        // Copy data to be plotted to start of buffers.
        for(n = 0; n < d_nconnections; n++) {
          memmove(d_residbufs_real[n], &d_residbufs_real[n][d_start], d_size*sizeof(double));
          memmove(d_residbufs_imag[n], &d_residbufs_imag[n][d_start], d_size*sizeof(double));
        }

        // Plot if we are able to update
        if(gr::high_res_timer_now() - d_last_time > d_update_time) {
          d_last_time = gr::high_res_timer_now();
          d_qApplication->postEvent(d_main_gui,
                                    new ConstUpdateEvent(d_residbufs_real,
							 d_residbufs_imag,
							 d_size));
        }

        // We've plotting, so reset the state
        _reset();
      }

      // If we've filled up the buffers but haven't triggered, reset.
      if(d_index == d_end) {
        _reset();
      }

      return nitems;
    }

    void
    const_sink_c_impl::handle_pdus(pmt::pmt_t msg)
    {
      size_t len = 0;
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
        throw std::runtime_error("const_sink_c: message must be either "
                                 "a PDU or a uniform vector of samples.");
      }

      len = pmt::length(samples);

      const gr_complex *in;
      if(pmt::is_c32vector(samples)) {
        in = (const gr_complex*)pmt::c32vector_elements(samples, len);
      }
      else {
        throw std::runtime_error("const_sink_c: unknown data type "
                                 "of samples; must be complex.");
      }

      set_nsamps(len);

      // Plot if we're past the last update time
      if(gr::high_res_timer_now() - d_last_time > d_update_time) {
        d_last_time = gr::high_res_timer_now();

        // Copy data into the buffers.
        volk_32fc_deinterleave_64f_x2(d_residbufs_real[d_nconnections],
                                      d_residbufs_imag[d_nconnections],
                                      in, len);

        d_qApplication->postEvent(d_main_gui,
                                  new ConstUpdateEvent(d_residbufs_real,
                                                       d_residbufs_imag,
                                                       len));
      }
    }

  } /* namespace qtgui */
} /* namespace gr */

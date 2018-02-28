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

#include "freq_sink_c_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/prefs.h>
#include <string.h>
#include <volk/volk.h>
#include <qwt_symbol.h>

namespace gr {
  namespace qtgui {

    freq_sink_c::sptr
    freq_sink_c::make(int fftsize, int wintype,
		      double fc, double bw,
		      const std::string &name,
		      int nconnections,
		      QWidget *parent)
    {
      return gnuradio::get_initial_sptr
	(new freq_sink_c_impl(fftsize, wintype,
			      fc, bw, name,
			      nconnections,
			      parent));
    }

    freq_sink_c_impl::freq_sink_c_impl(int fftsize, int wintype,
				       double fc, double bw,
				       const std::string &name,
				       int nconnections,
				       QWidget *parent)
      : sync_block("freq_sink_c",
                   io_signature::make(0, nconnections, sizeof(gr_complex)),
                   io_signature::make(0, 0, 0)),
	d_fftsize(fftsize), d_fftavg(1.0),
	d_wintype((filter::firdes::win_type)(wintype)),
  d_center_freq(fc), d_bandwidth(bw), d_name(name),
  d_nconnections(nconnections),
  d_port(pmt::mp("freq")),
  d_parent(parent)
    {
      // Required now for Qt; argc must be greater than 0 and argv
      // must have at least one valid character. Must be valid through
      // life of the qApplication:
      // http://harmattan-dev.nokia.com/docs/library/html/qt4/qapplication.html
      d_argc = 1;
      d_argv = new char;
      d_argv[0] = '\0';

      // setup output message port to post frequency when display is
      // double-clicked
      message_port_register_out(d_port);
      message_port_register_in(d_port);
      set_msg_handler(d_port,
                      boost::bind(&freq_sink_c_impl::handle_set_freq, this, _1));

      // setup PDU handling input port
      message_port_register_in(pmt::mp("in"));
      set_msg_handler(pmt::mp("in"),
                      boost::bind(&freq_sink_c_impl::handle_pdus, this, _1));

      d_main_gui = NULL;

      // Perform fftshift operation;
      // this is usually desired when plotting
      d_shift = true;

      d_fft = new fft::fft_complex(d_fftsize, true);
      d_fbuf = (float*)volk_malloc(d_fftsize*sizeof(float),
                                   volk_get_alignment());
      memset(d_fbuf, 0, d_fftsize*sizeof(float));

      d_tmpbuflen = (unsigned int)(floor(d_fftsize/2.0));
      d_tmpbuf = (float*)volk_malloc(sizeof(float)*(d_tmpbuflen + 1),
                                     volk_get_alignment());


      d_index = 0;
      // save the last "connection" for the PDU memory
      for(int i = 0; i < d_nconnections; i++) {
	d_residbufs.push_back((gr_complex*)volk_malloc(d_fftsize*sizeof(gr_complex),
                                                       volk_get_alignment()));
	d_magbufs.push_back((double*)volk_malloc(d_fftsize*sizeof(double),
                                                 volk_get_alignment()));

	memset(d_residbufs[i], 0, d_fftsize*sizeof(gr_complex));
	memset(d_magbufs[i], 0, d_fftsize*sizeof(double));
      }

      d_residbufs.push_back((gr_complex*)volk_malloc(d_fftsize*sizeof(gr_complex),
                                                     volk_get_alignment()));
      d_pdu_magbuf = (double*)volk_malloc(d_fftsize*sizeof(double),
                                          volk_get_alignment());
      d_magbufs.push_back(d_pdu_magbuf);
      memset(d_residbufs[d_nconnections], 0, d_fftsize*sizeof(gr_complex));
      memset(d_pdu_magbuf, 0, d_fftsize*sizeof(double));

      buildwindow();

      initialize();

      set_trigger_mode(TRIG_MODE_FREE, 0, 0);
    }

    freq_sink_c_impl::~freq_sink_c_impl()
    {
      if(!d_main_gui->isClosed())
        d_main_gui->close();

      // +1 to handle PDU buffers; will also take care of d_pdu_magbuf
      for(int i = 0; i < d_nconnections+1; i++) {
	volk_free(d_residbufs[i]);
	volk_free(d_magbufs[i]);
      }
      delete d_fft;
      volk_free(d_fbuf);
      volk_free(d_tmpbuf);

      delete d_argv;
    }

    bool
    freq_sink_c_impl::check_topology(int ninputs, int noutputs)
    {
      return ninputs == d_nconnections;
    }

    void
    freq_sink_c_impl::initialize()
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
      d_main_gui = new FreqDisplayForm(numplots, d_parent);
      set_fft_window(d_wintype);
      set_fft_size(d_fftsize);
      set_frequency_range(d_center_freq, d_bandwidth);

      if(d_name.size() > 0)
        set_title(d_name);

      set_output_multiple(d_fftsize);

      // initialize update time to 10 times a second
      set_update_time(0.1);
    }

    void
    freq_sink_c_impl::exec_()
    {
      d_qApplication->exec();
    }

    QWidget*
    freq_sink_c_impl::qwidget()
    {
      return d_main_gui;
    }

#ifdef ENABLE_PYTHON
    PyObject*
    freq_sink_c_impl::pyqwidget()
    {
      PyObject *w = PyLong_FromVoidPtr((void*)d_main_gui);
      PyObject *retarg = Py_BuildValue("N", w);
      return retarg;
    }
#else
    void *
    freq_sink_c_impl::pyqwidget()
    {
      return NULL;
    }
#endif

    void
    freq_sink_c_impl::set_fft_size(const int fftsize)
    {
      if((fftsize > 16) && (fftsize < 16384))
        d_main_gui->setFFTSize(fftsize);
      else
        throw std::runtime_error("freq_sink: FFT size must be > 16 and < 16384.");
    }

    int
    freq_sink_c_impl::fft_size() const
    {
      return d_fftsize;
    }

    void
    freq_sink_c_impl::set_fft_average(const float fftavg)
    {
      d_main_gui->setFFTAverage(fftavg);
    }

    float
    freq_sink_c_impl::fft_average() const
    {
      return d_fftavg;
    }

    void
    freq_sink_c_impl::set_fft_window(const filter::firdes::win_type win)
    {
      d_main_gui->setFFTWindowType(win);
    }

    filter::firdes::win_type
    freq_sink_c_impl::fft_window()
    {
      return d_wintype;
    }

    void
    freq_sink_c_impl::set_frequency_range(const double centerfreq,
					  const double bandwidth)
    {
      d_center_freq = centerfreq;
      d_bandwidth = bandwidth;
      d_main_gui->setFrequencyRange(d_center_freq, d_bandwidth);
    }

    void
    freq_sink_c_impl::set_y_axis(double min, double max)
    {
      d_main_gui->setYaxis(min, max);
    }

    void
    freq_sink_c_impl::set_y_label(const std::string &label,
                                  const std::string &unit)
    {
        d_main_gui->setYLabel(label, unit);
    }

    void
    freq_sink_c_impl::set_update_time(double t)
    {
      //convert update time to ticks
      gr::high_res_timer_type tps = gr::high_res_timer_tps();
      d_update_time = t * tps;
      d_main_gui->setUpdateTime(t);
      d_last_time = 0;
    }

    void
    freq_sink_c_impl::set_title(const std::string &title)
    {
      d_main_gui->setTitle(title.c_str());
    }

    void
    freq_sink_c_impl::set_line_label(int which, const std::string &label)
    {
      d_main_gui->setLineLabel(which, label.c_str());
    }

    void
    freq_sink_c_impl::set_line_color(int which, const std::string &color)
    {
      d_main_gui->setLineColor(which, color.c_str());
    }

    void
    freq_sink_c_impl::set_line_width(int which, int width)
    {
      d_main_gui->setLineWidth(which, width);
    }

    void
    freq_sink_c_impl::set_line_style(int which, int style)
    {
      d_main_gui->setLineStyle(which, (Qt::PenStyle)style);
    }

    void
    freq_sink_c_impl::set_line_marker(int which, int marker)
    {
      d_main_gui->setLineMarker(which, (QwtSymbol::Style)marker);
    }

    void
    freq_sink_c_impl::set_line_alpha(int which, double alpha)
    {
      d_main_gui->setMarkerAlpha(which, (int)(255.0*alpha));
    }

    void
    freq_sink_c_impl::set_size(int width, int height)
    {
      d_main_gui->resize(QSize(width, height));
    }

    void
    freq_sink_c_impl::set_trigger_mode(trigger_mode mode,
                                       float level,
                                       int channel,
                                       const std::string &tag_key)
    {
      gr::thread::scoped_lock lock(d_setlock);

      d_trigger_mode = mode;
      d_trigger_level = level;
      d_trigger_channel = channel;
      d_trigger_tag_key = pmt::intern(tag_key);
      d_triggered = false;
      d_trigger_count = 0;

      d_main_gui->setTriggerMode(d_trigger_mode);
      d_main_gui->setTriggerLevel(d_trigger_level);
      d_main_gui->setTriggerChannel(d_trigger_channel);
      d_main_gui->setTriggerTagKey(tag_key);

      _reset();
    }

    std::string
    freq_sink_c_impl::title()
    {
      return d_main_gui->title().toStdString();
    }

    std::string
    freq_sink_c_impl::line_label(int which)
    {
      return d_main_gui->lineLabel(which).toStdString();
    }

    std::string
    freq_sink_c_impl::line_color(int which)
    {
      return d_main_gui->lineColor(which).toStdString();
    }

    int
    freq_sink_c_impl::line_width(int which)
    {
      return d_main_gui->lineWidth(which);
    }

    int
    freq_sink_c_impl::line_style(int which)
    {
      return d_main_gui->lineStyle(which);
    }

    int
    freq_sink_c_impl::line_marker(int which)
    {
      return d_main_gui->lineMarker(which);
    }

    double
    freq_sink_c_impl::line_alpha(int which)
    {
      return (double)(d_main_gui->markerAlpha(which))/255.0;
    }

    void
    freq_sink_c_impl::enable_menu(bool en)
    {
      d_main_gui->enableMenu(en);
    }

    void
    freq_sink_c_impl::enable_grid(bool en)
    {
      d_main_gui->setGrid(en);
    }

    void
    freq_sink_c_impl::enable_autoscale(bool en)
    {
      d_main_gui->autoScale(en);
    }

    void
    freq_sink_c_impl::enable_axis_labels(bool en)
    {
        d_main_gui->setAxisLabels(en);
    }

    void
    freq_sink_c_impl::enable_control_panel(bool en)
    {
      if(en)
        d_main_gui->setupControlPanel();
      else
        d_main_gui->teardownControlPanel();
    }

    void
    freq_sink_c_impl::enable_max_hold(bool en)
    {
      d_main_gui->notifyMaxHold(en);
    }

    void
    freq_sink_c_impl::enable_min_hold(bool en)
    {
      d_main_gui->notifyMinHold(en);
    }

    void
    freq_sink_c_impl::clear_max_hold()
    {
      d_main_gui->clearMaxHold();
    }

    void
    freq_sink_c_impl::clear_min_hold()
    {
      d_main_gui->clearMinHold();
    }

    void
    freq_sink_c_impl::disable_legend()
    {
      d_main_gui->disableLegend();
    }

    void
    freq_sink_c_impl::reset()
    {
      gr::thread::scoped_lock lock(d_setlock);
      _reset();
    }

    void
    freq_sink_c_impl::_reset()
    {
      d_trigger_count = 0;

      // Reset the trigger.
      if(d_trigger_mode == TRIG_MODE_FREE) {
        d_triggered = true;
      }
      else {
        d_triggered = false;
      }
    }


    void
    freq_sink_c_impl::fft(float *data_out, const gr_complex *data_in, int size)
    {
      if(d_window.size()) {
	volk_32fc_32f_multiply_32fc(d_fft->get_inbuf(), data_in,
                                    &d_window.front(), size);
      }
      else {
	memcpy(d_fft->get_inbuf(), data_in, sizeof(gr_complex)*size);
      }

      d_fft->execute();     // compute the fft

      volk_32fc_s32f_x2_power_spectral_density_32f(data_out, d_fft->get_outbuf(),
                                                   size, 1.0, size);

      // Perform shift operation
      memcpy(d_tmpbuf, &data_out[0], sizeof(float)*(d_tmpbuflen + 1));
      memcpy(&data_out[0], &data_out[size - d_tmpbuflen], sizeof(float)*(d_tmpbuflen));
      memcpy(&data_out[d_tmpbuflen], d_tmpbuf, sizeof(float)*(d_tmpbuflen + 1));
    }

    bool
    freq_sink_c_impl::windowreset()
    {
      gr::thread::scoped_lock lock(d_setlock);

      filter::firdes::win_type newwintype;
      newwintype = d_main_gui->getFFTWindowType();
      if(d_wintype != newwintype) {
        d_wintype = newwintype;
        buildwindow();
        return true;
      }
      return false;
    }

    void
    freq_sink_c_impl::buildwindow()
    {
      d_window.clear();
      if(d_wintype != filter::firdes::WIN_NONE) {
	d_window = filter::firdes::window(d_wintype, d_fftsize, 6.76);
      }
    }

    bool
    freq_sink_c_impl::fftresize()
    {
      gr::thread::scoped_lock lock(d_setlock);

      int newfftsize = d_main_gui->getFFTSize();
      d_fftavg = d_main_gui->getFFTAverage();

      if(newfftsize != d_fftsize) {
	// Resize residbuf and replace data
        // +1 to handle PDU buffers
	for(int i = 0; i < d_nconnections+1; i++) {
	  volk_free(d_residbufs[i]);
	  volk_free(d_magbufs[i]);

	  d_residbufs[i] = (gr_complex*)volk_malloc(newfftsize*sizeof(gr_complex),
                                                    volk_get_alignment());
	  d_magbufs[i] = (double*)volk_malloc(newfftsize*sizeof(double),
                                              volk_get_alignment());

	  memset(d_residbufs[i], 0, newfftsize*sizeof(gr_complex));
	  memset(d_magbufs[i], 0, newfftsize*sizeof(double));
	}

        // Update the pointer to the newly allocated memory
        d_pdu_magbuf = d_magbufs[d_nconnections];

	// Set new fft size and reset buffer index
	// (throws away any currently held data, but who cares?)
	d_fftsize = newfftsize;
	d_index = 0;

	// Reset window to reflect new size
	buildwindow();

	// Reset FFTW plan for new size
	delete d_fft;
	d_fft = new fft::fft_complex(d_fftsize, true);

	volk_free(d_fbuf);
	d_fbuf = (float*)volk_malloc(d_fftsize*sizeof(float),
                                     volk_get_alignment());
	memset(d_fbuf, 0, d_fftsize*sizeof(float));

	volk_free(d_tmpbuf);
        d_tmpbuflen = (unsigned int)(floor(d_fftsize/2.0));
        d_tmpbuf = (float*)volk_malloc(sizeof(float)*(d_tmpbuflen + 1),
                                       volk_get_alignment());

        d_last_time = 0;

        set_output_multiple(d_fftsize);

        return true;
      }
      return false;
    }

    void
    freq_sink_c_impl::check_clicked()
    {
      if(d_main_gui->checkClicked()) {
        double freq = d_main_gui->getClickedFreq();
        message_port_pub(d_port,
                         pmt::cons(d_port,
                                   pmt::from_double(freq)));
      }
    }

    void
    freq_sink_c_impl::handle_set_freq(pmt::pmt_t msg)
    {
      if(pmt::is_pair(msg)) {
        pmt::pmt_t x = pmt::cdr(msg);
        if(pmt::is_real(x)) {
          d_center_freq = pmt::to_double(x);
          d_qApplication->postEvent(d_main_gui,
                                    new SetFreqEvent(d_center_freq, d_bandwidth));
        }
      }
    }

    void
    freq_sink_c_impl::_gui_update_trigger()
    {
      trigger_mode new_trigger_mode = d_main_gui->getTriggerMode();
      d_trigger_level = d_main_gui->getTriggerLevel();
      d_trigger_channel = d_main_gui->getTriggerChannel();

      std::string tagkey = d_main_gui->getTriggerTagKey();
      d_trigger_tag_key = pmt::intern(tagkey);

      if(new_trigger_mode != d_trigger_mode) {
        d_trigger_mode = new_trigger_mode;
        _reset();
      }
    }

    void
    freq_sink_c_impl::_test_trigger_tags(int start, int nitems)
    {
      uint64_t nr = nitems_read(d_trigger_channel);
      std::vector<gr::tag_t> tags;
      get_tags_in_range(tags, d_trigger_channel,
                        nr+start, nr+start+nitems,
                        d_trigger_tag_key);
      if(tags.size() > 0) {
        d_triggered = true;
        d_index = tags[0].offset - nr;
        d_trigger_count = 0;
      }
    }

    void
    freq_sink_c_impl::_test_trigger_norm(int nitems, std::vector<double*> inputs)
    {
      const double *in = (const double*)inputs[d_trigger_channel];
      for(int i = 0; i < nitems; i++) {
        d_trigger_count++;

        // Test if trigger has occurred based on the FFT magnitude and
        // channel number. Test if any value is > the level (in dBx).
        if(in[i] > d_trigger_level) {
          d_triggered = true;
          d_trigger_count = 0;
          break;
        }
      }

      // If using auto trigger mode, trigger periodically even
      // without a trigger event.
      if((d_trigger_mode == TRIG_MODE_AUTO) && (d_trigger_count > d_fftsize)) {
        d_triggered = true;
        d_trigger_count = 0;
      }
    }

    int
    freq_sink_c_impl::work(int noutput_items,
			   gr_vector_const_void_star &input_items,
			   gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex*)input_items[0];

      // Update the FFT size from the application
      bool updated = false;
      updated |= fftresize();
      updated |= windowreset();
      if(updated)
        return 0;

      check_clicked();
      _gui_update_trigger();

      gr::thread::scoped_lock lock(d_setlock);
      for(d_index = 0; d_index < noutput_items; d_index+=d_fftsize) {

        if((gr::high_res_timer_now() - d_last_time) > d_update_time) {

          // Trigger off tag, if active
          if((d_trigger_mode == TRIG_MODE_TAG) && !d_triggered) {
            _test_trigger_tags(d_index, d_fftsize);
            if(d_triggered) {
              // If not enough from tag position, early exit
              if((d_index + d_fftsize) >= noutput_items)
                return d_index;
            }
          }

          // Perform FFT and shift operations into d_magbufs
          for(int n = 0; n < d_nconnections; n++) {
            in = (const gr_complex*)input_items[n];
            memcpy(d_residbufs[n], &in[d_index], sizeof(gr_complex)*d_fftsize);

            fft(d_fbuf, d_residbufs[n], d_fftsize);
            for(int x = 0; x < d_fftsize; x++) {
              d_magbufs[n][x] = (double)((1.0-d_fftavg)*d_magbufs[n][x] + (d_fftavg)*d_fbuf[x]);
            }
            //volk_32f_convert_64f(d_magbufs[n], d_fbuf, d_fftsize);
          }

          // Test trigger off signal power in d_magbufs
          if((d_trigger_mode == TRIG_MODE_NORM) || (d_trigger_mode == TRIG_MODE_AUTO)) {
            _test_trigger_norm(d_fftsize, d_magbufs);
          }

          // If a trigger (FREE always triggers), plot and reset state
          if(d_triggered) {
            d_last_time = gr::high_res_timer_now();
            d_qApplication->postEvent(d_main_gui,
                                      new FreqUpdateEvent(d_magbufs, d_fftsize));
            _reset();
          }
        }
      }

      return noutput_items;
    }


    void
    freq_sink_c_impl::handle_pdus(pmt::pmt_t msg)
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

      const gr_complex *in;
      if(pmt::is_c32vector(samples)) {
        in = (const gr_complex*)pmt::c32vector_elements(samples, len);
      }
      else {
        throw std::runtime_error("freq_sink_c: unknown data type "
                                 "of samples; must be complex.");
      }

      // Plot if we're past the last update time
      if(gr::high_res_timer_now() - d_last_time > d_update_time) {
        d_last_time = gr::high_res_timer_now();

        // Update the FFT size from the application
        fftresize();
        windowreset();
        check_clicked();

        int winoverlap = 4;
        int fftoverlap = d_fftsize / winoverlap;
        float num = static_cast<float>(winoverlap * len) / static_cast<float>(d_fftsize);
        int nffts = static_cast<int>(ceilf(num));

        // Clear this as we will be accumulating in the for loop over nffts
        memset(d_pdu_magbuf, 0, sizeof(double)*d_fftsize);

        size_t min = 0;
        size_t max = std::min(d_fftsize, static_cast<int>(len));
        for(int n = 0; n < nffts; n++) {
          // Clear in case (max-min) < d_fftsize
          memset(d_residbufs[d_nconnections], 0x00, sizeof(gr_complex)*d_fftsize);

          // Copy in as much of the input samples as we can
          memcpy(d_residbufs[d_nconnections], &in[min], sizeof(gr_complex)*(max-min));

          // Apply the window and FFT; copy data into the PDU
          // magnitude buffer.
          fft(d_fbuf, d_residbufs[d_nconnections], d_fftsize);
          for(int x = 0; x < d_fftsize; x++) {
            d_pdu_magbuf[x] += (double)d_fbuf[x];
          }

          // Increment our indices; set max up to the number of
          // samples in the input PDU.
          min += fftoverlap;
          max = std::min(max + fftoverlap, len);
        }

        // Perform the averaging
        for(int x = 0; x < d_fftsize; x++) {
          d_pdu_magbuf[x] /= static_cast<double>(nffts);
        }

        //update gui per-pdu
        d_qApplication->postEvent(d_main_gui,
                                  new FreqUpdateEvent(d_magbufs, d_fftsize));
      }
    }

  } /* namespace qtgui */
} /* namespace gr */

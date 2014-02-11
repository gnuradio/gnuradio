/* -*- c++ -*- */
/*
 * Copyright 2008-2012 Free Software Foundation, Inc.
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

#include "sink_c_impl.h"
#include <gnuradio/io_signature.h>
#include <string.h>
#include <volk/volk.h>

namespace gr {
  namespace qtgui {

    sink_c::sptr
    sink_c::make(int fftsize, int wintype,
		 double fc, double bw,
		 const std::string &name,
		 bool plotfreq, bool plotwaterfall,
		 bool plottime, bool plotconst,
		 QWidget *parent)
    {
      return gnuradio::get_initial_sptr
	(new sink_c_impl(fftsize, wintype,
			 fc, bw, name,
			 plotfreq, plotwaterfall,
			 plottime, plotconst,
			 parent));
    }

    sink_c_impl::sink_c_impl(int fftsize, int wintype,
			     double fc, double bw,
			     const std::string &name,
			     bool plotfreq, bool plotwaterfall,
			     bool plottime, bool plotconst,
			     QWidget *parent)
      : block("sink_c",
		 io_signature::make(1, -1, sizeof(gr_complex)),
		 io_signature::make(0, 0, 0)),
	d_fftsize(fftsize),
	d_wintype((filter::firdes::win_type)(wintype)),
	d_center_freq(fc), d_bandwidth(bw), d_name(name),
	d_plotfreq(plotfreq), d_plotwaterfall(plotwaterfall),
	d_plottime(plottime), d_plotconst(plotconst),
	d_parent(parent)
    {
      // Required now for Qt; argc must be greater than 0 and argv
      // must have at least one valid character. Must be valid through
      // life of the qApplication:
      // http://harmattan-dev.nokia.com/docs/library/html/qt4/qapplication.html
      d_argc = 1;
      d_argv = new char;
      d_argv[0] = '\0';

      d_main_gui = NULL;

      // Perform fftshift operation;
      // this is usually desired when plotting
      d_shift = true;

      d_fft = new fft::fft_complex(d_fftsize, true);

      d_index = 0;
      d_residbuf = (gr_complex*)volk_malloc(d_fftsize*sizeof(gr_complex),
                                            volk_get_alignment());
      d_magbuf = (float*)volk_malloc(d_fftsize*sizeof(float),
                                     volk_get_alignment());

      buildwindow();

      initialize();
    }

    sink_c_impl::~sink_c_impl()
    {
      delete d_main_gui;
      delete d_fft;
      delete d_argv;
      volk_free(d_residbuf);
      volk_free(d_magbuf);
    }

    bool
    sink_c_impl::check_topology(int ninputs, int noutputs)
    {
      return ninputs == 1;
    }

    void
    sink_c_impl::forecast(int noutput_items, gr_vector_int &ninput_items_required)
    {
      unsigned int ninputs = ninput_items_required.size();
      for(unsigned int i = 0; i < ninputs; i++) {
	ninput_items_required[i] = std::min(d_fftsize, 8191);
      }
    }

    void
    sink_c_impl::initialize()
    {
      if(qApp != NULL) {
	d_qApplication = qApp;
      }
      else {
	d_qApplication = new QApplication(d_argc, &d_argv);
      }

      if(d_center_freq < 0) {
	throw std::runtime_error("sink_c_impl: Received bad center frequency.\n");
      }

      uint64_t maxBufferSize = 32768;
      d_main_gui = new SpectrumGUIClass(maxBufferSize, d_fftsize,
					d_center_freq,
					-d_bandwidth,
					d_bandwidth);

      d_main_gui->setDisplayTitle(d_name);
      d_main_gui->setWindowType((int)d_wintype);
      set_fft_size(d_fftsize);

      d_main_gui->openSpectrumWindow(d_parent,
				     d_plotfreq, d_plotwaterfall,
				     d_plottime, d_plotconst);

      // initialize update time to 10 times a second
      set_update_time(0.5);

      d_last_update = gr::high_res_timer_now();
      d_update_active = false;
    }

    void
    sink_c_impl::exec_()
    {
      d_qApplication->exec();
    }

    QWidget*
    sink_c_impl::qwidget()
    {
      return d_main_gui->qwidget();
    }

#ifdef ENABLE_PYTHON
    PyObject*
    sink_c_impl::pyqwidget()
    {
      PyObject *w = PyLong_FromVoidPtr((void*)d_main_gui->qwidget());
      PyObject *retarg = Py_BuildValue("N", w);
      return retarg;
    }
#endif

    void
    sink_c_impl::set_fft_size(const int fftsize)
    {
      d_fftsize = fftsize;
      d_main_gui->setFFTSize(fftsize);
    }

    int
    sink_c_impl::fft_size() const
    {
      return d_fftsize;
    }

    void
    sink_c_impl::set_frequency_range(const double centerfreq,
				     const double bandwidth)
    {
      d_center_freq = centerfreq;
      d_bandwidth = bandwidth;
      d_main_gui->setFrequencyRange(d_center_freq,
				    -d_bandwidth,
				    d_bandwidth);
    }

    void
    sink_c_impl::set_fft_power_db(double min, double max)
    {
      d_main_gui->setFrequencyAxis(min, max);
    }

    /*
    void
    sink_c_impl::set_time_domain_axis(double min, double max)
    {
      d_main_gui->setTimeDomainAxis(min, max);
    }

    void
    sink_c_impl::set_constellation_axis(double xmin, double xmax,
					double ymin, double ymax)
    {
      d_main_gui->setConstellationAxis(xmin, xmax, ymin, ymax);
    }

    void
    sink_c_impl::set_constellation_pen_size(int size)
    {
      d_main_gui->setConstellationPenSize(size);
    }
    */

    void
    sink_c_impl::set_update_time(double t)
    {
      d_update_time = t * gr::high_res_timer_tps();
      d_main_gui->setUpdateTime(t);
    }

    void
    sink_c_impl::fft(float *data_out, const gr_complex *data_in, int size)
    {
      if (d_window.size()) {
	volk_32fc_32f_multiply_32fc_a(d_fft->get_inbuf(), data_in,
				      &d_window.front(), size);
      }
      else {
	memcpy (d_fft->get_inbuf(), data_in, sizeof(gr_complex)*size);
      }

      d_fft->execute ();     // compute the fft
      volk_32fc_s32f_x2_power_spectral_density_32f_a(data_out, d_fft->get_outbuf(),
						     size, 1.0, size);
}

    void
    sink_c_impl::windowreset()
    {
      filter::firdes::win_type newwintype;
      newwintype = (filter::firdes::win_type)d_main_gui->getWindowType();
      if(d_wintype != newwintype) {
	d_wintype = newwintype;
	buildwindow();
      }
    }

    void
    sink_c_impl::buildwindow()
    {
      d_window.clear();
      if(d_wintype != 0) {
	d_window = filter::firdes::window(d_wintype, d_fftsize, 6.76);
      }
    }

    void
    sink_c_impl::fftresize()
    {
      int newfftsize = d_main_gui->getFFTSize();

      if(newfftsize != d_fftsize) {

	// Resize residbuf and replace data
        volk_free(d_residbuf);
        d_residbuf = (gr_complex*)volk_malloc(newfftsize*sizeof(gr_complex),
                                              volk_get_alignment());

        volk_free(d_magbuf);
        d_magbuf = (float*)volk_malloc(newfftsize*sizeof(float),
                                       volk_get_alignment());

	// Set new fft size and reset buffer index
	// (throws away any currently held data, but who cares?)
	d_fftsize = newfftsize;
	d_index = 0;

	// Reset window to reflect new size
	buildwindow();

	// Reset FFTW plan for new size
	delete d_fft;
	d_fft = new fft::fft_complex (d_fftsize, true);
      }
    }

    int
    sink_c_impl::general_work(int noutput_items,
			      gr_vector_int &ninput_items,
			      gr_vector_const_void_star &input_items,
			      gr_vector_void_star &output_items)
    {
      int j=0;
      const gr_complex *in = (const gr_complex*)input_items[0];

      // Update the FFT size from the application
      fftresize();
      windowreset();

      for(int i=0; i < noutput_items; i+=d_fftsize) {
	unsigned int datasize = noutput_items - i;
	unsigned int resid = d_fftsize-d_index;

	if (!d_update_active && (gr::high_res_timer_now() - d_last_update) < d_update_time) {
	  consume_each(noutput_items);
	  return noutput_items;
	}
	else {
	  d_last_update = gr::high_res_timer_now();
	  d_update_active = true;
	}

	// If we have enough input for one full FFT, do it
	if(datasize >= resid) {
	  const gr::high_res_timer_type currentTime = gr::high_res_timer_now();

	  // Fill up residbuf with d_fftsize number of items
	  memcpy(d_residbuf+d_index, &in[j], sizeof(gr_complex)*resid);
	  d_index = 0;

	  j += resid;
	  fft(d_magbuf, d_residbuf, d_fftsize);

	  d_main_gui->updateWindow(true, d_magbuf, d_fftsize,
				   NULL, 0, (float*)d_residbuf, d_fftsize,
				   currentTime, true);
	  d_update_active = false;
	}
	// Otherwise, copy what we received into the residbuf for next time
	else {
	  memcpy(d_residbuf+d_index, &in[j], sizeof(gr_complex)*datasize);
	  d_index += datasize;
	  j += datasize;
	}
      }

      consume_each(j);
      return j;
    }

  } /* namespace qtgui */
} /* namespace gr */

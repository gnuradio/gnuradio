/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#include "waterfall_sink_c_impl.h"
#include <gr_io_signature.h>
#include <string.h>
#include <volk/volk.h>

namespace gr {
  namespace qtgui {
    
    waterfall_sink_c::sptr
    waterfall_sink_c::make(int fftsize, int wintype,
			   double fc, double bw,
			   const std::string &name,
			   QWidget *parent)
    {
      return gnuradio::get_initial_sptr
	(new waterfall_sink_c_impl(fftsize, wintype,
				   fc, bw, name,
				   parent));
    }

    waterfall_sink_c_impl::waterfall_sink_c_impl(int fftsize, int wintype,
						 double fc, double bw,
						 const std::string &name,
						 QWidget *parent)
      : gr_sync_block("waterfall_sink_c",
		      gr_make_io_signature(1, -1, sizeof(gr_complex)),
		      gr_make_io_signature(0, 0, 0)),
	d_fftsize(fftsize), d_fftavg(1.0),
	d_wintype((filter::firdes::win_type)(wintype)),
	d_center_freq(fc), d_bandwidth(bw), d_name(name),
	d_nconnections(1), d_parent(parent)
    {
      d_main_gui = NULL;

      // Perform fftshift operation;
      // this is usually desired when plotting
      d_shift = true;

      d_fft = new fft::fft_complex(d_fftsize, true);
      d_fbuf = fft::malloc_float(d_fftsize);
      memset(d_fbuf, 0, d_fftsize*sizeof(float));

      d_index = 0;
      for(int i = 0; i < d_nconnections; i++) {
	d_residbufs.push_back(fft::malloc_complex(d_fftsize));
	d_magbufs.push_back(fft::malloc_double(d_fftsize));
	memset(d_residbufs[i], 0, d_fftsize*sizeof(float));
	memset(d_magbufs[i], 0, d_fftsize*sizeof(double));
      }

      buildwindow();

      initialize();
    }

    waterfall_sink_c_impl::~waterfall_sink_c_impl()
    {
      for(int i = 0; i < d_nconnections; i++) {
	fft::free(d_residbufs[i]);
	fft::free(d_magbufs[i]);
      }
      delete d_fft;
      fft::free(d_fbuf);
    }

    void
    waterfall_sink_c_impl::forecast(int noutput_items, gr_vector_int &ninput_items_required)
    {
      unsigned int ninputs = ninput_items_required.size();
      for (unsigned int i = 0; i < ninputs; i++) {
	ninput_items_required[i] = std::min(d_fftsize, 8191);
      }
    }

    void
    waterfall_sink_c_impl::initialize()
    {
      if(qApp != NULL) {
	d_qApplication = qApp;
      }
      else {
	int argc=0;
	char **argv = NULL;
	d_qApplication = new QApplication(argc, argv);
      }

      d_main_gui = new WaterfallDisplayForm(d_nconnections, d_parent);
      d_main_gui->SetFFTSize(d_fftsize);
      d_main_gui->SetFFTWindowType(d_wintype);
      d_main_gui->SetFrequencyRange(d_center_freq,
				    d_center_freq - d_bandwidth/2.0,
				    d_center_freq + d_bandwidth/2.0);

      // initialize update time to 10 times a second
      set_update_time(0.1);
      d_last_time = 0;
    }

    void
    waterfall_sink_c_impl::exec_()
    {
      d_qApplication->exec();
    }

    QWidget*
    waterfall_sink_c_impl::qwidget()
    {
      return d_main_gui;
    }

    PyObject*
    waterfall_sink_c_impl::pyqwidget()
    {
      PyObject *w = PyLong_FromVoidPtr((void*)d_main_gui);
      PyObject *retarg = Py_BuildValue("N", w);
      return retarg;
    }

    void
    waterfall_sink_c_impl::set_fft_size(const int fftsize)
    {
      d_fftsize = fftsize;
      d_main_gui->SetFFTSize(fftsize);
    }

    int
    waterfall_sink_c_impl::fft_size() const
    {
      return d_fftsize;
    }

    void
    waterfall_sink_c_impl::set_fft_average(const float fftavg)
    {
      d_fftavg = fftavg;
      d_main_gui->SetFFTAverage(fftavg);
    }

    float
    waterfall_sink_c_impl::fft_average() const
    {
      return d_fftavg;
    }

    void
    waterfall_sink_c_impl::set_frequency_range(const double centerfreq,
					       const double bandwidth)
    {
      d_center_freq = centerfreq;
      d_bandwidth = bandwidth;
      d_main_gui->SetFrequencyRange(d_center_freq,
				    -d_bandwidth/2.0,
				    d_bandwidth/2.0);
    }

    void
    waterfall_sink_c_impl::set_update_time(double t)
    {
      //convert update time to ticks
      gruel::high_res_timer_type tps = gruel::high_res_timer_tps();
      d_update_time = t * tps;
      d_main_gui->setUpdateTime(t);
    }

    void
    waterfall_sink_c_impl::set_title(const std::string &title)
    {
      d_main_gui->setTitle(0, title.c_str());
    }

    void
    waterfall_sink_c_impl::set_color(const std::string &color)
    {
      d_main_gui->setColor(0, color.c_str());
    }

    void
    waterfall_sink_c_impl::set_line_width(int width)
    {
      d_main_gui->setLineWidth(0, width);
    }

    void
    waterfall_sink_c_impl::set_line_style(Qt::PenStyle style)
    {
      d_main_gui->setLineStyle(0, style);
    }

    void
    waterfall_sink_c_impl::set_line_marker(QwtSymbol::Style marker)
    {
      d_main_gui->setLineMarker(0, marker);
    }

    void
    waterfall_sink_c_impl::set_size(int width, int height)
    {
      d_main_gui->resize(QSize(width, height));
    }

    void
    waterfall_sink_c_impl::fft(float *data_out, const gr_complex *data_in, int size)
    {
      if(d_window.size()) {
	volk_32fc_32f_multiply_32fc_a(d_fft->get_inbuf(), data_in, &d_window.front(), size);
      }
      else {
	memcpy(d_fft->get_inbuf(), data_in, sizeof(gr_complex)*size);
      }

      d_fft->execute();     // compute the fft

      volk_32fc_s32f_x2_power_spectral_density_32f_a(data_out, d_fft->get_outbuf(),
						     size, 1.0, size);

      // Perform shift operation
      unsigned int len = (unsigned int)(floor(size/2.0));
      float *tmp = (float*)malloc(sizeof(float)*len);
      memcpy(tmp, &data_out[0], sizeof(float)*len);
      memcpy(&data_out[0], &data_out[len], sizeof(float)*(size - len));
      memcpy(&data_out[size - len], tmp, sizeof(float)*len);
      free(tmp);
    }

    void
    waterfall_sink_c_impl::windowreset()
    {
      filter::firdes::win_type newwintype;
      newwintype = d_main_gui->GetFFTWindowType();
      if(d_wintype != newwintype) {
        d_wintype = newwintype;
        buildwindow();
      }
    }

    void
    waterfall_sink_c_impl::buildwindow()
    {
      d_window.clear();
      if(d_wintype != filter::firdes::WIN_NONE) {
	d_window = filter::firdes::window(d_wintype, d_fftsize, 6.76);
      }
    }

    void
    waterfall_sink_c_impl::fftresize()
    {
      int newfftsize = d_main_gui->GetFFTSize();
      d_fftavg = d_main_gui->GetFFTAverage();

      if(newfftsize != d_fftsize) {

	// Resize residbuf and replace data
	for(int i = 0; i < d_nconnections; i++) {
	  fft::free(d_residbufs[i]);
	  fft::free(d_magbufs[i]);

	  d_residbufs[i] = fft::malloc_complex(newfftsize);
	  d_magbufs[i] = fft::malloc_double(newfftsize);

	  memset(d_residbufs[i], 0, newfftsize*sizeof(gr_complex));
	  memset(d_magbufs[i], 0, newfftsize*sizeof(double));
	}

	// Set new fft size and reset buffer index 
	// (throws away any currently held data, but who cares?) 
	d_fftsize = newfftsize;
	d_index = 0;

	// Reset window to reflect new size
	buildwindow();

	// Reset FFTW plan for new size
	delete d_fft;
	d_fft = new fft::fft_complex(d_fftsize, true);

	fft::free(d_fbuf);
	d_fbuf = fft::malloc_float(d_fftsize);
	memset(d_fbuf, 0, d_fftsize*sizeof(float));
      }
    }

    int
    waterfall_sink_c_impl::work(int noutput_items,
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

	// If we have enough input for one full FFT, do it
	if(datasize >= resid) {

	  for(int n = 0; n < d_nconnections; n++) {
	    // Fill up residbuf with d_fftsize number of items
	    in = (const gr_complex*)input_items[n];
	    memcpy(d_residbufs[n]+d_index, &in[j], sizeof(gr_complex)*resid);

	    fft(d_fbuf, d_residbufs[n], d_fftsize);
	    for(int x = 0; x < d_fftsize; x++) {
	      d_magbufs[n][x] = (double)((1.0-d_fftavg)*d_magbufs[n][x] + (d_fftavg)*d_fbuf[x]);
	    }
	    //volk_32f_convert_64f_a(d_magbufs[n], d_fbuf, d_fftsize);
	  }
      
	  if(gruel::high_res_timer_now() - d_last_time > d_update_time) {
	    d_last_time = gruel::high_res_timer_now();
	    d_qApplication->postEvent(d_main_gui,
				      new WaterfallUpdateEvent(d_magbufs,
							       d_fftsize,
							       d_last_time));
	  }

	  d_index = 0;
	  j += resid;
	}
	// Otherwise, copy what we received into the residbuf for next time
	else {
	  for(int n = 0; n < d_nconnections; n++) {
	    in = (const gr_complex*)input_items[n];
	    memcpy(d_residbufs[n]+d_index, &in[j], sizeof(gr_complex)*datasize);
	  }
	  d_index += datasize;
	  j += datasize;
	}
      }

      return j;
    }

  } /* namespace qtgui */
} /* namespace gr */

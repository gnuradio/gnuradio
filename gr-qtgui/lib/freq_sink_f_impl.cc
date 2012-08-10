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

#include "freq_sink_f_impl.h"
#include <gr_io_signature.h>
#include <string.h>
#include <volk/volk.h>

namespace gr {
  namespace qtgui {

    freq_sink_f::sptr
    freq_sink_f::make(int fftsize, int wintype,
		      double fc, double bw,
		      const std::string &name,
		      int nconnections,
		      QWidget *parent)
    {
      return gnuradio::get_initial_sptr
	(new freq_sink_f_impl(fftsize, wintype,
			      fc, bw, name,
			      nconnections,
			      parent));
    }

    freq_sink_f_impl::freq_sink_f_impl(int fftsize, int wintype,
				       double fc, double bw,
				       const std::string &name,
				       int nconnections,
				       QWidget *parent)
      : gr_sync_block("freq_sink_f",
		      gr_make_io_signature(1, -1, sizeof(float)),
		      gr_make_io_signature(0, 0, 0)),
	d_fftsize(fftsize), d_fftavg(1.0), 
	d_wintype((filter::firdes::win_type)(wintype)),
	d_center_freq(fc), d_bandwidth(bw), d_name(name),
	d_nconnections(nconnections), d_parent(parent)
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
	d_residbufs.push_back(fft::malloc_float(d_fftsize));
	d_magbufs.push_back(fft::malloc_double(d_fftsize));

	memset(d_residbufs[i], 0, d_fftsize*sizeof(float));
	memset(d_magbufs[i], 0, d_fftsize*sizeof(double));
      }

      buildwindow();

      initialize();
    }

    freq_sink_f_impl::~freq_sink_f_impl()
    {
      for(int i = 0; i < d_nconnections; i++) {
	fft::free(d_residbufs[i]);
	fft::free(d_magbufs[i]);
      }
      delete d_fft;
      fft::free(d_fbuf);
    }

    void
    freq_sink_f_impl::forecast(int noutput_items, gr_vector_int &ninput_items_required)
    {
      unsigned int ninputs = ninput_items_required.size();
      for (unsigned int i = 0; i < ninputs; i++) {
	ninput_items_required[i] = std::min(d_fftsize, 8191);
      }
    }

    void
    freq_sink_f_impl::initialize()
    {
      if(qApp != NULL) {
	d_qApplication = qApp;
      }
      else {
	int argc=0;
	char **argv = NULL;
	d_qApplication = new QApplication(argc, argv);
      }

      d_main_gui = new FreqDisplayForm(d_nconnections, d_parent);
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
    freq_sink_f_impl::exec_()
    {
      d_qApplication->exec();
    }

    QWidget*
    freq_sink_f_impl::qwidget()
    {
      return d_main_gui;
    }

    PyObject*
    freq_sink_f_impl::pyqwidget()
    {
      PyObject *w = PyLong_FromVoidPtr((void*)d_main_gui);
      PyObject *retarg = Py_BuildValue("N", w);
      return retarg;
    }

    void
    freq_sink_f_impl::set_fft_size(const int fftsize)
    {
      d_fftsize = fftsize;
      d_main_gui->SetFFTSize(fftsize);
    }

    int
    freq_sink_f_impl::fft_size() const
    {
      return d_fftsize;
    }
 
    void
    freq_sink_f_impl::set_fft_average(const float fftavg)
    {
      d_fftavg = fftavg;
      d_main_gui->SetFFTAverage(fftavg);
    }

    float
    freq_sink_f_impl::fft_average() const
    {
      return d_fftavg;
    }

    void
    freq_sink_f_impl::set_frequency_range(const double centerfreq,
					  const double bandwidth)
    {
      d_center_freq = centerfreq;
      d_bandwidth = bandwidth;
      d_main_gui->SetFrequencyRange(d_center_freq,
				    -d_bandwidth/2.0,
				    d_bandwidth/2.0);
    }

    void
    freq_sink_f_impl::set_fft_power_db(double min, double max)
    {
      d_main_gui->SetFrequencyAxis(min, max);
    }

    void
    freq_sink_f_impl::set_update_time(double t)
    {
      //convert update time to ticks
      gruel::high_res_timer_type tps = gruel::high_res_timer_tps();
      d_update_time = t * tps;
      d_main_gui->setUpdateTime(t);
    }

    void
    freq_sink_f_impl::set_title(int which, const std::string &title)
    {
      d_main_gui->setTitle(which, title.c_str());
    }

    void
    freq_sink_f_impl::set_color(int which, const std::string &color)
    {
      d_main_gui->setColor(which, color.c_str());
    }

    void
    freq_sink_f_impl::set_line_width(int which, int width)
    {
      d_main_gui->setLineWidth(which, width);
    }

    void
    freq_sink_f_impl::set_line_style(int which, Qt::PenStyle style)
    {
      d_main_gui->setLineStyle(which, style);
    }

    void
    freq_sink_f_impl::set_line_marker(int which, QwtSymbol::Style marker)
    {
      d_main_gui->setLineMarker(which, marker);
    }


    void
    freq_sink_f_impl::set_size(int width, int height)
    {
      d_main_gui->resize(QSize(width, height));
    }

    void
    freq_sink_f_impl::fft(float *data_out, const float *data_in, int size)
    {
      // float to complex conversion
      gr_complex *dst = d_fft->get_inbuf();
      for (int i = 0; i < size; i++)
	dst[i] = data_in[i];

      if(d_window.size()) {
	volk_32fc_32f_multiply_32fc_a(d_fft->get_inbuf(), dst,
				      &d_window.front(), size);
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
    freq_sink_f_impl::windowreset()
    {
      gruel::scoped_lock lock(d_mutex);

      filter::firdes::win_type newwintype;
      newwintype = d_main_gui->GetFFTWindowType();
      if(d_wintype != newwintype) {
        d_wintype = newwintype;
        buildwindow();
      }
    }

    void
    freq_sink_f_impl::buildwindow()
    {
      d_window.clear();
      if(d_wintype != filter::firdes::WIN_NONE) {
	d_window = filter::firdes::window(d_wintype, d_fftsize, 6.76);
      }
    }

    void
    freq_sink_f_impl::fftresize()
    {
      gruel::scoped_lock lock(d_mutex);

      int newfftsize = d_main_gui->GetFFTSize();
      d_fftavg = d_main_gui->GetFFTAverage();

      if(newfftsize != d_fftsize) {
	// Resize residbuf and replace data
	for(int i = 0; i < d_nconnections; i++) {
	  fft::free(d_residbufs[i]);
	  fft::free(d_magbufs[i]);

	  d_residbufs[i] = fft::malloc_float(newfftsize);
	  d_magbufs[i] = fft::malloc_double(newfftsize);

	  memset(d_residbufs[i], 0, newfftsize*sizeof(float));
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
    freq_sink_f_impl::work(int noutput_items,
			   gr_vector_const_void_star &input_items,
			   gr_vector_void_star &output_items)
    {
      int j=0;
      const float *in = (const float*)input_items[0];

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
	    in = (const float*)input_items[n];
	    memcpy(d_residbufs[n]+d_index, &in[j], sizeof(float)*resid);

	    fft(d_fbuf, d_residbufs[n], d_fftsize);
	    for(int x = 0; x < d_fftsize; x++) {
	      d_magbufs[n][x] = (double)((1.0-d_fftavg)*d_magbufs[n][x] + (d_fftavg)*d_fbuf[x]);
	    }
	    //volk_32f_convert_64f_a(d_magbufs[n], d_fbuf, d_fftsize);
	  }
      
	  if(gruel::high_res_timer_now() - d_last_time > d_update_time) {
	    d_last_time = gruel::high_res_timer_now();
	    d_qApplication->postEvent(d_main_gui,
				      new FreqUpdateEvent(d_magbufs, d_fftsize));
	  }

	  d_index = 0;
	  j += resid;
	}
	// Otherwise, copy what we received into the residbuf for next time
	else {
	  for(int n = 0; n < d_nconnections; n++) {
	    in = (const float*)input_items[n];
	    memcpy(d_residbufs[n]+d_index, &in[j], sizeof(float)*datasize);
	  }
	  d_index += datasize;
	  j += datasize;
	}
      }

      return j;
    }

  } /* namespace qtgui */
} /* namespace gr */

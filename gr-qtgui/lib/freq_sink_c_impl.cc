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

#include "freq_sink_c_impl.h"
#include <gnuradio/io_signature.h>
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
		      io_signature::make(1, -1, sizeof(gr_complex)),
		      io_signature::make(0, 0, 0)),
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
	d_residbufs.push_back(fft::malloc_complex(d_fftsize));
	d_magbufs.push_back(fft::malloc_double(d_fftsize));

	memset(d_residbufs[i], 0, d_fftsize*sizeof(gr_complex));
	memset(d_magbufs[i], 0, d_fftsize*sizeof(double));
      }

      buildwindow();

      initialize();
    }

    freq_sink_c_impl::~freq_sink_c_impl()
    {
      if(!d_main_gui->isClosed())
        d_main_gui->close();

      for(int i = 0; i < d_nconnections; i++) {
	fft::free(d_residbufs[i]);
	fft::free(d_magbufs[i]);
      }
      delete d_fft;
      fft::free(d_fbuf);
    }

    bool
    freq_sink_c_impl::check_topology(int ninputs, int noutputs)
    {
      return ninputs == d_nconnections;
    }

    void
    freq_sink_c_impl::forecast(int noutput_items, gr_vector_int &ninput_items_required)
    {
      unsigned int ninputs = ninput_items_required.size();
      for (unsigned int i = 0; i < ninputs; i++) {
	ninput_items_required[i] = std::min(d_fftsize, 8191);
      }
    }

    void
    freq_sink_c_impl::initialize()
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
      set_fft_window(d_wintype);
      set_fft_size(d_fftsize);
      set_frequency_range(d_center_freq, d_bandwidth);

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

    PyObject*
    freq_sink_c_impl::pyqwidget()
    {
      PyObject *w = PyLong_FromVoidPtr((void*)d_main_gui);
      PyObject *retarg = Py_BuildValue("N", w);
      return retarg;
    }

    void
    freq_sink_c_impl::set_fft_size(const int fftsize)
    {
      d_main_gui->setFFTSize(fftsize);
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
    freq_sink_c_impl::reset()
    {
      d_index = 0;
    }

    void
    freq_sink_c_impl::fft(float *data_out, const gr_complex *data_in, int size)
    {
      if(d_window.size()) {
	volk_32fc_32f_multiply_32fc_a(d_fft->get_inbuf(), data_in,
				      &d_window.front(), size);
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
    freq_sink_c_impl::windowreset()
    {
      filter::firdes::win_type newwintype;
      newwintype = d_main_gui->getFFTWindowType();
      if(d_wintype != newwintype) {
        d_wintype = newwintype;
        buildwindow();
      }
    }

    void
    freq_sink_c_impl::buildwindow()
    {
      d_window.clear();
      if(d_wintype != filter::firdes::WIN_NONE) {
	d_window = filter::firdes::window(d_wintype, d_fftsize, 6.76);
      }
    }

    void
    freq_sink_c_impl::fftresize()
    {
      gr::thread::scoped_lock lock(d_mutex);

      int newfftsize = d_main_gui->getFFTSize();
      d_fftavg = d_main_gui->getFFTAverage();

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
    freq_sink_c_impl::work(int noutput_items,
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

	  if(gr::high_res_timer_now() - d_last_time > d_update_time) {
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
      
	    d_last_time = gr::high_res_timer_now();
	    d_qApplication->postEvent(d_main_gui,
				      new FreqUpdateEvent(d_magbufs, d_fftsize));
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

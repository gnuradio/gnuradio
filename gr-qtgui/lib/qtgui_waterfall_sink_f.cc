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

#include <qtgui_waterfall_sink_f.h>
#include <gr_io_signature.h>
#include <string.h>

#include <QTimer>
#include <volk/volk.h>

qtgui_waterfall_sink_f_sptr
qtgui_make_waterfall_sink_f(int fftsize, int wintype,
			    double fc, double bw,
			    const std::string &name,
			    int nconnections,
			    QWidget *parent)
{
  return gnuradio::get_initial_sptr
    (new qtgui_waterfall_sink_f(fftsize, wintype,
				fc, bw, name,
				nconnections,
				parent));
}

qtgui_waterfall_sink_f::qtgui_waterfall_sink_f(int fftsize, int wintype,
					       double fc, double bw,
					       const std::string &name,
					       int nconnections,
					       QWidget *parent)
  : gr_sync_block("waterfall_sink_f",
		  gr_make_io_signature(1, -1, sizeof(float)),
		  gr_make_io_signature(0, 0, 0)),
    d_fftsize(fftsize),
    d_wintype((gr_firdes::win_type)(wintype)),
    d_center_freq(fc), d_bandwidth(bw), d_name(name),
    d_nconnections(nconnections), d_parent(parent)
{
  d_main_gui = NULL;

  // Perform fftshift operation;
  // this is usually desired when plotting
  d_shift = true;

  d_fft = new gri_fft_complex(d_fftsize, true);

  d_index = 0;
  for(int i = 0; i < d_nconnections; i++) {
    d_residbufs.push_back(gri_fft_malloc_float(d_fftsize));
    d_magbufs.push_back(gri_fft_malloc_double(d_fftsize));
  }

  buildwindow();

  initialize();
}

qtgui_waterfall_sink_f::~qtgui_waterfall_sink_f()
{
  for(int i = 0; i < d_nconnections; i++) {
    gri_fft_free(d_residbufs[i]);
    gri_fft_free(d_magbufs[i]);
  }
  delete d_fft;
}

void
qtgui_waterfall_sink_f::forecast(int noutput_items, gr_vector_int &ninput_items_required)
{
  unsigned int ninputs = ninput_items_required.size();
  for (unsigned int i = 0; i < ninputs; i++) {
    ninput_items_required[i] = std::min(d_fftsize, 8191);
  }
}

void
qtgui_waterfall_sink_f::initialize()
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
  d_main_gui->SetFrequencyRange(d_center_freq,
				d_center_freq - d_bandwidth/2.0,
				d_center_freq + d_bandwidth/2.0);

  // initialize update time to 10 times a second
  set_update_time(0.1);
  d_last_time = 0;
}

void
qtgui_waterfall_sink_f::exec_()
{
  d_qApplication->exec();
}

QWidget*
qtgui_waterfall_sink_f::qwidget()
{
  return d_main_gui;
}

PyObject*
qtgui_waterfall_sink_f::pyqwidget()
{
  PyObject *w = PyLong_FromVoidPtr((void*)d_main_gui);
  PyObject *retarg = Py_BuildValue("N", w);
  return retarg;
}

void
qtgui_waterfall_sink_f::set_frequency_range(const double centerfreq,
					    const double bandwidth)
{
  d_center_freq = centerfreq;
  d_bandwidth = bandwidth;
  d_main_gui->SetFrequencyRange(d_center_freq,
				-d_bandwidth/2.0,
				d_bandwidth/2.0);
}

void
qtgui_waterfall_sink_f::set_update_time(double t)
{
  //convert update time to ticks
  gruel::high_res_timer_type tps = gruel::high_res_timer_tps();
  d_update_time = t * tps;
  d_main_gui->setUpdateTime(t);
}

void
qtgui_waterfall_sink_f::set_title(int which, const std::string &title)
{
  d_main_gui->setTitle(which, title.c_str());
}

void
qtgui_waterfall_sink_f::set_color(int which, const std::string &color)
{
  d_main_gui->setColor(which, color.c_str());
}

void
qtgui_waterfall_sink_f::set_resize(int width, int height)
{
  d_main_gui->resize(QSize(width, height));
}

void
qtgui_waterfall_sink_f::fft(float *data_out, const float *data_in, int size)
{
  // float to complex conversion
  gr_complex *dst = d_fft->get_inbuf();
  for (int i = 0; i < size; i++)
    dst[i] = data_in[i];

  if(d_window.size()) {
    volk_32fc_32f_multiply_32fc_a(d_fft->get_inbuf(), dst, &d_window.front(), size);
  }

  d_fft->execute();     // compute the fft

  volk_32fc_s32f_x2_power_spectral_density_32f_a(data_out, d_fft->get_outbuf(), size, 1.0, size);
}

void
qtgui_waterfall_sink_f::windowreset()
{
  //gr_firdes::win_type newwintype = (gr_firdes::win_type)d_main_gui->GetWindowType();
  //if(d_wintype != newwintype) {
  //  d_wintype = newwintype;
  //  buildwindow();
  //}
}

void
qtgui_waterfall_sink_f::buildwindow()
{
  d_window.clear();
  if(d_wintype != 0) {
    d_window = gr_firdes::window(d_wintype, d_fftsize, 6.76);
  }
}

void
qtgui_waterfall_sink_f::fftresize()
{
  int newfftsize = d_fftsize;

  if(newfftsize != d_fftsize) {

    // Resize residbuf and replace data
    for(int i = 0; i < d_nconnections; i++) {
      gri_fft_free(d_residbufs[i]);
      gri_fft_free(d_magbufs[i]);

      d_residbufs.push_back(gri_fft_malloc_float(newfftsize));
      d_magbufs.push_back(gri_fft_malloc_double(newfftsize));
    }

    // Set new fft size and reset buffer index 
    // (throws away any currently held data, but who cares?) 
    d_fftsize = newfftsize;
    d_index = 0;

    // Reset window to reflect new size
    buildwindow();

    // Reset FFTW plan for new size
    delete d_fft;
    d_fft = new gri_fft_complex(d_fftsize, true);
  }
}

int
qtgui_waterfall_sink_f::work(int noutput_items,
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

      float *fbuf = gri_fft_malloc_float(d_fftsize);
      for(int n = 0; n < d_nconnections; n++) {
	// Fill up residbuf with d_fftsize number of items
	in = (const float*)input_items[n];
	memcpy(d_residbufs[n]+d_index, &in[j], sizeof(float)*resid);

	fft(fbuf, d_residbufs[n], d_fftsize);
	volk_32f_convert_64f_a(d_magbufs[n], fbuf, d_fftsize);
      }
      gri_fft_free(fbuf);
      
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
	in = (const float*)input_items[n];
	memcpy(d_residbufs[n]+d_index, &in[j], sizeof(float)*datasize);
      }
      d_index += datasize;
      j += datasize;
    }
  }

  return noutput_items;
}

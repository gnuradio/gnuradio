/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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

#include <qtgui_sink_f.h>
#include <gr_io_signature.h>
#include <string.h>

#include <QTimer>

qtgui_sink_f_sptr
qtgui_make_sink_f (int fftsize, const std::vector<float> &window,
		   float fmin, float fmax, const std::string &name)
{
  return qtgui_sink_f_sptr (new qtgui_sink_f (fftsize, window, fmin, fmax, name));
}

qtgui_sink_f::qtgui_sink_f (int fftsize, const std::vector<float> &window,
			    float fmin, float fmax, const std::string &name)
  : gr_block ("sink_f",
	      gr_make_io_signature (1, 1, sizeof(float)),
	      gr_make_io_signature (0, 0, 0)),
    d_fftsize(fftsize), d_window(window), 
    d_fmin(fmin), d_fmax(fmax), d_name(name)
{
  d_main_gui = NULL;
  pthread_mutex_init(&d_pmutex, NULL);
  lock();

  d_shift = true;  // Perform fftshift operation; this is usually desired when plotting

  d_fft = new gri_fft_complex (d_fftsize, true);

  fftdata = new gr_complex[d_fftsize];

  d_index = 0;
  d_residbuf = new float[d_fftsize];
}

qtgui_sink_f::~qtgui_sink_f()
{
  delete [] fftdata;
  delete [] d_residbuf;
  delete d_main_gui;
  delete d_fft;
}

void qtgui_sink_f::lock()
{
  pthread_mutex_lock(&d_pmutex);
}

void qtgui_sink_f::unlock()
{
  pthread_mutex_unlock(&d_pmutex);
}

void
qtgui_sink_f::start_app()
{
  d_qApplication = new QApplication(0, NULL);

  uint64_t maxBufferSize = 32768;
  d_main_gui = new SpectrumGUIClass(maxBufferSize, d_fftsize, d_fmin, d_fmax);
  d_main_gui->SetDisplayTitle(d_name);
  d_main_gui->OpenSpectrumWindow(NULL);

  qtgui_obj object(d_qApplication);
  qApp->postEvent(&object, new qtgui_event(&d_pmutex));

  d_qApplication->exec();
}


void
qtgui_sink_f::fft(const float *data_in, int size, gr_complex *data_out)
{
  if (d_window.size()) {
    gr_complex *dst = d_fft->get_inbuf();
    for (int i = 0; i < size; i++)		// apply window
      dst[i] = data_in[i] * d_window[i];
  }
  else {
      gr_complex *dst = d_fft->get_inbuf();
      for (unsigned int i = 0; i < size; i++)		// float to complex conversion
	dst[i] = data_in[i];
  }
  
  d_fft->execute ();     // compute the fft

  for(int i=0; i < size; i++) {
    d_fft->get_outbuf()[i] /= size;
  }

  // copy result to our output
  if(d_shift) {  // apply a fft shift on the data
    unsigned int len = (unsigned int)(ceil(size/2.0));
    memcpy(&data_out[0], &d_fft->get_outbuf()[len], sizeof(gr_complex)*(size - len));
    memcpy(&data_out[size - len], &d_fft->get_outbuf()[0], sizeof(gr_complex)*len);
  }
  else {
    memcpy(data_out, d_fft->get_outbuf(), sizeof(gr_complex)*size);
  }
}


int
qtgui_sink_f::general_work (int noutput_items,
			    gr_vector_int &ninput_items,
			    gr_vector_const_void_star &input_items,
			    gr_vector_void_star &output_items)
{
  int i=0, j=0;
  const float *in = (const float*)input_items[0];

  pthread_mutex_lock(&d_pmutex);

  if(d_index) {
    int filler = std::min(d_fftsize - d_index, noutput_items);
    memcpy(&d_residbuf[d_index], &in[0], sizeof(float)*filler);
    d_index += filler;
    i = filler;
    j = filler;
  }

  if(d_index == d_fftsize) {
    d_index = 0;
    fft(d_residbuf, d_fftsize, fftdata);
    
    d_main_gui->UpdateWindow(true, fftdata, d_fftsize, d_residbuf, d_fftsize, NULL, 0,
			     1.0/4.0, convert_to_timespec(0.0), true);
  }
  
  for(; i < noutput_items; i+=d_fftsize) {
    if(noutput_items - i > d_fftsize) {
      j += d_fftsize;
      fft(&in[i], d_fftsize, fftdata);
      
      d_main_gui->UpdateWindow(true, fftdata, d_fftsize, &in[i], d_fftsize, NULL, 0,
			       1.0/4.0, convert_to_timespec(0.0), true);
    }
  }

  if(noutput_items > j) {
    d_index = noutput_items - j;
    memcpy(d_residbuf, &in[j], sizeof(float)*d_index);
  }

  pthread_mutex_unlock(&d_pmutex);

  consume_each(noutput_items);
  return noutput_items;
}

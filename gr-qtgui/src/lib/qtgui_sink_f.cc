/* -*- c++ -*- */
/*
 * Copyright 2008,2009 Free Software Foundation, Inc.
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
qtgui_make_sink_f (int fftsize, int wintype,
		   float fmin, float fmax,
		   const std::string &name,
		   bool plotfreq, bool plotwaterfall,
		   bool plotwaterfall3d, bool plottime,
		   bool plotconst,
		   QWidget *parent)
{
  return qtgui_sink_f_sptr (new qtgui_sink_f (fftsize, wintype,
					      fmin, fmax, name,
					      plotfreq, plotwaterfall,
					      plotwaterfall3d, plottime,
					      plotconst,
					      parent));
}

qtgui_sink_f::qtgui_sink_f (int fftsize, int wintype,
			    float fmin, float fmax,
			    const std::string &name,
			    bool plotfreq, bool plotwaterfall,
			    bool plotwaterfall3d, bool plottime,
			    bool plotconst,
			    QWidget *parent)
  : gr_block ("sink_f",
	      gr_make_io_signature (1, 1, sizeof(float)),
	      gr_make_io_signature (0, 0, 0)),
    d_fftsize(fftsize),
    d_wintype((gr_firdes::win_type)(wintype)),
    d_fmin(fmin), d_fmax(fmax), d_name(name),
    d_plotfreq(plotfreq), d_plotwaterfall(plotwaterfall),
    d_plotwaterfall3d(plotwaterfall3d), d_plottime(plottime),
    d_plotconst(plotconst),
    d_parent(parent)
{
  d_main_gui = NULL;
  pthread_mutex_init(&d_pmutex, NULL);
  lock();

  // Perform fftshift operation;
  // this is usually desired when plotting
  d_shift = true;

  d_fft = new gri_fft_complex (d_fftsize, true);

  d_fftdata = new gr_complex[d_fftsize];

  d_index = 0;
  d_residbuf = new float[d_fftsize];

  buildwindow();

  initialize();
}

qtgui_sink_f::~qtgui_sink_f()
{
  delete d_object;
  delete [] d_fftdata;
  delete [] d_residbuf;
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
qtgui_sink_f::initialize()
{
  if(qApp != NULL) {
    d_qApplication = qApp;
  }
  else {
    int argc;
    char **argv = NULL;
    d_qApplication = new QApplication(argc, argv);
  }


  uint64_t maxBufferSize = 32768;
  d_main_gui = new SpectrumGUIClass(maxBufferSize, d_fftsize,
				    d_fmin, d_fmax);
  d_main_gui->SetDisplayTitle(d_name);
  d_main_gui->SetFFTSize(d_fftsize);
  d_main_gui->SetWindowType((int)d_wintype);

  d_main_gui->OpenSpectrumWindow(d_parent,
				 d_plotfreq, d_plotwaterfall,
				 d_plotwaterfall3d, d_plottime,
				 d_plotconst);

  d_object = new qtgui_obj(d_qApplication);
  qApp->postEvent(d_object, new qtgui_event(&d_pmutex));
}

void
qtgui_sink_f::exec_()
{
  d_qApplication->exec();
}

QWidget*
qtgui_sink_f::qwidget()
{
  return d_main_gui->qwidget();
}

PyObject*
qtgui_sink_f::pyqwidget()
{
  PyObject *w = PyLong_FromVoidPtr((void*)d_main_gui->qwidget());
  PyObject *retarg = Py_BuildValue("N", w);
  return retarg;
}

void
qtgui_sink_f::set_frequency_range(const double centerfreq, 
				  const double startfreq,
				  const double stopfreq)
{
  d_main_gui->SetFrequencyRange(centerfreq, startfreq, stopfreq);
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
      for (int i = 0; i < size; i++)	        // float to complex conversion
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

void 
qtgui_sink_f::windowreset()
{
  gr_firdes::win_type newwintype = (gr_firdes::win_type)d_main_gui->GetWindowType();  
  if(d_wintype != newwintype) {
    d_wintype = newwintype;
    buildwindow();
  }
}

void
qtgui_sink_f::buildwindow()
{
  d_window.clear();
  if(d_wintype != 0) {
    d_window = gr_firdes::window(d_wintype, d_fftsize, 6.76);
  }
}

void
qtgui_sink_f::fftresize()
{
  int newfftsize = d_main_gui->GetFFTSize();

  if(newfftsize != d_fftsize) {

    // Resize the fftdata buffer; no need to preserve old data
    delete [] d_fftdata;
    d_fftdata = new gr_complex[newfftsize];

    // Resize residbuf and replace data
    delete [] d_residbuf;
    d_residbuf = new float[newfftsize];

    // Set new fft size and reset buffer index 
    // (throws away any currently held data, but who cares?) 
    d_fftsize = newfftsize;
    d_index = 0;
    
    // Reset window to reflect new size
    buildwindow();

    // Reset FFTW plan for new size
    delete d_fft;
    d_fft = new gri_fft_complex (d_fftsize, true);
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
    fft(d_residbuf, d_fftsize, d_fftdata);
    
    d_main_gui->UpdateWindow(true, d_fftdata, d_fftsize,
			     d_residbuf, d_fftsize, NULL, 0,
			     1.0/4.0, convert_to_timespec(0.0), true);
  }
  
  for(; i < noutput_items; i+=d_fftsize) {
    if(noutput_items - i > d_fftsize) {
      j += d_fftsize;
      fft(&in[i], d_fftsize, d_fftdata);
      
      d_main_gui->UpdateWindow(true, d_fftdata, d_fftsize, &in[i],
			       d_fftsize, NULL, 0, 1.0/4.0,
			       convert_to_timespec(0.0), true);
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

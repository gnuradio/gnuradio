/* -*- c++ -*- */
/*
 * Copyright 2008,2009,2010,2011 Free Software Foundation, Inc.
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

#include <qtgui_sink_c.h>
#include <gr_io_signature.h>
#include <string.h>

#include <QTimer>

qtgui_sink_c_sptr
qtgui_make_sink_c (int fftsize, int wintype,
		   double fc, double bw,
		   const std::string &name,
		   bool plotfreq, bool plotwaterfall,
		   bool plottime, bool plotconst,
		   QWidget *parent)
{
  return gnuradio::get_initial_sptr(new qtgui_sink_c (fftsize, wintype,
						      fc, bw, name,
						      plotfreq, plotwaterfall,
						      plottime, plotconst,
						      parent));
}

qtgui_sink_c::qtgui_sink_c (int fftsize, int wintype,
			    double fc, double bw,
			    const std::string &name,
			    bool plotfreq, bool plotwaterfall,
			    bool plottime, bool plotconst,
			    QWidget *parent)
  : gr_block ("sink_c",
	      gr_make_io_signature (1, -1, sizeof(gr_complex)),
	      gr_make_io_signature (0, 0, 0)),
    d_fftsize(fftsize),
    d_wintype((gr_firdes::win_type)(wintype)), 
    d_center_freq(fc), d_bandwidth(bw), d_name(name),
    d_plotfreq(plotfreq), d_plotwaterfall(plotwaterfall),
    d_plottime(plottime), d_plotconst(plotconst),
    d_parent(parent)
{
  d_main_gui = NULL;

  // Perform fftshift operation;
  // this is usually desired when plotting
  d_shift = true;  

  d_fft = new gri_fft_complex (d_fftsize, true);

  d_index = 0;
  d_residbuf = new gr_complex[d_fftsize];

  buildwindow();

  initialize();
}

qtgui_sink_c::~qtgui_sink_c()
{
  delete d_main_gui;
  delete [] d_residbuf;
  delete d_fft;
}

void
qtgui_sink_c::forecast(int noutput_items, gr_vector_int &ninput_items_required)
{
  unsigned int ninputs = ninput_items_required.size();
  for (unsigned int i = 0; i < ninputs; i++) {
    ninput_items_required[i] = std::min(d_fftsize, 8191);
  }
}

void
qtgui_sink_c::initialize()
{
  if(qApp != NULL) {
    d_qApplication = qApp;
  }
  else {
    int argc=0;
    char **argv = NULL;
    d_qApplication = new QApplication(argc, argv);
  }

  if(d_center_freq < 0) {
    throw std::runtime_error("qtgui_sink_c: Received bad center frequency.\n");
  }

  uint64_t maxBufferSize = 32768;
  d_main_gui = new SpectrumGUIClass(maxBufferSize, d_fftsize, 
				    d_center_freq, 
				    -d_bandwidth/2.0, 
				    d_bandwidth/2.0);

  d_main_gui->SetDisplayTitle(d_name);
  d_main_gui->SetFFTSize(d_fftsize);
  d_main_gui->SetWindowType((int)d_wintype);

  d_main_gui->OpenSpectrumWindow(d_parent, 
				 d_plotfreq, d_plotwaterfall,
				 d_plottime, d_plotconst);

  // initialize update time to 10 times a second
  set_update_time(0.5);

  d_last_update = gruel::high_res_timer_now();
  d_update_active = false;
}


void
qtgui_sink_c::exec_()
{
  d_qApplication->exec();
}

QWidget*
qtgui_sink_c::qwidget()
{
  return d_main_gui->qwidget();
}

PyObject*
qtgui_sink_c::pyqwidget()
{
  PyObject *w = PyLong_FromVoidPtr((void*)d_main_gui->qwidget());
  PyObject *retarg = Py_BuildValue("N", w);
  return retarg;
}

void
qtgui_sink_c::set_frequency_range(const double centerfreq, 
				  const double bandwidth)
{
  d_center_freq = centerfreq;
  d_bandwidth = bandwidth;
  d_main_gui->SetFrequencyRange(d_center_freq, 
				-d_bandwidth/2.0,
				d_bandwidth/2.0);
}

void
qtgui_sink_c::set_time_domain_axis(double min, double max)
{
  d_main_gui->SetTimeDomainAxis(min, max);
}

void
qtgui_sink_c::set_constellation_axis(double xmin, double xmax,
				     double ymin, double ymax)
{
  d_main_gui->SetConstellationAxis(xmin, xmax, ymin, ymax);
}

void 
qtgui_sink_c::set_constellation_pen_size(int size)
{
  d_main_gui->SetConstellationPenSize(size);
}


void
qtgui_sink_c::set_frequency_axis(double min, double max)
{
  d_main_gui->SetFrequencyAxis(min, max);
}

void
qtgui_sink_c::set_update_time(double t)
{
  d_update_time = t * gruel::high_res_timer_tps();
  d_main_gui->SetUpdateTime(t);
}

void
qtgui_sink_c::fft(const gr_complex *data_in, int size)
{
  if (d_window.size()) {
    gr_complex *dst = d_fft->get_inbuf();
    int i;
    for (i = 0; i < size; i++)		// apply window
      dst[i] = data_in[i] * d_window[i];
  }
  else {
    memcpy (d_fft->get_inbuf(), data_in, sizeof(gr_complex)*size);
  }

  d_fft->execute ();     // compute the fft
}

void 
qtgui_sink_c::windowreset()
{
  gr_firdes::win_type newwintype = (gr_firdes::win_type)d_main_gui->GetWindowType();  
  if(d_wintype != newwintype) {
    d_wintype = newwintype;
    buildwindow();
  }
}

void
qtgui_sink_c::buildwindow()
{
  d_window.clear();
  if(d_wintype != 0) {
    d_window = gr_firdes::window(d_wintype, d_fftsize, 6.76);
  }
}

void
qtgui_sink_c::fftresize()
{
  int newfftsize = d_main_gui->GetFFTSize();

  if(newfftsize != d_fftsize) {

    // Resize residbuf and replace data
    delete [] d_residbuf;
    d_residbuf = new gr_complex[newfftsize];

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
qtgui_sink_c::general_work (int noutput_items,
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

    if (!d_update_active && (gruel::high_res_timer_now() - d_last_update) < d_update_time) {
      consume_each(noutput_items);
      return noutput_items;
    } else {
      d_last_update = gruel::high_res_timer_now();
      d_update_active = true;
    }

    // If we have enough input for one full FFT, do it
    if(datasize >= resid) {
      const gruel::high_res_timer_type currentTime = gruel::high_res_timer_now();
      
      // Fill up residbuf with d_fftsize number of items
      memcpy(d_residbuf+d_index, &in[j], sizeof(gr_complex)*resid);
      d_index = 0;

      j += resid;
      fft(d_residbuf, d_fftsize);
      
      d_main_gui->UpdateWindow(true, d_fft->get_outbuf(), d_fftsize,
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

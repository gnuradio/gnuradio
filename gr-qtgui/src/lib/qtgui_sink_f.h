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

#ifndef INCLUDED_QTGUI_SINK_F_H
#define INCLUDED_QTGUI_SINK_F_H

#include <gr_block.h>
#include <gri_fft.h>
#include <qapplication.h>
#include <qtgui.h>
#include "SpectrumGUIClass.h"


class qtgui_sink_f;
typedef boost::shared_ptr<qtgui_sink_f> qtgui_sink_f_sptr;

qtgui_sink_f_sptr qtgui_make_sink_f (int fftsize, const std::vector<float> &window,
				     float fmin=-0.5, float fmax=0.5, const std::string &name="Display");

class qtgui_sink_f : public gr_block
{
private:
  friend qtgui_sink_f_sptr qtgui_make_sink_f (int fftsize, const std::vector<float> &window,
					      float fmin, float fmax, const std::string &name);
  qtgui_sink_f (int fftsize, const std::vector<float> &window,
		float fmin, float fmax, const std::string &name);
  
  int d_fftsize;
  std::vector<float> d_window;
  float d_fmin;
  float d_fmax;
  std::string d_name;
  
  pthread_mutex_t d_pmutex;

  bool d_shift;
  gri_fft_complex *d_fft;
  gr_complex *fftdata;

  int d_index;
  float *d_residbuf;

  SpectrumGUIClass *d_main_gui; 

  void fft(const float *data_in, int size, gr_complex *data_out);
  
public:
  ~qtgui_sink_f();
  void start_app();
  void lock();
  void unlock();

  QApplication *d_qApplication
;

  int general_work (int noutput_items,
		    gr_vector_int &ninput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items);
};

#endif /* INCLUDED_QTGUI_SINK_F_H */

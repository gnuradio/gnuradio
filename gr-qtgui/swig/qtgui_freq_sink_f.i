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

%include "gnuradio.i"

%{
#include <qtgui_freq_sink_f.h>
%}

GR_SWIG_BLOCK_MAGIC(qtgui,freq_sink_f)

qtgui_freq_sink_f_sptr
qtgui_make_freq_sink_f(int size, int wintype,
		       double fc, double bw,
		       const std::string &name,
		       int nconnections=1,
		       QWidget *parent=NULL);

class qtgui_freq_sink_f : public gr_sync_block
{
public:
  void exec_();
  PyObject* pyqwidget();

  void set_fft_size(const int fftsize);
  int fft_size() const;
  void set_fft_average(const float fftavg);
  float fft_average() const;

  void set_frequency_range(const double centerfreq, const double bandwidth);
  void set_fft_power_db(double min, double max);

  void set_update_time(double t);
  void set_title(int which, const std::string &title);
  void set_color(int which, const std::string &color);

  void set_resize(int width, int height);
};

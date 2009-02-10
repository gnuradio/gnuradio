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

%include "gnuradio.i"

%{
#include "qtgui_sink_c.h"
#include "qtgui_sink_f.h"
%}

GR_SWIG_BLOCK_MAGIC(qtgui,sink_c)

  qtgui_sink_c_sptr qtgui_make_sink_c (int fftsize, int wintype,
				       float fmin=-0.5, float fmax=0.5, const std::string &name="Display");

class qtgui_sink_c : public gr_block
{
private:
  friend qtgui_sink_c_sptr qtgui_make_sink_c (int fftsize, int wintype,
					      float fmin, float fmax, const std::string &name);
  qtgui_sink_c (int fftsize, int wintype,
		float fmin, float fmax, const std::string &name);

public:
  void start_app();

};



/****************************************************************************************/


GR_SWIG_BLOCK_MAGIC(qtgui,sink_f)
  
qtgui_sink_f_sptr qtgui_make_sink_f (int fftsize, const std::vector<float> &window,
				     float fmin, float fmax, const std::string &name="Display");

class qtgui_sink_f : public gr_block
{
private:
  friend qtgui_sink_f_sptr qtgui_make_sink_f (int fftsize, const std::vector<float> &window,
					      float fmin, float fmax, const std::string &name);
  qtgui_sink_fy (int fftsize, const std::vector<float> &window,
		 float fmin, float fmax, const std::string &name);
  
public:
  void start_app();
};


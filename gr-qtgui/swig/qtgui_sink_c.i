/* -*- c++ -*- */
/*
 * Copyright 2008,2009,2011 Free Software Foundation, Inc.
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
#include <qtgui_sink_c.h>
%}

GR_SWIG_BLOCK_MAGIC(qtgui,sink_c)

qtgui_sink_c_sptr qtgui_make_sink_c (int fftsize, int wintype,
				     double fc=0, double bw=1.0,
				     const std::string &name="Display",
				     bool plotfreq=true, bool plotwaterfall=true,
				     bool plottime=true, bool plotconst=true,
				     QWidget *parent=NULL);

class qtgui_sink_c : public gr_block
{
private:
  friend qtgui_sink_c_sptr qtgui_make_sink_c (int fftsize, int wintype,
					      double fc, double bw,
					      const std::string &name,
					      bool plotfreq, bool plotwaterfall,
					      bool plottime, bool plotconst,
					      QWidget *parent);
  qtgui_sink_c (int fftsize, int wintype,
		double fc, double bw,
		const std::string &name,
		bool plotfreq, bool plotwaterfall,
		bool plottime, bool plotconst,
		QWidget *parent);
  
public:
  void exec_();
  PyObject* pyqwidget();

  void set_frequency_range(const double centerfreq,
			   const double bandwidth);
  void set_time_domain_axis(double min, double max);
  void set_constellation_axis(double xmin, double xmax,
			      double ymin, double ymax);
  void set_frequency_axis(double min, double max);
  void set_constellation_pen_size(int size);
  void set_update_time(double t);
};

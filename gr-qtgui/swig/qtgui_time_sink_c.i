/* -*- c++ -*- */
/*
 * Copyright 2011 Free Software Foundation, Inc.
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
#include <qtgui_time_sink_c.h>
%}

GR_SWIG_BLOCK_MAGIC(qtgui,time_sink_c)

qtgui_time_sink_c_sptr qtgui_make_time_sink_c(int size, double bw,
					      const std::string &name,
					      int nconnections=1,
					      QWidget *parent=NULL);

class qtgui_time_sink_c : public gr_sync_block
{
private:
  friend qtgui_time_sink_c_sptr qtgui_make_time_sink_c(int size, double bw,
						       const std::string &name,
						       int nconnections,
						       QWidget *parent);
  qtgui_time_sink_c(int size, double bw,
		    const std::string &name,
		    int nconnections,
		    QWidget *parent=NULL);
  
public:
  void exec_();
  PyObject* pyqwidget();

  void set_time_domain_axis(double min, double max);
  void set_update_time(double t);
  void set_title(int which, const std::string &title);
  void set_color(int which, const std::string &color);
};

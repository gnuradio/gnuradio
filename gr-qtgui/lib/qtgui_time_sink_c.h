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

#ifndef INCLUDED_QTGUI_TIME_SINK_C_H
#define INCLUDED_QTGUI_TIME_SINK_C_H

#include <Python.h>
#include <gr_block.h>
#include <gr_firdes.h>
#include <gri_fft.h>
#include <qapplication.h>
#include <timedisplayform.h>

class qtgui_time_sink_c;
typedef boost::shared_ptr<qtgui_time_sink_c> qtgui_time_sink_c_sptr;

qtgui_time_sink_c_sptr qtgui_make_time_sink_c(int size, double bw,
					      const std::string &name,
					      QWidget *parent);

class qtgui_time_sink_c : public gr_block
{
private:
  friend qtgui_time_sink_c_sptr qtgui_make_time_sink_c(int size, double bw,
						       const std::string &name,
						       QWidget *parent);
  qtgui_time_sink_c(int size, double bw,
		    const std::string &name,
		    QWidget *parent);
  
  void forecast(int noutput_items, gr_vector_int &ninput_items_required);

  void initialize();

  int d_size;
  double d_bandwidth;
  std::string d_name;

  int d_index;
  gr_complex *d_residbuf;

  double d_update_time;

  QWidget *d_parent;
  TimeDisplayForm *d_main_gui;

public:
  ~qtgui_time_sink_c();
  void exec_();
  QWidget*  qwidget();
  PyObject* pyqwidget();

  void set_time_domain_axis(double min, double max);
  void set_update_time(double t);

  QApplication *d_qApplication;

  int general_work (int noutput_items,
		    gr_vector_int &ninput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items);
};

#endif /* INCLUDED_QTGUI_TIME_SINK_C_H */

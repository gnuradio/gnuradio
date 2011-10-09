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

#ifndef INCLUDED_QTGUI_TIME_SINK_F_H
#define INCLUDED_QTGUI_TIME_SINK_F_H

#include <Python.h>
#include <gr_qtgui_api.h>
#include <gr_sync_block.h>
#include <gr_firdes.h>
#include <gri_fft.h>
#include <qapplication.h>
#include <timedisplayform.h>

class qtgui_time_sink_f;
typedef boost::shared_ptr<qtgui_time_sink_f> qtgui_time_sink_f_sptr;

GR_QTGUI_API qtgui_time_sink_f_sptr qtgui_make_time_sink_f(int size, double bw,
					      const std::string &name,
					      int nconnectons=1,
					      QWidget *parent=NULL);

/*!
 * \brief A graphical sink to display multiple signals in time.
 * \ingroup qtgui_blk
 *
 * This is a QT-based graphical sink the takes set of a float streams
 * and plots them in the time domain. Each signal is plotted with a
 * different color, and the \a set_title and \a set_color functions
 * can be used to change the lable and color for a given input number.
 */
class GR_QTGUI_API qtgui_time_sink_f : public gr_sync_block
{
private:
  friend GR_QTGUI_API qtgui_time_sink_f_sptr qtgui_make_time_sink_f(int size, double bw,
						       const std::string &name,
						       int nconnections,
						       QWidget *parent);
  qtgui_time_sink_f(int size, double bw,
		    const std::string &name,
		    int nconnections,
		    QWidget *parent=NULL);
  
  void initialize();

  int d_size;
  double d_bandwidth;
  std::string d_name;
  int d_nconnections;

  int d_index;
  std::vector<double*> d_residbufs;

  double d_update_time;

  QWidget *d_parent;
  TimeDisplayForm *d_main_gui;

  gruel::high_res_timer_type d_current_time;
  gruel::high_res_timer_type d_last_time;

public:
  ~qtgui_time_sink_f();
  void exec_();
  QWidget*  qwidget();
  PyObject* pyqwidget();

  void set_time_domain_axis(double min, double max);
  void set_update_time(double t);
  void set_title(int which, const std::string &title);
  void set_color(int which, const std::string &color);

  QApplication *d_qApplication;

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif /* INCLUDED_QTGUI_TIME_SINK_F_H */

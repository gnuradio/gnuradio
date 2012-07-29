/* -*- c++ -*- */
/*
 * Copyright 2011,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_QTGUI_TIME_SINK_C_IMPL_H
#define INCLUDED_QTGUI_TIME_SINK_C_IMPL_H

#include <qtgui/time_sink_c.h>
#include <timedisplayform.h>
#include <gruel/thread.h>
#include <gruel/high_res_timer.h>

namespace gr {
  namespace qtgui {
    
    class QTGUI_API time_sink_c_impl : public time_sink_c
    {
    private:
      void initialize();

      gruel::mutex d_mutex;

      int d_size;
      double d_bandwidth;
      std::string d_name;
      int d_nconnections;

      int d_index;
      std::vector<double*> d_residbufs;

      QWidget *d_parent;
      TimeDisplayForm *d_main_gui;

      gruel::high_res_timer_type d_update_time;
      gruel::high_res_timer_type d_last_time;

      void npoints_resize();

    public:
      time_sink_c_impl(int size, double bw,
		       const std::string &name,
		       int nconnections,
		       QWidget *parent=NULL);
      ~time_sink_c_impl();

      void exec_();
      QWidget*  qwidget();
      PyObject* pyqwidget();

      void set_time_domain_axis(double min, double max);
      void set_update_time(double t);
      void set_title(int which, const std::string &title);
      void set_color(int which, const std::string &color);
      void set_line_width(int which, int width);
      void set_line_style(int which, Qt::PenStyle style);
      void set_line_marker(int which, QwtSymbol::Style marker);
      void set_nsamps(const int size);

      void set_size(int width, int height);

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace qtgui */
} /* namespace gr */

#endif /* INCLUDED_QTGUI_TIME_SINK_C_IMPL_H */

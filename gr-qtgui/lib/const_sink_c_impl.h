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

#ifndef INCLUDED_QTGUI_CONST_SINK_C_IMPL_H
#define INCLUDED_QTGUI_CONST_SINK_C_IMPL_H

#include <gnuradio/qtgui/const_sink_c.h>
#include <gnuradio/high_res_timer.h>
#include <gnuradio/thread/thread.h>
#include <gnuradio/qtgui/constellationdisplayform.h>

namespace gr {
  namespace qtgui {
    
    class QTGUI_API const_sink_c_impl : public const_sink_c
    {
    private:
      void initialize();

      gr::thread::mutex d_mutex;

      int d_size;
      std::string d_name;
      int d_nconnections;

      int d_index;
      std::vector<double*> d_residbufs_real;
      std::vector<double*> d_residbufs_imag;

      QWidget *d_parent;
      ConstellationDisplayForm *d_main_gui;

      gr::high_res_timer_type d_update_time;
      gr::high_res_timer_type d_last_time;

      void npoints_resize();

    public:
      const_sink_c_impl(int size,
			const std::string &name,
			int nconnections,
			QWidget *parent=NULL);
      ~const_sink_c_impl();

      bool check_topology(int ninputs, int noutputs);

      void exec_();
      QWidget*  qwidget();
      PyObject* pyqwidget();

      void set_y_axis(double min, double max);
      void set_x_axis(double min, double max);

      void set_update_time(double t);
      void set_title(const std::string &title);
      void set_line_label(int which, const std::string &label);
      void set_line_color(int which, const std::string &color);
      void set_line_width(int which, int width);
      void set_line_style(int which, int style);
      void set_line_marker(int which, int marker);
      void set_nsamps(const int size);
      void set_line_alpha(int which, double alpha);

      std::string title();
      std::string line_label(int which);
      std::string line_color(int which);
      int line_width(int which);
      int line_style(int which);
      int line_marker(int which);
      double line_alpha(int which);

      void set_size(int width, int height);

      int nsamps() const;
      void enable_menu(bool en);
      void enable_autoscale(bool en);
      void reset();

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace qtgui */
} /* namespace gr */

#endif /* INCLUDED_QTGUI_CONST_SINK_C_IMPL_H */

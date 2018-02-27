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

#ifndef INCLUDED_QTGUI_VECTOR_SINK_F_IMPL_H
#define INCLUDED_QTGUI_VECTOR_SINK_F_IMPL_H

#include <gnuradio/qtgui/vector_sink_f.h>
#include <gnuradio/high_res_timer.h>
#include <gnuradio/qtgui/vectordisplayform.h>

namespace gr {
  namespace qtgui {

    class QTGUI_API vector_sink_f_impl : public vector_sink_f
    {
    private:
      void initialize(
          const std::string &name,
          const std::string &x_axis_label,
          const std::string &y_axis_label,
          double x_start,
          double x_step
      );

      const int d_vlen; //!< Vector length at input
      float d_vecavg;

      std::string d_name; //!< Initial title of the plot
      int d_nconnections; //!< Number of connected streaming ports on input

      const pmt::pmt_t d_port;
      const pmt::pmt_t d_msg; //< Key of outgoing messages

      std::vector<double*> d_magbufs;


      int d_argc;
      char *d_argv;
      QWidget *d_parent;
      VectorDisplayForm *d_main_gui;

      gr::high_res_timer_type d_update_time;
      gr::high_res_timer_type d_last_time;

      // TODO remove this?
      void check_clicked();

      // Handles message input port for setting new center frequency.
      // The message is a PMT pair (intern('freq'), double(frequency)).
      void handle_set_freq(pmt::pmt_t msg);

    public:
      vector_sink_f_impl(
          int vlen,
          double x_start,
          double x_step,
          const std::string &x_axis_label,
          const std::string &y_axis_label,
          const std::string &name,
          int nconnections,
          QWidget *parent=NULL
      );
      ~vector_sink_f_impl();

      bool check_topology(int ninputs, int noutputs);

      void exec_();
      QWidget*  qwidget();

#ifdef ENABLE_PYTHON
      PyObject* pyqwidget();
#else
      void* pyqwidget();
#endif

      int vlen() const;
      void set_vec_average(const float avg);
      float vec_average() const;

      void set_frequency_range(const double centerfreq, const double bandwidth);
      void set_x_axis(const double start, const double step);
      void set_y_axis(double min, double max);
      void set_ref_level(double ref_level);

      void set_x_axis_label(const std::string &label);
      void set_y_axis_label(const std::string &label);

      void set_x_axis_units(const std::string &units);
      void set_y_axis_units(const std::string &units);

      void set_update_time(double t);
      void set_title(const std::string &title);
      void set_line_label(int which, const std::string &label);
      void set_line_color(int which, const std::string &color);
      void set_line_width(int which, int width);
      void set_line_style(int which, int style);
      void set_line_marker(int which, int marker);
      void set_line_alpha(int which, double alpha);

      std::string title();
      std::string line_label(int which);
      std::string line_color(int which);
      int line_width(int which);
      int line_style(int which);
      int line_marker(int which);
      double line_alpha(int which);

      void set_size(int width, int height);

      void enable_menu(bool en);
      void enable_grid(bool en);
      void enable_autoscale(bool en);
      void clear_max_hold();
      void clear_min_hold();
      void reset();

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace qtgui */
} /* namespace gr */

#endif /* INCLUDED_QTGUI_VECTOR_SINK_F_IMPL_H */

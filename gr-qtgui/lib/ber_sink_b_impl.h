/* -*- c++ -*- */
/*
 * Copyright 2013-2014 Free Software Foundation, Inc.
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

#ifndef INCLUDED_QTGUI_BER_SINK_B_IMPL_H
#define INCLUDED_QTGUI_BER_SINK_B_IMPL_H

#include <gnuradio/qtgui/ber_sink_b.h>
#include <gnuradio/high_res_timer.h>
#include <gnuradio/qtgui/constellationdisplayform.h>

namespace gr {
  namespace qtgui {

    class QTGUI_API ber_sink_b_impl : public ber_sink_b
    {
    private:
      void initialize();

      std::vector<double*> d_esno_buffers;
      std::vector<double*> d_ber_buffers;

      ConstellationDisplayForm *d_main_gui;
      gr::high_res_timer_type d_update_time;
      std::vector<int> d_total_errors;
      int d_ber_min_errors;
      float d_ber_limit;
      QWidget *d_parent;
      int d_nconnections;
      int d_curves;
      gr::high_res_timer_type d_last_time;
      std::vector<int> d_total;

      //int compBER(unsigned char *inBuffer1, unsigned char *inBuffer2,int buffSize);

    public:
      ber_sink_b_impl(std::vector<float> esnos, int curves = 1,
                      int ber_min_errors = 100, float ber_limit = -7.0,
                      std::vector<std::string> curvenames = std::vector<std::string>(),
                      QWidget *parent=NULL);
      ~ber_sink_b_impl();

      bool check_topology(int ninputs, int noutputs);

      void exec_();
      QWidget*  qwidget();

#ifdef ENABLE_PYTHON
      PyObject* pyqwidget();
#else
      void* pyqwidget();
#endif

      void set_y_axis(double min, double max);
      void set_x_axis(double min, double max);

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

      int nsamps() const;
      void enable_menu(bool en);
      void enable_autoscale(bool en);

      int general_work(int noutput_items,
                       gr_vector_int& ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items);
    };

  } /* namespace qtgui */
} /* namespace gr */

#endif /*INCLUDED_QTGUI_BER_SINK_B_IMPL_H*/

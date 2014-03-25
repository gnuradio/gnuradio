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

#ifndef INCLUDED_QTGUI_BER_SINK_B_H
#define INCLUDED_QTGUI_BER_SINK_B_H

#ifdef ENABLE_PYTHON
#include <Python.h>
#endif

#include <gnuradio/qtgui/api.h>
#include <gnuradio/block.h>
#include <qapplication.h>



namespace gr {
  namespace qtgui {

    class QTGUI_API ber_sink_b : virtual public block
    {
    public:
      // gr::fec::ber_sink_b::sptr
      typedef boost::shared_ptr<ber_sink_b> sptr;

      static sptr make(std::vector<float> esnos, int curves = 1,
                       int berminerrors = 100, float berLimit = -7.0,
                       std::vector<std::string> curvenames = std::vector<std::string>(),
                       QWidget *parent=NULL);

      virtual void exec_() = 0;

#ifdef ENABLE_PYTHON
      virtual PyObject* pyqwidget() = 0;
#else
      virtual void* pyqwidget() = 0;
#endif

      virtual void set_y_axis(double min, double max) = 0;
      virtual void set_x_axis(double min, double max) = 0;

      virtual void set_update_time(double t) = 0;
      virtual void set_title(const std::string &title) = 0;
      virtual void set_line_label(int which, const std::string &label) = 0;
      virtual void set_line_color(int which, const std::string &color) = 0;
      virtual void set_line_width(int which, int width) = 0;
      virtual void set_line_style(int which, int style) = 0;
      virtual void set_line_marker(int which, int marker) = 0;

      virtual void set_line_alpha(int which, double alpha) = 0;

      virtual std::string title() = 0;
      virtual std::string line_label(int which) = 0;
      virtual std::string line_color(int which) = 0;
      virtual int line_width(int which) = 0;
      virtual int line_style(int which) = 0;
      virtual int line_marker(int which) = 0;
      virtual double line_alpha(int which) = 0;

      virtual void set_size(int width, int height) = 0;

      virtual void enable_menu(bool en=true) = 0;
      virtual void enable_autoscale(bool en) = 0;
      virtual int nsamps() const = 0;

      QApplication *d_qApplication;
    };

  } /* namespace qtgui */
} /* namespace gr */

#endif /* INCLUDED_QTGUI_BER_SINK_B_H */

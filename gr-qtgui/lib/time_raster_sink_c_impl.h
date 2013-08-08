/* -*- c++ -*- */
/*
 * Copyright 2012,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_QTGUI_TIME_RASTER_SINK_C_IMPL_H
#define INCLUDED_QTGUI_TIME_RASTER_SINK_C_IMPL_H

#include <gnuradio/qtgui/time_raster_sink_c.h>
#include <gnuradio/filter/firdes.h>
#include <gnuradio/fft/fft.h>
#include <gnuradio/high_res_timer.h>
#include <gnuradio/thread/thread.h>
#include <gnuradio/qtgui/time_rasterdisplayform.h>

namespace gr {
  namespace qtgui {

    class QTGUI_API time_raster_sink_c_impl : public time_raster_sink_c
    {
    private:
      void forecast(int noutput_items, gr_vector_int &ninput_items_required);

      void initialize();

      std::string d_name;
      int d_nconnections;

      int d_index;
      std::vector<gr_complex*> d_residbufs;
      float *d_fbuf;

      QWidget *d_parent;
      TimeRasterDisplayForm *d_main_gui;

      unsigned int d_rows, d_cols;

      gr::high_res_timer_type d_update_time;
      gr::high_res_timer_type d_last_time;

    public:
      time_raster_sink_c_impl(unsignedint rows,
			      unsigned int cols,
			      const std::string &name,
			      QWidget *parent=NULL);
      ~time_raster_sink_c_impl();

      void exec_();
      QWidget*  qwidget();
      PyObject* pyqwidget();

      void set_update_time(double t);
      void set_title(const std::string &title);
      void set_line_label(const std::string &label);
      void set_line_color(const std::string &color);
      void set_line_width(int width);
      void set_line_style(Qt::PenStyle style);
      void set_line_marker(QwtSymbol::Style marker);

      void set_size(int width, int height);

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace qtgui */
} /* namespace gr */

#endif /* INCLUDED_QTGUI_TIME_RASTER_SINK_C_IMPL_H */

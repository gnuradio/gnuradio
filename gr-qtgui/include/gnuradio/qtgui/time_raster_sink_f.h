/* -*- c++ -*- */
/*
 * Copyright 2012,2013,2015 Free Software Foundation, Inc.
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

#ifndef INCLUDED_QTGUI_TIME_RASTER_SINK_F_H
#define INCLUDED_QTGUI_TIME_RASTER_SINK_F_H

#ifdef ENABLE_PYTHON
#include <Python.h>
#endif

#include <gnuradio/qtgui/api.h>
#include <gnuradio/sync_block.h>
#include <qapplication.h>
#include <qwt_symbol.h>

namespace gr {
  namespace qtgui {

    /*!
     * \brief A graphical sink to display multiple signals on a
     * time_raster plot.
     * \ingroup instrumentation_blk
     * \ingroup qtgui_blk
     *
     * \details
     * This is a QT-based graphical sink that takes set of a floating
     * point streams and plots a time_raster (spectrogram) plot.
     *
     * The sink supports plotting streaming float data or
     * messages. The message port is named "in". The two modes cannot
     * be used simultaneously, and \p nconnections should be set to 0
     * when using the message mode. GRC handles this issue by
     * providing the "Float Message" type that removes the streaming
     * port(s).
     *
     * This sink can plot messages that contain either uniform vectors
     * of float 32 values (pmt::is_f32vector) or PDUs where the data
     * is a uniform vector of float 32 values.
     */
    class QTGUI_API time_raster_sink_f : virtual public sync_block
    {
    public:
      // gr::qtgui::time_raster_sink_f::sptr
      typedef boost::shared_ptr<time_raster_sink_f> sptr;

      /*!
       * \brief Build a floating point time raster sink.
       *
       * \param samp_rate sample rate of signal
       * \param cols number of cols to plot
       * \param rows number of rows to plot
       * \param mult vector of floats as a scaling multiplier for each input stream
       * \param offset vector of floats as an offset for each input stream
       * \param name title for the plot
       * \param nconnections number of streams connected
       * \param parent a QWidget parent object, if any
       */
      static sptr make(double samp_rate,
		       double rows, double cols,
		       const std::vector<float> &mult,
		       const std::vector<float> &offset,
		       const std::string &name,
		       int nconnections=1,
		       QWidget *parent=NULL);

      virtual void exec_() = 0;
      virtual QWidget* qwidget() = 0;

#ifdef ENABLE_PYTHON
      virtual PyObject* pyqwidget() = 0;
#else
      virtual void* pyqwidget() = 0;
#endif

      virtual void set_update_time(double t) = 0;
      virtual void set_title(const std::string &title) = 0;
      virtual void set_line_label(int which, const std::string &lable) = 0;
      virtual void set_line_color(int which, const std::string &color) = 0;
      virtual void set_line_width(int which, int width) = 0;
      virtual void set_line_style(int which, Qt::PenStyle style) = 0;
      virtual void set_line_marker(int which, QwtSymbol::Style marker) = 0;
      virtual void set_line_alpha(int which, double alpha) = 0;
      virtual void set_color_map(int which, const int color) = 0;

      virtual std::string title() = 0;
      virtual std::string line_label(int which) = 0;
      virtual std::string line_color(int which) = 0;
      virtual int line_width(int which) = 0;
      virtual int line_style(int which) = 0;
      virtual int line_marker(int which) = 0;
      virtual double line_alpha(int which) = 0;
      virtual int color_map(int which) = 0;

      virtual void set_size(int width, int height) = 0;

      virtual void set_samp_rate(const double samp_rate) = 0;
      virtual void set_num_rows(double rows) = 0;
      virtual void set_num_cols(double cols) = 0;

      virtual double num_rows() = 0;
      virtual double num_cols() = 0;

      virtual void set_multiplier(const std::vector<float> &mult) = 0;
      virtual void set_offset(const std::vector<float> &offset) = 0;

      virtual void set_intensity_range(float min, float max) = 0;

      virtual void enable_menu(bool en) = 0;
      virtual void enable_grid(bool en) = 0;
      virtual void enable_autoscale(bool en) = 0;
      virtual void enable_axis_labels(bool en=true) = 0;
      virtual void reset() = 0;

      QApplication *d_qApplication;
    };

  } /* namespace qtgui */
} /* namespace gr */

#endif /* INCLUDED_QTGUI_TIME_RASTER_SINK_F_H */

/* -*- c++ -*- */
/*
 * Copyright 2013,2015 Free Software Foundation, Inc.
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

#ifndef INCLUDED_QTGUI_HISTOGRAM_SINK_F_H
#define INCLUDED_QTGUI_HISTOGRAM_SINK_F_H

#ifdef ENABLE_PYTHON
#include <Python.h>
#endif

#include <gnuradio/qtgui/api.h>
#include <gnuradio/sync_block.h>
#include <qapplication.h>

namespace gr {
  namespace qtgui {

    /*!
     * \brief A graphical sink to display a histogram.
     * \ingroup instrumentation_blk
     * \ingroup qtgui_blk
     *
     * \details
     * This is a QT-based graphical sink the displays a histogram of
     * the data.
     *
     * This histogram allows you to set and change at runtime the
     * number of points to plot at once and the number of bins in the
     * histogram. Both x and y-axis have their own auto-scaling
     * behavior. By default, auto-scaling the y-axis is turned on and
     * continuously updates the y-axis max value based on the
     * currently plotted histogram.
     *
     * The x-axis auto-scaling function only updates once when
     * clicked. This resets the x-axis to the current range of minimum
     * and maximum values represented in the histogram. It resets any
     * values currently displayed because the location and width of
     * the bins may have changed.
     *
     * The histogram also has an accumulate function that simply
     * accumulates the data between calls to work. When accumulate is
     * activated, the y-axis autoscaling is turned on by default as
     * the values will quickly grow in the this direction.
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
    class QTGUI_API histogram_sink_f : virtual public sync_block
    {
    public:
      // gr::qtgui::histogram_sink_f::sptr
      typedef boost::shared_ptr<histogram_sink_f> sptr;

      /*!
       * \brief Build floating point histogram sink
       *
       * \param size number of points to plot at once
       * \param bins number of bins to sort the data into
       * \param xmin minimum x-axis value
       * \param xmax maximum x-axis value
       * \param name title for the plot
       * \param nconnections number of signals connected to sink
       * \param parent a QWidget parent object, if any
       */
      static sptr make(int size, int bins,
                       double xmin, double xmax,
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

    public:
      virtual std::string title() = 0;
      virtual std::string line_label(int which) = 0;
      virtual std::string line_color(int which) = 0;
      virtual int line_width(int which) = 0;
      virtual int line_style(int which) = 0;
      virtual int line_marker(int which) = 0;
      virtual double line_alpha(int which) = 0;

      virtual void set_size(int width, int height) = 0;

      virtual void enable_menu(bool en=true) = 0;
      virtual void enable_grid(bool en=true) = 0;
      virtual void enable_autoscale(bool en=true) = 0;
      virtual void enable_semilogx(bool en=true) = 0;
      virtual void enable_semilogy(bool en=true) = 0;
      virtual void enable_accumulate(bool en=true) = 0;
      virtual void enable_axis_labels(bool en=true) = 0;
      virtual void autoscalex() = 0;
      virtual int nsamps() const = 0;
      virtual int bins() const = 0;
      virtual void reset() = 0;

      QApplication *d_qApplication;

      virtual void set_y_axis(double min, double max) = 0;
      virtual void set_x_axis(double min, double max) = 0;
      virtual void set_update_time(double t) = 0;
      virtual void set_title(const std::string &title) = 0;
      virtual void set_line_label(int which, const std::string &line) = 0;
      virtual void set_line_color(int which, const std::string &color) = 0;
      virtual void set_line_width(int which, int width) = 0;
      virtual void set_line_style(int which, int style) = 0;
      virtual void set_line_marker(int which, int marker) = 0;
      virtual void set_line_alpha(int which, double alpha) = 0;
      virtual void set_nsamps(const int newsize) = 0;
      virtual void set_bins(const int bins) = 0;
      virtual void disable_legend() = 0;
    };

  } /* namespace qtgui */
} /* namespace gr */

#endif /* INCLUDED_QTGUI_HISTOGRAM_SINK_F_H */

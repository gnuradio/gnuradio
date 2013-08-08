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
#include <gnuradio/qtgui/api.h>
#include <gnuradio/sync_block.h>
#include <qapplication.h>

namespace gr {
  namespace qtgui {
    
    /*!
     * \brief A graphical sink to display multiple signals in time.
     * \ingroup instrumentation_blk
     * \ingroup qtgui_blk
     *
     * \details
     * This is a QT-based graphical sink the takes set of a float streams
     * and plots them in the time domain. Each signal is plotted with a
     * different color, and the \a set_title and \a set_color functions
     * can be used to change the lable and color for a given input number.
     */
    class QTGUI_API time_sink_f : virtual public sync_block
    {
    public:
      // gr::qtgui::time_sink_f::sptr
      typedef boost::shared_ptr<time_sink_f> sptr;

      /*!
       * \brief Build floating point time sink
       *
       * \param size number of points to plot at once
       * \param samp_rate sample rate (used to set x-axis labels)
       * \param name title for the plot
       * \param nconnections number of signals connected to sink
       * \param parent a QWidget parent object, if any
       */
      static sptr make(int size, double samp_rate,
		       const std::string &name,
		       int nconnections=1,
		       QWidget *parent=NULL);

      virtual void exec_() = 0;
      virtual PyObject* pyqwidget() = 0;

      virtual void set_y_axis(double min, double max) = 0;
      virtual void set_update_time(double t) = 0;
      virtual void set_title(const std::string &title) = 0;
      virtual void set_line_label(int which, const std::string &line) = 0;
      virtual void set_line_color(int which, const std::string &color) = 0;
      virtual void set_line_width(int which, int width) = 0;
      virtual void set_line_style(int which, int style) = 0;
      virtual void set_line_marker(int which, int marker) = 0;
      virtual void set_nsamps(const int newsize) = 0;
      virtual void set_samp_rate(const double samp_rate) = 0;
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
      virtual void enable_grid(bool en=true) = 0;
      virtual void enable_autoscale(bool en=true) = 0;
      virtual void enable_stem_plot(bool en=true) = 0;
      virtual void enable_semilogx(bool en=true) = 0;
      virtual void enable_semilogy(bool en=true) = 0;
      virtual int nsamps() const = 0;
      virtual void reset() = 0;

      QApplication *d_qApplication;
    };

  } /* namespace qtgui */
} /* namespace gr */

#endif /* INCLUDED_QTGUI_TIME_SINK_F_H */

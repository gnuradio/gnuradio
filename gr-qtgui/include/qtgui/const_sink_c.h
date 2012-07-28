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

#ifndef INCLUDED_QTGUI_CONST_SINK_C_H
#define INCLUDED_QTGUI_CONST_SINK_C_H

#include <qtgui/api.h>
#include <gr_sync_block.h>

#include <Python.h>
#include <qapplication.h>
#include <qwt_symbol.h>

namespace gr {
  namespace qtgui {
    
    /*!
     * \brief A graphical sink to display the IQ constellation of multiple signals.
     * \ingroup qtgui_blk
     *
     * This is a QT-based graphical sink the takes set of a complex
     * streams and plots them on an IQ constellation plot.
     */
    class QTGUI_API const_sink_c : virtual public gr_sync_block
    {
    public:
      // gr::qtgui::const_sink_c::sptr
      typedef boost::shared_ptr<const_sink_c> sptr;

      /*!
       * \brief Build a complex PSD sink.
       *
       * \param size number of points to plot at once
       * \param name title for the plot
       * \param nconnections number of signals connected to sink
       * \param parent a QWidget parent object, if any
       */
      static sptr make(int size,
		       const std::string &name,
		       int nconnections,
		       QWidget *parent=NULL);

      virtual void exec_() = 0;
      virtual PyObject* pyqwidget() = 0;

      virtual void set_update_time(double t) = 0;
      virtual void set_title(int which, const std::string &title) = 0;
      virtual void set_color(int which, const std::string &color) = 0;
      virtual void set_line_width(int which, int width) = 0;
      virtual void set_line_style(int which, Qt::PenStyle style) = 0;
      virtual void set_line_marker(int which, QwtSymbol::Style marker) = 0;
      virtual void set_nsamps(const int newsize) = 0;

      virtual void set_size(int width, int height) = 0;

      QApplication *d_qApplication;
    };

  } /* namespace qtgui */
} /* namespace gr */

#endif /* INCLUDED_QTGUI_CONST_SINK_C_H */

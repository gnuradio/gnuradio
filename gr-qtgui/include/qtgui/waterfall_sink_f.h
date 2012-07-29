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

#ifndef INCLUDED_QTGUI_WATERFALL_SINK_F_H
#define INCLUDED_QTGUI_WATERFALL_SINK_F_H

#include <qtgui/api.h>
#include <gr_sync_block.h>

#include <Python.h>
#include <qapplication.h>
#include <qwt_symbol.h>

namespace gr {
  namespace qtgui {

    /*!
     * \brief A graphical sink to display multiple signals on a
     * waterfall (spectrogram) plot.
     * \ingroup qtgui_blk
     *
     * This is a QT-based graphical sink the takes set of a floating
     * point streams and plots a waterfall (spectrogram) plot.
     *
     * Note that unlike the other qtgui sinks, this one does not
     * support multiple input streams. We have yet to figure out a
     * good way to display multiple, independent signals on this kind
     * of a plot. If there are any suggestions or examples of this, we
     * would love to see them. Otherwise, to display multiple signals
     * here, it's probably best to sum the signals together and
     * connect that here.
     */
    class QTGUI_API waterfall_sink_f : virtual public gr_sync_block
    {
    public:
      // gr::qtgui::waterfall_sink_f::sptr
      typedef boost::shared_ptr<waterfall_sink_f> sptr;

      /*!
       * \brief Build a floating point waterfall sink.
       *
       * \param size size of the FFT to compute and display
       * \param wintype type of window to apply (see filter/firdes.h)
       * \param fc center frequency of signal (use for x-axis labels)
       * \param bw bandwidth of signal (used to set x-axis labels)
       * \param name title for the plot
       * \param parent a QWidget parent object, if any
       */
      static sptr make(int size, int wintype,
		       double fc, double bw,
		       const std::string &name,
		       QWidget *parent=NULL);

      virtual void exec_() = 0;
      virtual PyObject* pyqwidget() = 0;

      virtual void set_fft_size(const int fftsize) = 0;
      virtual int fft_size() const = 0;
      virtual void set_fft_average(const float fftavg) = 0;
      virtual float fft_average() const = 0;

      virtual void set_frequency_range(const double centerfreq,
				       const double bandwidth) = 0;

      virtual void set_update_time(double t) = 0;
      virtual void set_title(const std::string &title) = 0;
      virtual void set_color(const std::string &color) = 0;
      virtual void set_line_width(int width) = 0;
      virtual void set_line_style(Qt::PenStyle style) = 0;
      virtual void set_line_marker(QwtSymbol::Style marker) = 0;

      virtual void set_size(int width, int height) = 0;

      QApplication *d_qApplication;
    };

  } /* namespace qtgui */
} /* namespace gr */

#endif /* INCLUDED_QTGUI_WATERFALL_SINK_F_H */

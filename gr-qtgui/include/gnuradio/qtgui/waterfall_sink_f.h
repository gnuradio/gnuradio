/* -*- c++ -*- */
/*
 * Copyright 2012,2014 Free Software Foundation, Inc.
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

#ifdef ENABLE_PYTHON
#include <Python.h>
#endif

#include <gnuradio/qtgui/api.h>
#include <gnuradio/sync_block.h>
#include <qapplication.h>
#include <gnuradio/filter/firdes.h>

namespace gr {
  namespace qtgui {

    /*!
     * \brief A graphical sink to display multiple signals on a
     * waterfall (spectrogram) plot.
     * \ingroup instrumentation_blk
     * \ingroup qtgui_blk
     *
     * \details
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
     *
     * Message Ports:
     *
     * - freq (input):
     *        Receives a PMT pair: (intern("freq"), double(frequency).
     *        This is used to retune the center frequency of the
     *        display's x-axis.
     *
     * - freq (output):
     *        Produces a PMT pair with (intern("freq"), double(frequency).
     *        When a user double-clicks on the display, the block
     *        produces and emits a message containing the frequency of
     *        where on the x-axis the user clicked. This value can be
     *        used by other blocks to update their frequency setting.
     *
     *        To perform click-to-tune behavior, this output 'freq'
     *        port can be redirected to this block's input 'freq' port
     *        to catch the message and update the center frequency of
     *        the display.
     */
    class QTGUI_API waterfall_sink_f : virtual public sync_block
    {
    public:
      // gr::qtgui::waterfall_sink_f::sptr
      typedef boost::shared_ptr<waterfall_sink_f> sptr;

      /*!
       * \brief Build a floating point waterfall sink.
       *
       * \param size size of the FFT to compute and display
       * \param wintype type of window to apply (see gnuradio/filter/firdes.h)
       * \param fc center frequency of signal (use for x-axis labels)
       * \param bw bandwidth of signal (used to set x-axis labels)
       * \param name title for the plot
       * \param nconnections number of signals to be connected to the sink
       * \param parent a QWidget parent object, if any
       */
      static sptr make(int size, int wintype,
		       double fc, double bw,
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

      virtual void clear_data() = 0;

      virtual void set_fft_size(const int fftsize) = 0;
      virtual int fft_size() const = 0;
      virtual void set_fft_average(const float fftavg) = 0;
      virtual float fft_average() const = 0;
      virtual void set_fft_window(const gr::filter::firdes::win_type win) = 0;
      virtual gr::filter::firdes::win_type fft_window() = 0;

      virtual void set_frequency_range(const double centerfreq,
				       const double bandwidth) = 0;
      virtual void set_intensity_range(const double min,
				       const double max) = 0;

      virtual void set_update_time(double t) = 0;
      virtual void set_title(const std::string &title) = 0;
      virtual void set_line_label(int which, const std::string &line) = 0;
      virtual void set_line_alpha(int which, double alpha) = 0;
      virtual void set_color_map(int which, const int color) = 0;

      /*!
       *  Pass "true" to this function to only show the positive half
       *  of the spectrum. By default, this plotter shows the full
       *  spectrum (positive and negative halves).
       */
      virtual void set_plot_pos_half(bool half) = 0;

      virtual std::string title() = 0;
      virtual std::string line_label(int which) = 0;
      virtual double line_alpha(int which) = 0;
      virtual int color_map(int which) = 0;

      virtual void set_size(int width, int height) = 0;

      virtual void auto_scale() = 0;
      virtual double min_intensity(int which) = 0;
      virtual double max_intensity(int which) = 0;

      virtual void enable_menu(bool en=true) = 0;
      virtual void enable_grid(bool en=true) = 0;

      QApplication *d_qApplication;
    };

  } /* namespace qtgui */
} /* namespace gr */

#endif /* INCLUDED_QTGUI_WATERFALL_SINK_F_H */

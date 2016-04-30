/* -*- c++ -*- */
/*
 * Copyright 2012,2014-2015 Free Software Foundation, Inc.
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

#ifndef INCLUDED_QTGUI_FREQ_SINK_C_H
#define INCLUDED_QTGUI_FREQ_SINK_C_H

#ifdef ENABLE_PYTHON
#include <Python.h>
#endif

#include <gnuradio/qtgui/api.h>
#include <gnuradio/qtgui/trigger_mode.h>
#include <gnuradio/sync_block.h>
#include <qapplication.h>
#include <gnuradio/filter/firdes.h>

namespace gr {
  namespace qtgui {

    /*!
     * \brief A graphical sink to display multiple signals in frequency.
     * \ingroup instrumentation_blk
     * \ingroup qtgui_blk
     *
     * \details
     * This is a QT-based graphical sink the takes set of a complex
     * streams and plots the PSD. Each signal is plotted with a
     * different color, and the \a set_title and \a set_color
     * functions can be used to change the lable and color for a given
     * input number.
     *
     * The sink supports plotting streaming complex data or
     * messages. The message port is named "in". The two modes cannot
     * be used simultaneously, and \p nconnections should be set to 0
     * when using the message mode. GRC handles this issue by
     * providing the "Complex Message" type that removes the streaming
     * port(s).
     *
     * This sink can plot messages that contain either uniform vectors
     * of complex 32 values (pmt::is_c32vector) or PDUs where the data
     * is a uniform vector of complex 32 values.
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
    class QTGUI_API freq_sink_c : virtual public sync_block
    {
    public:
      // gr::qtgui::freq_sink_c::sptr
      typedef boost::shared_ptr<freq_sink_c> sptr;

      /*!
       * \brief Build a complex PSD sink.
       *
       * \param fftsize size of the FFT to compute and display. If using
       *        the PDU message port to plot samples, the length of
       *        each PDU must be a multiple of the FFT size.
       * \param wintype type of window to apply (see gr::fft::window::win_type)
       * \param fc center frequency of signal (use for x-axis labels)
       * \param bw bandwidth of signal (used to set x-axis labels)
       * \param name title for the plot
       * \param nconnections number of signals to be connected to the
       *        sink. The PDU message port is always available for a
       *        connection, and this value must be set to 0 if only
       *        the PDU message port is being used.
       * \param parent a QWidget parent object, if any
       */
      static sptr make(int fftsize, int wintype,
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

      virtual void set_fft_size(const int fftsize) = 0;
      virtual int fft_size() const = 0;
      virtual void set_fft_average(const float fftavg) = 0;
      virtual float fft_average() const = 0;
      virtual void set_fft_window(const gr::filter::firdes::win_type win) = 0;
      virtual gr::filter::firdes::win_type fft_window() = 0;

      virtual void set_frequency_range(const double centerfreq, const double bandwidth) = 0;
      virtual void set_y_axis(double min, double max) = 0;

      virtual void set_update_time(double t) = 0;

      virtual void set_title(const std::string &title) = 0;
      virtual void set_y_label(const std::string &label, const std::string &unit) = 0;
      virtual void set_line_label(int which, const std::string &label) = 0;
      virtual void set_line_color(int which, const std::string &color) = 0;
      virtual void set_line_width(int which, int width) = 0;
      virtual void set_line_style(int which, int style) = 0;
      virtual void set_line_marker(int which, int marker) = 0;
      virtual void set_line_alpha(int which, double alpha) = 0;

      /*!
       * Set up a trigger for the sink to know when to start
       * plotting. Useful to isolate events and avoid noise.
       *
       * The trigger modes are Free, Auto, Normal, and Tag (see
       * gr::qtgui::trigger_mode). The first three are like a normal
       * trigger function. Free means free running with no trigger,
       * auto will trigger if the trigger event is seen, but will
       * still plot otherwise, and normal will hold until the trigger
       * event is observed. The Tag trigger mode allows us to trigger
       * off a specific stream tag. The tag trigger is based only on
       * the name of the tag, so when a tag of the given name is seen,
       * the trigger is activated.
       *
       * In auto and normal mode, we look to see if the magnitude of
       * the any FFT point is over the set level.
       *
       * \param mode The trigger_mode: free, auto, normal, or tag.
       * \param level The magnitude of the trigger even for auto or normal modes.
       * \param channel Which input channel to use for the trigger events.
       * \param tag_key The name (as a string) of the tag to trigger off
       *                of if using the tag mode.
       */
      virtual void set_trigger_mode(trigger_mode mode,
                                    float level, int channel,
                                    const std::string &tag_key="") = 0;

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
      virtual void enable_control_panel(bool en=true) = 0;
      virtual void enable_max_hold(bool en) = 0;
      virtual void enable_min_hold(bool en) = 0;
      virtual void clear_max_hold() = 0;
      virtual void clear_min_hold() = 0;
      virtual void disable_legend() = 0;
      virtual void reset() = 0;
      virtual void enable_axis_labels(bool en=true) = 0;

      QApplication *d_qApplication;
    };

  } /* namespace qtgui */
} /* namespace gr */

#endif /* INCLUDED_QTGUI_FREQ_SINK_C_H */

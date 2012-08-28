/* -*- c++ -*- */
/*
 * Copyright 2008,2009,2011,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_QTGUI_SINK_C_H
#define INCLUDED_QTGUI_SINK_C_H

#include <qtgui/api.h>
#include <gr_block.h>

#include <Python.h>
#include <qapplication.h>
#include <qwt_symbol.h>


namespace gr {
  namespace qtgui {

    /*!
     * \brief A graphical sink to display freq, spec, time, and const plots.
     * \ingroup qtgui_blk
     *
     * This is a QT-based graphical sink the takes a complex stream and
     * plots it. The default action is to plot the signal as a PSD (FFT),
     * spectrogram (waterfall), time domain I&Q, and constellation (I
     * vs. Q) plots. The plots may be turned off by setting the
     * appropriate boolean value in the constructor to False.
     */

    class QTGUI_API sink_c : virtual public gr_block
    {
    public:
      // gr::qtgui::sink_c::sptr
      typedef boost::shared_ptr<sink_c> sptr;

      /*!
       * \brief Build a complex qtgui sink.
       *
       * \param fftsize size of the FFT to compute and display
       * \param wintype type of window to apply (see filter/firdes.h)
       * \param fc center frequency of signal (use for x-axis labels)
       * \param bw bandwidth of signal (used to set x-axis labels)
       * \param name title for the plot
       * \param plotfreq Toggle frequency plot on/off
       * \param plotwaterfall Toggle waterfall plot on/off
       * \param plottime Toggle time plot on/off
       * \param plotconst Toggle constellation plot on/off
       * \param parent a QWidget parent object, if any
       */
      static sptr make(int fftsize, int wintype,
		       double fc, double bw,
		       const std::string &name,
		       bool plotfreq, bool plotwaterfall,
		       bool plottime, bool plotconst,
		       QWidget *parent=NULL);

      virtual void exec_() = 0;
      virtual PyObject* pyqwidget() = 0;

      virtual void set_fft_size(const int fftsize) = 0;
      virtual int fft_size() const = 0;

      virtual void set_frequency_range(const double centerfreq,
				       const double bandwidth) = 0;
      virtual void set_fft_power_db(double min, double max) = 0;

      //void set_time_domain_axis(double min, double max);
      //void set_constellation_axis(double xmin, double xmax,
      //                            double ymin, double ymax);
      //void set_constellation_pen_size(int size);

      virtual void set_update_time(double t) = 0;

      QApplication *d_qApplication;
    };

  } /* namespace qtgui */
} /* namespace gr */

#endif /* INCLUDED_QTGUI_SINK_C_H */

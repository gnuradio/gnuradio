/* -*- c++ -*- */
/*
 * Copyright 2008,2009,2011,2012,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_QTGUI_SINK_C_H
#define INCLUDED_QTGUI_SINK_C_H

#ifdef ENABLE_PYTHON
#pragma push_macro("slots")
#undef slots
#include "Python.h"
#pragma pop_macro("slots")
#endif

#include <gnuradio/block.h>
#include <gnuradio/qtgui/api.h>
#include <qapplication.h>
#include <qwt_legend.h>


namespace gr {
namespace qtgui {

/*!
 * \brief A graphical sink to display freq, spec, time, and const plots.
 * \ingroup instrumentation_blk
 * \ingroup qtgui_blk
 *
 * \details
 * This is a QT-based graphical sink the takes a complex stream and
 * plots it. The default action is to plot the signal as a PSD (FFT),
 * spectrogram (waterfall), time domain I&Q, and constellation (I
 * vs. Q) plots. The plots may be turned off by setting the
 * appropriate boolean value in the constructor to False.
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
class QTGUI_API sink_c : virtual public block
{
public:
    // gr::qtgui::sink_c::sptr
    typedef std::shared_ptr<sink_c> sptr;

    /*!
     * \brief Build a complex qtgui sink.
     *
     * \param fftsize size of the FFT to compute and display
     * \param wintype type of window to apply (see gnuradio/filter/firdes.h)
     * \param fc center frequency of signal (use for x-axis labels)
     * \param bw bandwidth of signal (used to set x-axis labels)
     * \param name title for the plot
     * \param plotfreq Toggle frequency plot on/off
     * \param plotwaterfall Toggle waterfall plot on/off
     * \param plottime Toggle time plot on/off
     * \param plotconst Toggle constellation plot on/off
     * \param parent a QWidget parent object, if any
     */
    static sptr make(int fftsize,
                     int wintype,
                     double fc,
                     double bw,
                     const std::string& name,
                     bool plotfreq,
                     bool plotwaterfall,
                     bool plottime,
                     bool plotconst,
                     QWidget* parent = nullptr);

    virtual void exec_() = 0;
    virtual QWidget* qwidget() = 0;

    virtual void set_fft_size(const int fftsize) = 0;
    virtual int fft_size() const = 0;

    virtual void set_frequency_range(const double centerfreq, const double bandwidth) = 0;
    virtual void set_fft_power_db(double min, double max) = 0;
    virtual void enable_rf_freq(bool en) = 0;

    virtual void set_update_time(double t) = 0;

    QApplication* d_qApplication;
};

} /* namespace qtgui */
} /* namespace gr */

#endif /* INCLUDED_QTGUI_SINK_C_H */

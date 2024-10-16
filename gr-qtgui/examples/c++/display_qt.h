/*
 * Copyright 2016 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

// Q_MOC_RUN is a workaround for a QT4 + Boost version issue
#ifndef Q_MOC_RUN
#include <gnuradio/analog/noise_source.h>
#include <gnuradio/analog/sig_source.h>
#include <gnuradio/blocks/add_blk.h>
#include <gnuradio/blocks/throttle.h>
#include <gnuradio/fft/window.h>
#include <gnuradio/qtgui/freq_sink_f.h>
#include <gnuradio/qtgui/histogram_sink_f.h>
#include <gnuradio/qtgui/time_sink_f.h>
#include <gnuradio/qtgui/waterfall_sink_f.h>
#include <gnuradio/top_block.h>
#endif

#include <QHBoxLayout>
#include <QTabWidget>
#include <QWidget>

class mywindow : public QWidget
{
    Q_OBJECT

private:
    QHBoxLayout* layout;
    QTabWidget* tab0;
    QTabWidget* tab1;
    QWidget* qtgui_time_sink_win;
    QWidget* qtgui_freq_sink_win;
    QWidget* qtgui_waterfall_sink_win;
    QWidget* qtgui_histogram_sink_win;

#ifndef Q_MOC_RUN
    gr::top_block_sptr tb;
    gr::analog::sig_source_f::sptr src0;
    gr::analog::noise_source_f::sptr src1;
    gr::blocks::add_ff::sptr src;
    gr::blocks::throttle::sptr thr;
    gr::qtgui::time_sink_f::sptr tsnk;
    gr::qtgui::freq_sink_f::sptr fsnk;
    gr::qtgui::waterfall_sink_f::sptr wsnk;
    gr::qtgui::histogram_sink_f::sptr hsnk;
#endif

public slots:
    // Stop the topblock before shutting down the window
    void quitting();

public:
    mywindow();
    ~mywindow() override;

    // call start() on the topblock
    void start();
};

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

using namespace gr;

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
    top_block_sptr tb;
    analog::sig_source_f::sptr src0;
    analog::noise_source_f::sptr src1;
    blocks::add_ff::sptr src;
    blocks::throttle::sptr thr;
    qtgui::time_sink_f::sptr tsnk;
    qtgui::freq_sink_f::sptr fsnk;
    qtgui::waterfall_sink_f::sptr wsnk;
    qtgui::histogram_sink_f::sptr hsnk;
#endif

public slots:
    // Stop the topblock before shutting down the window
    void quitting();

public:
    mywindow();
    virtual ~mywindow();

    // call start() on the topblock
    void start();
};

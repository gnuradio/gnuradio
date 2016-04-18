/*
 * Copyright 2016 Free Software Foundation, Inc.
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

// Q_MOC_RUN is a workaround for a QT4 + Boost version issue
#ifndef Q_MOC_RUN
#include <gnuradio/top_block.h>
#include <gnuradio/analog/sig_source_f.h>
#include <gnuradio/analog/noise_source_f.h>
#include <gnuradio/blocks/add_ff.h>
#include <gnuradio/blocks/throttle.h>
#include <gnuradio/qtgui/time_sink_f.h>
#include <gnuradio/qtgui/freq_sink_f.h>
#include <gnuradio/qtgui/waterfall_sink_f.h>
#include <gnuradio/qtgui/histogram_sink_f.h>
#include <gnuradio/fft/window.h>
#endif

#include <QWidget>
#include <QHBoxLayout>
#include <QTabWidget>

using namespace gr;

class mywindow : public QWidget
{
  Q_OBJECT

private:
  QHBoxLayout *layout;
  QTabWidget *tab0;
  QTabWidget *tab1;
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

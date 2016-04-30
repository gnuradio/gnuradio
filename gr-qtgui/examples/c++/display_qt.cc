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

#include "display_qt.h"

mywindow::mywindow()
  : QWidget()
{
  // We'll use a horizontal layout of two QTabWidgets
  layout = new QHBoxLayout();

  // Create the tab widgets
  tab0 = new QTabWidget();
  tab1 = new QTabWidget();

  // Add the tab widgets to the layou
  layout->addWidget(tab0);
  layout->addWidget(tab1);

  // Set the layout as the main widget's layou
  setLayout(layout);

  // Simple resizing of the app
  resize(2000,800);

  // sample rate
  int rate = 48000;

  // Create the GNU Radio top block
  tb = make_top_block("display_qt");

  // Source will be sine wave in noise
  src0 = analog::sig_source_f::make(rate, analog::GR_SIN_WAVE, 1500, 1);
  src1 = analog::noise_source_f::make(analog::GR_GAUSSIAN, 0.1);

  // Combine signal and noise; add throttle
  src = blocks::add_ff::make();
  thr = blocks::throttle::make(sizeof(float), rate);

  // Create the QTGUI sinks
  // Time, Freq, Waterfall, and Histogram sinks
  tsnk = qtgui::time_sink_f::make(1024, rate, "", 1);
  fsnk = qtgui::freq_sink_f::make(1024, fft::window::WIN_HANN,
                                  0, rate, "", 1);
  wsnk = qtgui::waterfall_sink_f::make(1024, fft::window::WIN_HANN,
                                       0, rate, "", 1);
  hsnk = qtgui::histogram_sink_f::make(1024, 100, -2, 2, "", 1);

  // Turn off the legend on these plots
  tsnk->disable_legend();
  fsnk->disable_legend();
  hsnk->disable_legend();

  // Connect the graph
  tb->connect(src0, 0, src, 0);
  tb->connect(src1, 0, src, 1);
  tb->connect(src, 0, thr, 0);
  tb->connect(thr, 0, tsnk, 0);
  tb->connect(thr, 0, fsnk, 0);
  tb->connect(thr, 0, wsnk, 0);
  tb->connect(thr, 0, hsnk, 0);

  // Get the raw QWidget objects from the GNU Radio blocks
  qtgui_time_sink_win = tsnk->qwidget();
  qtgui_freq_sink_win = fsnk->qwidget();
  qtgui_waterfall_sink_win = wsnk->qwidget();
  qtgui_histogram_sink_win = hsnk->qwidget();

  // Plug the widgets into the tabs
  tab0->addTab(qtgui_time_sink_win, "Time");
  tab0->addTab(qtgui_histogram_sink_win, "Hist");
  tab1->addTab(qtgui_freq_sink_win, "Freq");
  tab1->addTab(qtgui_waterfall_sink_win, "Waterfall");
}

mywindow::~mywindow()
{
}

void
mywindow::start()
{
  tb->start();
}

void
mywindow::quitting()
{
  tb->stop();
  tb->wait();
}

int main(int argc, char **argv)
{
  // The global QT application
  QApplication app(argc, argv);

  mywindow *w = new mywindow();

  QObject::connect(&app, SIGNAL(aboutToQuit()),
                   w, SLOT(quitting()));

  w->start();   // Start the flowgraph
  w->show();    // show the window
  app.exec();   // run the QT executor loop

  return 0;
}

/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
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

#ifndef FREQ_CONTROL_PANEL_H
#define FREQ_CONTROL_PANEL_H

#include <QtGui/QtGui>
#include <vector>
#include <QSlider>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <gnuradio/qtgui/displayform.h>
#include <gnuradio/qtgui/freqdisplayform.h>

class FreqControlPanel : public QVBoxLayout
{
  Q_OBJECT

public:
  FreqControlPanel(FreqDisplayForm *form);
  ~FreqControlPanel();

public slots:
  void notifyAvgSlider(int val);
  void setFFTAverage(float val);
  void toggleGrid(bool en);
  void toggleAxisLabels(bool en);
  void toggleMaxHold(bool en);
  void toggleMinHold(bool en);

  void toggleFFTSize(int val);
  void toggleFFTWindow(const gr::filter::firdes::win_type win);

  void toggleTriggerMode(gr::qtgui::trigger_mode mode);

  void toggleStopButton();

signals:
  void signalAvgSlider(float val);
  void signalAvg(bool en);
  void signalToggleStopButton();

private:
  FreqDisplayForm *d_parent;

  QGroupBox *d_trace_box;
  QVBoxLayout *d_trace_layout;
  QCheckBox *d_maxhold_check;
  QCheckBox *d_minhold_check;

  QHBoxLayout *d_avg_layout;
  QLabel *d_avg_label;
  QSlider *d_avg_slider;

  QGroupBox *d_axes_box;
  QVBoxLayout *d_axes_layout;

  QCheckBox *d_grid_check;
  QCheckBox *d_axislabels_check;
  QHBoxLayout *d_yrange_layout;
  QLabel *d_yrange_label;
  QPushButton *d_yrange_plus;
  QPushButton *d_yrange_minus;

  QHBoxLayout *d_ymin_layout;
  QLabel *d_ymin_label;
  QPushButton *d_ymin_plus;
  QPushButton *d_ymin_minus;

  QPushButton *d_autoscale_button;

  QGroupBox *d_fft_box;
  QVBoxLayout *d_fft_layout;
  QComboBox *d_fft_size_combo;
  QComboBox *d_fft_win_combo;

  int d_slider_max, d_slider_min, d_slider_step;

  QGroupBox *d_trigger_box;
  QVBoxLayout *d_trigger_layout;
  QHBoxLayout *d_trigger_level_layout;
  QLabel *d_trigger_level_label;
  QComboBox *d_trigger_mode_combo;
  QPushButton *d_trigger_level_plus, *d_trigger_level_minus;

  QGroupBox *d_extras_box;
  QVBoxLayout *d_extras_layout;
  QPushButton *d_stop_button;
};

#endif /* FREQ_CONTROL_PANEL_H */

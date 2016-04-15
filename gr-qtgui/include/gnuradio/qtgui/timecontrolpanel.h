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

#ifndef TIME_CONTROL_PANEL_H
#define TIME_CONTROL_PANEL_H

#include <QtGui/QtGui>
#include <vector>
#include <QSlider>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <gnuradio/qtgui/displayform.h>
#include <gnuradio/qtgui/timedisplayform.h>

class TimeControlPanel : public QVBoxLayout
{
  Q_OBJECT

public:
  TimeControlPanel(TimeDisplayForm *form);
  ~TimeControlPanel();

public slots:
  void toggleAutoScale(bool en);
  void toggleGrid(bool en);
  void toggleTriggerMode(gr::qtgui::trigger_mode mode);
  void toggleTriggerSlope(gr::qtgui::trigger_slope slope);
  void toggleStopButton();

signals:
  void signalToggleStopButton();

private:
  TimeDisplayForm *d_parent;
  QGroupBox *d_axes_box;
  QGroupBox *d_trigger_box;
  QGroupBox *d_extras_box;

  QVBoxLayout *d_axes_layout;
  QHBoxLayout *d_yoff_layout;
  QHBoxLayout *d_yrange_layout;
  QHBoxLayout *d_xmax_layout;
  QVBoxLayout *d_trigger_layout;
  QHBoxLayout *d_trigger_level_layout;
  QHBoxLayout *d_trigger_delay_layout;
  QVBoxLayout *d_extras_layout;

  QLabel *d_yoff_label;
  QLabel *d_yrange_label;
  QLabel *d_xmax_label;
  QLabel *d_trigger_level_label;
  QLabel *d_trigger_delay_label;

  QCheckBox *d_autoscale_check;
  QCheckBox *d_grid_check;
  QCheckBox *d_axislabels_check;

  QPushButton *d_yoff_plus, *d_yoff_minus;
  QPushButton *d_yrange_plus, *d_yrange_minus;
  QPushButton *d_xmax_plus, *d_xmax_minus;
  QComboBox *d_trigger_mode_combo;
  QComboBox *d_trigger_slope_combo;
  QPushButton *d_trigger_level_plus, *d_trigger_level_minus;
  QPushButton *d_trigger_delay_plus, *d_trigger_delay_minus;
  QPushButton *d_autoscale_button;
  QPushButton *d_stop_button;
};

#endif /* TIME_CONTROL_PANEL_H */

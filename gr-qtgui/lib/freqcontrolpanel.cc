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

#include <gnuradio/qtgui/freqcontrolpanel.h>
#include <cmath>

FreqControlPanel::FreqControlPanel(FreqDisplayForm *form)
  : QVBoxLayout(),
    d_parent(form)
{
  // Set up the box for trace items
  d_trace_box = new QGroupBox("Trace Options");
  d_trace_layout = new QVBoxLayout;
  d_maxhold_check = new QCheckBox("Max Hold");
  d_minhold_check = new QCheckBox("Min Hold");

  d_avg_layout = new QHBoxLayout;
  d_avg_label = new QLabel("Avg:");
  d_slider_max = 100;
  d_slider_min = 1;
  d_slider_step = 1;
  d_avg_slider = new QSlider(Qt::Horizontal);
  d_avg_slider->setRange(d_slider_min, d_slider_max);
  d_avg_slider->setTickInterval(d_slider_step);
  d_avg_slider->setValue(d_slider_max);
  d_avg_layout->addWidget(d_avg_label);
  d_avg_layout->addWidget(d_avg_slider);

  // Set up the box for axis items
  d_axes_box = new QGroupBox("Axis Options");
  d_axes_layout = new QVBoxLayout;

  d_grid_check = new QCheckBox("Grid");
  d_axislabels_check = new QCheckBox("Axis Labels");
  d_yrange_layout = new QHBoxLayout;
  d_yrange_label = new QLabel("Y Range:");
  d_yrange_plus = new QPushButton("+");
  d_yrange_minus = new QPushButton("-");
  d_yrange_plus->setMaximumWidth(30);
  d_yrange_plus->setMinimumWidth(30);
  d_yrange_minus->setMaximumWidth(30);
  d_yrange_minus->setMinimumWidth(30);
  d_yrange_layout->addWidget(d_yrange_label);
  d_yrange_layout->addWidget(d_yrange_plus);
  d_yrange_layout->addWidget(d_yrange_minus);

  d_ymin_layout = new QHBoxLayout;
  d_ymin_label = new QLabel("Ref Level:");
  d_ymin_plus = new QPushButton("+");
  d_ymin_minus = new QPushButton("-");
  d_ymin_plus->setMaximumWidth(30);
  d_ymin_plus->setMinimumWidth(30);
  d_ymin_minus->setMaximumWidth(30);
  d_ymin_minus->setMinimumWidth(30);
  d_ymin_layout->addWidget(d_ymin_label);
  d_ymin_layout->addWidget(d_ymin_plus);
  d_ymin_layout->addWidget(d_ymin_minus);

  d_autoscale_button = new QPushButton("Autoscale");


  // Set up the box for FFT settings
  d_fft_box = new QGroupBox("FFT");
  d_fft_layout = new QVBoxLayout;
  d_fft_size_combo = new QComboBox();
  d_fft_size_combo->addItem("32");
  d_fft_size_combo->addItem("64");
  d_fft_size_combo->addItem("128");
  d_fft_size_combo->addItem("256");
  d_fft_size_combo->addItem("512");
  d_fft_size_combo->addItem("1024");
  d_fft_size_combo->addItem("2048");
  d_fft_size_combo->addItem("4096");

  d_fft_win_combo = new QComboBox();
  d_fft_win_combo->addItem("None");
  d_fft_win_combo->addItem("Hamming");
  d_fft_win_combo->addItem("Hann");
  d_fft_win_combo->addItem("Blackman");
  d_fft_win_combo->addItem("Blackman-harris");
  d_fft_win_combo->addItem("Rectangular");
  d_fft_win_combo->addItem("Kaiser");
  d_fft_win_combo->addItem("Flat-top");

  // Trigger box setup
  d_trigger_box = new QGroupBox("Trigger");
  d_trigger_layout = new QVBoxLayout;
  d_trigger_mode_combo = new QComboBox();
  d_trigger_mode_combo->addItem("Free");
  d_trigger_mode_combo->addItem("Auto");
  d_trigger_mode_combo->addItem("Normal");
  d_trigger_mode_combo->addItem("Tag");

  d_trigger_level_layout = new QHBoxLayout;
  d_trigger_level_label = new QLabel("Level:");
  d_trigger_level_plus = new QPushButton("+");
  d_trigger_level_minus = new QPushButton("-");
  d_trigger_level_plus->setMaximumWidth(30);
  d_trigger_level_minus->setMaximumWidth(30);
  d_trigger_level_layout->addWidget(d_trigger_level_label);
  d_trigger_level_layout->addWidget(d_trigger_level_plus);
  d_trigger_level_layout->addWidget(d_trigger_level_minus);


  // Set up the box for other items
  d_extras_box = new QGroupBox("Extras");
  d_extras_layout = new QVBoxLayout;
  d_stop_button = new QPushButton("Stop");
  d_stop_button->setCheckable(true);

  // Set up the boxes into the layout
  d_trace_layout->addWidget(d_maxhold_check);
  d_trace_layout->addWidget(d_minhold_check);
  d_trace_layout->addLayout(d_avg_layout);
  d_trace_box->setLayout(d_trace_layout);

  d_axes_layout->addWidget(d_grid_check);
  d_axes_layout->addWidget(d_axislabels_check);
  d_axes_layout->addLayout(d_yrange_layout);
  d_axes_layout->addLayout(d_ymin_layout);
  d_axes_layout->addWidget(d_autoscale_button);
  d_axes_box->setLayout(d_axes_layout);

  d_fft_layout->addWidget(d_fft_size_combo);
  d_fft_layout->addWidget(d_fft_win_combo);
  d_fft_box->setLayout(d_fft_layout);

  d_trigger_layout->addWidget(d_trigger_mode_combo);
  d_trigger_layout->addLayout(d_trigger_level_layout);
  d_trigger_box->setLayout(d_trigger_layout);

  d_extras_layout->addWidget(d_stop_button);
  d_extras_box->setLayout(d_extras_layout);

  addWidget(d_trace_box);
  addWidget(d_axes_box);
  addWidget(d_fft_box);
  addWidget(d_trigger_box);
  addWidget(d_extras_box);

  addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum,
                          QSizePolicy::Expanding));

  connect(d_maxhold_check, SIGNAL(clicked(bool)),
	  d_parent, SLOT(notifyMaxHold(bool)));
  connect(d_minhold_check, SIGNAL(clicked(bool)),
	  d_parent, SLOT(notifyMinHold(bool)));

  connect(d_avg_slider, SIGNAL(valueChanged(int)),
	  this, SLOT(notifyAvgSlider(int)));
  connect(this, SIGNAL(signalAvgSlider(float)),
	  d_parent, SLOT(setFFTAverage(float)));

  connect(d_grid_check, SIGNAL(clicked(bool)),
	  d_parent, SLOT(setGrid(bool)));

  connect(d_axislabels_check, SIGNAL(clicked(bool)),
	  d_parent, SLOT(setAxisLabels(bool)));

  connect(d_ymin_plus, SIGNAL(pressed(void)),
	  d_parent, SLOT(notifyYAxisPlus(void)));
  connect(d_ymin_minus, SIGNAL(pressed(void)),
	  d_parent, SLOT(notifyYAxisMinus(void)));
  connect(d_yrange_plus, SIGNAL(pressed(void)),
	  d_parent, SLOT(notifyYRangePlus(void)));
  connect(d_yrange_minus, SIGNAL(pressed(void)),
	  d_parent, SLOT(notifyYRangeMinus(void)));

  connect(d_autoscale_button, SIGNAL(pressed(void)),
	  d_parent, SLOT(autoScaleShot(void)));

  connect(d_fft_size_combo, SIGNAL(currentIndexChanged(const QString&)),
	  d_parent, SLOT(notifyFFTSize(const QString&)));
  connect(d_fft_win_combo, SIGNAL(currentIndexChanged(const QString&)),
	  d_parent, SLOT(notifyFFTWindow(const QString&)));

  connect(d_trigger_mode_combo, SIGNAL(currentIndexChanged(const QString&)),
	  d_parent, SLOT(notifyTriggerMode(const QString&)));
  connect(d_trigger_level_plus, SIGNAL(pressed(void)),
	  d_parent, SLOT(notifyTriggerLevelPlus()));
  connect(d_trigger_level_minus, SIGNAL(pressed(void)),
	  d_parent, SLOT(notifyTriggerLevelMinus()));

  connect(d_stop_button, SIGNAL(pressed(void)),
          d_parent, SLOT(setStop(void)));
  connect(this, SIGNAL(signalToggleStopButton(void)),
          d_stop_button, SLOT(toggle(void)));
}

FreqControlPanel::~FreqControlPanel()
{
  removeWidget(d_axes_box);
  removeWidget(d_trace_box);
  removeWidget(d_fft_box);
  removeWidget(d_trigger_box);
  removeWidget(d_extras_box);
  delete d_axes_box;
  delete d_trace_box;
  delete d_fft_box;
  delete d_trigger_box;
  delete d_extras_box;

  // All other children of the boxes are automatically deleted.
}

void
FreqControlPanel::toggleGrid(bool en)
{
  d_grid_check->setChecked(en);
}

void
FreqControlPanel::toggleAxisLabels(bool en)
{
  d_axislabels_check->setChecked(en);
}

void
FreqControlPanel::toggleMaxHold(bool en)
{
  d_maxhold_check->setChecked(en);
}

void
FreqControlPanel::toggleMinHold(bool en)
{
  d_minhold_check->setChecked(en);
}

void
FreqControlPanel::notifyAvgSlider(int val)
{
  float fval = static_cast<float>(val) / (d_slider_max - d_slider_min + 1);
  emit signalAvgSlider(fval);
  emit signalAvg(true);
}

void
FreqControlPanel::setFFTAverage(float val)
{
  int slider_val = static_cast<int>(roundf(val * (d_slider_max - d_slider_min + 1)));
  if (slider_val > d_slider_max)
    slider_val = d_slider_max;
  else if (slider_val < d_slider_min)
    slider_val = d_slider_min;
  d_avg_slider->setValue(slider_val);
}

void
FreqControlPanel::toggleFFTSize(int val)
{
  int index = static_cast<int>(round(logf(static_cast<float>(val))/logf(2.0f))) - 5;
  d_fft_size_combo->setCurrentIndex(index);
}

void
FreqControlPanel::toggleFFTWindow(const gr::filter::firdes::win_type win)
{
  if(win == -1)
    d_fft_win_combo->setCurrentIndex(0);
  if(win == gr::filter::firdes::WIN_HAMMING)
    d_fft_win_combo->setCurrentIndex(1);
  else if(win == gr::filter::firdes::WIN_HANN)
    d_fft_win_combo->setCurrentIndex(2);
  else if(win == gr::filter::firdes::WIN_BLACKMAN)
    d_fft_win_combo->setCurrentIndex(3);
  else if(win == gr::filter::firdes::WIN_BLACKMAN_hARRIS)
    d_fft_win_combo->setCurrentIndex(4);
  else if(win == gr::filter::firdes::WIN_RECTANGULAR)
    d_fft_win_combo->setCurrentIndex(5);
  else if(win == gr::filter::firdes::WIN_KAISER)
    d_fft_win_combo->setCurrentIndex(6);
  else if(win == gr::filter::firdes::WIN_FLATTOP)
    d_fft_win_combo->setCurrentIndex(7);
}

void
FreqControlPanel::toggleTriggerMode(gr::qtgui::trigger_mode mode)
{
  d_trigger_mode_combo->setCurrentIndex(static_cast<int>(mode));
}

void
FreqControlPanel::toggleStopButton()
{
  emit signalToggleStopButton();
}

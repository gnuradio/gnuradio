/* -*- c++ -*- */
/*
 * Copyright 2008-2012 Free Software Foundation, Inc.
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

#ifndef WATERFALL_DISPLAY_PLOT_H
#define WATERFALL_DISPLAY_PLOT_H

#include <stdint.h>
#include <cstdio>
#include <vector>
#include <qwt_plot_spectrogram.h>
#include "DisplayPlot.h"
#include "waterfallGlobalData.h"
#include <gruel/high_res_timer.h>

#if QWT_VERSION < 0x060000
#include <plot_waterfall.h>
#else
#include <qwt_compat.h>
#endif

class WaterfallDisplayPlot: public DisplayPlot
{
  Q_OBJECT

public:
  WaterfallDisplayPlot(int nplots, QWidget*);
  virtual ~WaterfallDisplayPlot();

  void Reset();

  void SetFrequencyRange(const double, const double,
			 const double, const bool,
			 const double units=1000.0,
			 const std::string &strunits = "kHz");
  double GetStartFrequency() const;
  double GetStopFrequency() const;

  void PlotNewData(const std::vector<double*> dataPoints,
		   const int64_t numDataPoints,
		   const double timePerFFT,
		   const gruel::high_res_timer_type timestamp,
		   const int droppedFrames);

  // to be removed
  void PlotNewData(const double* dataPoints,
		   const int64_t numDataPoints,
		   const double timePerFFT,
		   const gruel::high_res_timer_type timestamp,
		   const int droppedFrames);

  void SetIntensityRange(const double minIntensity, const double maxIntensity);

  void replot(void);

  int GetIntensityColorMapType(int) const;
  void SetIntensityColorMapType(const int, const int, const QColor, const QColor);
  const QColor GetUserDefinedLowIntensityColor() const;
  const QColor GetUserDefinedHighIntensityColor() const;

signals:
  void UpdatedLowerIntensityLevel(const double);
  void UpdatedUpperIntensityLevel(const double);

private:
  void _UpdateIntensityRangeDisplay();

  double _startFrequency;
  double _stopFrequency;
  int    _xAxisMultiplier;

  std::vector<WaterfallData*> d_data;

#if QWT_VERSION < 0x060000
  std::vector<PlotWaterfall*> d_spectrogram;
#else
  std::vector<QwtPlotSpectrogram*> d_spectrogram;
#endif

  gruel::high_res_timer_type _lastReplot;

  bool _useCenterFrequencyFlag;

  std::vector<int> _intensityColorMapType;
  QColor _userDefinedLowIntensityColor;
  QColor _userDefinedHighIntensityColor;
};

#endif /* WATERFALL_DISPLAY_PLOT_H */

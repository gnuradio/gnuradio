/* -*- c++ -*- */
/*
 * Copyright 2008,2009,2010,2011 Free Software Foundation, Inc.
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

#ifndef FREQUENCY_DISPLAY_PLOT_HPP
#define FREQUENCY_DISPLAY_PLOT_HPP

#include <stdint.h>
#include <cstdio>
#include <vector>
#include "DisplayPlot.h"

class FrequencyDisplayPlot: public DisplayPlot
{
  Q_OBJECT

public:
  FrequencyDisplayPlot(int nplots, QWidget*);
  virtual ~FrequencyDisplayPlot();

  void SetFrequencyRange(const double, const double,
			 const double, const bool,
			 const double units=1000.0,
			 const std::string &strunits = "kHz");
  double GetStartFrequency()const;
  double GetStopFrequency()const;

  void PlotNewData(const std::vector<double*> dataPoints,
		   const int64_t numDataPoints,
		   const double noiseFloorAmplitude, const double peakFrequency,
		   const double peakAmplitude, const double timeInterval);

  // Old method to be removed
  void PlotNewData(const double* dataPoints,
		   const int64_t numDataPoints,
		   const double noiseFloorAmplitude, const double peakFrequency,
		   const double peakAmplitude, const double timeInterval);

  void ClearMaxData();
  void ClearMinData();

  void SetMaxFFTVisible(const bool);
  void SetMinFFTVisible(const bool);

  void replot();

  void set_yaxis(double min, double max);

  void SetTraceColour (QColor);
  void SetBGColour (QColor c);
  void ShowCFMarker (const bool);

public slots:
  void SetLowerIntensityLevel(const double);
  void SetUpperIntensityLevel(const double);

  void OnPickerPointSelected(const QwtDoublePoint & p);
  void OnPickerPointSelected6(const QPointF & p);

private:
  void _resetXAxisPoints();

  std::vector<double*> _dataPoints;

  QwtPlotCurve* _min_fft_plot_curve;
  QwtPlotCurve* _max_fft_plot_curve;

  double _startFrequency;
  double _stopFrequency;
  double _maxYAxis;
  double _minYAxis;

  QwtPlotMarker* _lower_intensity_marker;
  QwtPlotMarker* _upper_intensity_marker;

  QwtPlotMarker *_markerPeakAmplitude;
  QwtPlotMarker *_markerNoiseFloorAmplitude;
  QwtPlotMarker *_markerCF;

  double* _xAxisPoints;
  int     _xAxisMultiplier;

  double* _minFFTPoints;
  double* _maxFFTPoints;

  double _peakFrequency;
  double _peakAmplitude;

  double _noiseFloorAmplitude;

  bool _useCenterFrequencyFlag;
};

#endif /* FREQUENCY_DISPLAY_PLOT_HPP */

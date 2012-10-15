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

  Q_PROPERTY ( QColor min_fft_color READ GetMinFFTColor WRITE SetMinFFTColor )
  Q_PROPERTY ( QColor max_fft_color READ GetMaxFFTColor WRITE SetMaxFFTColor )
  Q_PROPERTY ( bool min_fft_visible READ GetMinFFTVisible WRITE SetMinFFTVisible )
  Q_PROPERTY ( bool max_fft_visible READ GetMaxFFTVisible WRITE SetMaxFFTVisible )
  Q_PROPERTY ( QColor marker_lower_intensity_color READ GetMarkerLowerIntensityColor WRITE SetMarkerLowerIntensityColor )
  Q_PROPERTY ( bool marker_lower_intensity_visible READ GetMarkerLowerIntensityVisible WRITE SetMarkerLowerIntensityVisible )
  Q_PROPERTY ( QColor marker_upper_intensity_color READ GetMarkerUpperIntensityColor WRITE SetMarkerUpperIntensityColor )
  Q_PROPERTY ( bool marker_upper_intensity_visible READ GetMarkerUpperIntensityVisible WRITE SetMarkerUpperIntensityVisible )
  Q_PROPERTY ( QColor marker_peak_amplitude_color READ GetMarkerPeakAmplitudeColor WRITE SetMarkerPeakAmplitudeColor )
  Q_PROPERTY ( QColor marker_noise_floor_amplitude_color READ GetMarkerNoiseFloorAmplitudeColor WRITE SetMarkerNoiseFloorAmplitudeColor )
  Q_PROPERTY ( bool marker_noise_floor_amplitude_visible READ GetMarkerNoiseFloorAmplitudeVisible WRITE SetMarkerNoiseFloorAmplitudeVisible )
  Q_PROPERTY ( QColor marker_CF_color READ GetMarkerCFColor WRITE SetMarkerCFColor )

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

  void replot();

  void set_yaxis(double min, double max);

  void SetTraceColour (QColor);
  void SetBGColour (QColor c);
  void ShowCFMarker (const bool);

  const bool GetMaxFFTVisible() const;
  const bool GetMinFFTVisible() const;
  const QColor GetMinFFTColor() const;
  const QColor GetMaxFFTColor() const;
  const QColor GetMarkerLowerIntensityColor () const;
  const bool GetMarkerLowerIntensityVisible () const;
  const QColor GetMarkerUpperIntensityColor () const;
  const bool GetMarkerUpperIntensityVisible () const;
  const QColor GetMarkerPeakAmplitudeColor () const;
  const bool GetMarkerNoiseFloorAmplitudeVisible () const;
  const QColor GetMarkerNoiseFloorAmplitudeColor () const;
  const QColor GetMarkerCFColor () const;

public slots:
  void SetMaxFFTVisible(const bool);
  void SetMinFFTVisible(const bool);
  void SetMinFFTColor (QColor c);
  void SetMaxFFTColor (QColor c);
  void SetMarkerLowerIntensityColor (QColor c);
  void SetMarkerLowerIntensityVisible (bool visible);
  void SetMarkerUpperIntensityColor (QColor c);
  void SetMarkerUpperIntensityVisible (bool visible);
  void SetMarkerPeakAmplitudeColor (QColor c);
  void SetMarkerNoiseFloorAmplitudeVisible (bool visible);
  void SetMarkerNoiseFloorAmplitudeColor (QColor c);
  void SetMarkerCFColor (QColor c);

  void SetLowerIntensityLevel(const double);
  void SetUpperIntensityLevel(const double);

  void OnPickerPointSelected(const QwtDoublePoint & p);
  void OnPickerPointSelected6(const QPointF & p);

private:
  void _resetXAxisPoints();

  std::vector<double*> _dataPoints;

  QwtPlotCurve* _min_fft_plot_curve;
  QwtPlotCurve* _max_fft_plot_curve;
  QColor _min_fft_color;
  bool _min_fft_visible;
  QColor _max_fft_color;
  bool _max_fft_visible;
  QColor _marker_lower_intensity_color;
  bool _marker_lower_intensity_visible;
  QColor _marker_upper_intensity_color;
  bool _marker_upper_intensity_visible;
  QColor _marker_peak_amplitude_color;
  QColor _marker_noise_floor_amplitude_color;
  bool _marker_noise_floor_amplitude_visible;
  QColor _marker_CF_color;

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

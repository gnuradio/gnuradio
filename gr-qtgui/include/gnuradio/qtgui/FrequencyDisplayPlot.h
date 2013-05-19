/* -*- c++ -*- */
/*
 * Copyright 2008-2011 Free Software Foundation, Inc.
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
#include <gnuradio/qtgui/DisplayPlot.h>

/*!
 * \brief QWidget for displaying frequency domain (PSD) plots.
 * \ingroup qtgui_blk
 */
class FrequencyDisplayPlot: public DisplayPlot
{
  Q_OBJECT

  Q_PROPERTY ( QColor min_fft_color READ getMinFFTColor WRITE setMinFFTColor )
  Q_PROPERTY ( QColor max_fft_color READ getMaxFFTColor WRITE setMaxFFTColor )
  Q_PROPERTY ( bool min_fft_visible READ getMinFFTVisible WRITE setMinFFTVisible )
  Q_PROPERTY ( bool max_fft_visible READ getMaxFFTVisible WRITE setMaxFFTVisible )
  Q_PROPERTY ( QColor marker_lower_intensity_color READ getMarkerLowerIntensityColor WRITE setMarkerLowerIntensityColor )
  Q_PROPERTY ( bool marker_lower_intensity_visible READ getMarkerLowerIntensityVisible WRITE setMarkerLowerIntensityVisible )
  Q_PROPERTY ( QColor marker_upper_intensity_color READ getMarkerUpperIntensityColor WRITE setMarkerUpperIntensityColor )
  Q_PROPERTY ( bool marker_upper_intensity_visible READ getMarkerUpperIntensityVisible WRITE setMarkerUpperIntensityVisible )
  Q_PROPERTY ( QColor marker_peak_amplitude_color READ getMarkerPeakAmplitudeColor WRITE setMarkerPeakAmplitudeColor )
  Q_PROPERTY ( QColor marker_noise_floor_amplitude_color READ getMarkerNoiseFloorAmplitudeColor WRITE setMarkerNoiseFloorAmplitudeColor )
  Q_PROPERTY ( bool marker_noise_floor_amplitude_visible READ getMarkerNoiseFloorAmplitudeVisible WRITE setMarkerNoiseFloorAmplitudeVisible )
  Q_PROPERTY ( QColor marker_CF_color READ getMarkerCFColor WRITE setMarkerCFColor )

public:
  FrequencyDisplayPlot(int nplots, QWidget*);
  virtual ~FrequencyDisplayPlot();

  void setFrequencyRange(const double, const double,
			 const double units=1000.0,
			 const std::string &strunits = "kHz");
  double getStartFrequency()const;
  double getStopFrequency()const;

  void plotNewData(const std::vector<double*> dataPoints,
		   const int64_t numDataPoints,
		   const double noiseFloorAmplitude, const double peakFrequency,
		   const double peakAmplitude, const double timeInterval);

  // Old method to be removed
  void plotNewData(const double* dataPoints,
		   const int64_t numDataPoints,
		   const double noiseFloorAmplitude, const double peakFrequency,
		   const double peakAmplitude, const double timeInterval);

  void clearMaxData();
  void clearMinData();

  void replot();

  void setYaxis(double min, double max);

  void setTraceColour (QColor);
  void setBGColour (QColor c);
  void showCFMarker (const bool);

  const bool getMaxFFTVisible() const;
  const bool getMinFFTVisible() const;
  const QColor getMinFFTColor() const;
  const QColor getMaxFFTColor() const;
  const QColor getMarkerLowerIntensityColor () const;
  const bool getMarkerLowerIntensityVisible () const;
  const QColor getMarkerUpperIntensityColor () const;
  const bool getMarkerUpperIntensityVisible () const;
  const QColor getMarkerPeakAmplitudeColor () const;
  const bool getMarkerNoiseFloorAmplitudeVisible () const;
  const QColor getMarkerNoiseFloorAmplitudeColor () const;
  const QColor getMarkerCFColor () const;

public slots:
  void setMaxFFTVisible(const bool);
  void setMinFFTVisible(const bool);
  void setMinFFTColor (QColor c);
  void setMaxFFTColor (QColor c);
  void setMarkerLowerIntensityColor (QColor c);
  void setMarkerLowerIntensityVisible (bool visible);
  void setMarkerUpperIntensityColor (QColor c);
  void setMarkerUpperIntensityVisible (bool visible);
  void setMarkerPeakAmplitudeColor (QColor c);
  void setMarkerNoiseFloorAmplitudeVisible (bool visible);
  void setMarkerNoiseFloorAmplitudeColor (QColor c);
  void setMarkerCFColor (QColor c);

  void setLowerIntensityLevel(const double);
  void setUpperIntensityLevel(const double);

  void onPickerPointSelected(const QwtDoublePoint & p);
  void onPickerPointSelected6(const QPointF & p);

  void setAutoScale(bool state);

private:
  void _resetXAxisPoints();
  void _autoScale(double bottom, double top);

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
};

#endif /* FREQUENCY_DISPLAY_PLOT_HPP */

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
#include <qwt_plot.h>
#include <qwt_painter.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_engine.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_magnifier.h>
#include <gruel/high_res_timer.h>
#include <qwt_symbol.h>
#include <qtgui_util.h>

#if QWT_VERSION >= 0x060000
#include <qwt_compat.h>
#endif

class FrequencyDisplayPlot:public QwtPlot{
  Q_OBJECT

public:
  FrequencyDisplayPlot(QWidget*);
  virtual ~FrequencyDisplayPlot();

  void SetFrequencyRange(const double, const double, 
			 const double, const bool,
			 const double units=1000.0, 
			 const std::string &strunits = "kHz");
  double GetStartFrequency()const;
  double GetStopFrequency()const;

  void PlotNewData(const double* dataPoints, const int64_t numDataPoints,
		   const double noiseFloorAmplitude, const double peakFrequency,
		   const double peakAmplitude, const double timeInterval);
  
  void ClearMaxData();
  void ClearMinData();
  
  void SetMaxFFTVisible(const bool);
  void SetMinFFTVisible(const bool);
				   
  virtual void replot();

  void set_yaxis(double min, double max);

  void SetTraceColour (QColor);
  void SetBGColour (QColor c);
  void ShowCFMarker (const bool);

public slots:
  void resizeSlot( QSize *e );
  void SetLowerIntensityLevel(const double);
  void SetUpperIntensityLevel(const double);

  // Because of the preprocessing of slots in QT, these are no
  // easily separated by the version check. Make one for each
  // version until it's worked out.
  void OnPickerPointSelected(const QwtDoublePoint & p);
  void OnPickerPointSelected6(const QPointF & p);

signals:
  void plotPointSelected(const QPointF p);

protected:

private:

  void _resetXAxisPoints();
  
  double _startFrequency;
  double _stopFrequency;
  double _maxYAxis;
  double _minYAxis;
  
  QwtPlotCurve* _fft_plot_curve;
  QwtPlotCurve* _min_fft_plot_curve;
  QwtPlotCurve* _max_fft_plot_curve;

  QwtPlotMarker* _lower_intensity_marker;
  QwtPlotMarker* _upper_intensity_marker;

  QwtPlotPanner* _panner;
  QwtPlotZoomer* _zoomer;

  QwtPlotMarker *_markerPeakAmplitude;
  QwtPlotMarker *_markerNoiseFloorAmplitude;
  QwtPlotMarker *_markerCF;

  QwtDblClickPlotPicker *_picker;
  
  QwtPlotMagnifier *_magnifier;
  
  double* _dataPoints;
  double* _xAxisPoints;
  int     _xAxisMultiplier;

  double* _minFFTPoints;
  double* _maxFFTPoints;
  int64_t _numPoints;

  double _peakFrequency;
  double _peakAmplitude;

  double _noiseFloorAmplitude;

  gruel::high_res_timer_type _lastReplot;

  bool _useCenterFrequencyFlag;
};

#endif /* FREQUENCY_DISPLAY_PLOT_HPP */

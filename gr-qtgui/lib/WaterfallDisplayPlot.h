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

#ifndef WATERFALL_DISPLAY_PLOT_HPP
#define WATERFALL_DISPLAY_PLOT_HPP

#include <stdint.h>
#include <cstdio>
#include <qwt_plot.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qtgui_util.h>
#include <waterfallGlobalData.h>

#include <gruel/high_res_timer.h>

#if QWT_VERSION < 0x060000
#include <plot_waterfall.h>
#else
#include <qwt_compat.h>
#endif

class WaterfallDisplayPlot:public QwtPlot{
  Q_OBJECT

public:
  WaterfallDisplayPlot(QWidget*);
  virtual ~WaterfallDisplayPlot();

  void Reset();

  void SetFrequencyRange(const double, const double, 
			 const double, const bool,
			 const double units=1000.0, 
			 const std::string &strunits = "kHz");
  double GetStartFrequency()const;
  double GetStopFrequency()const;

  void PlotNewData(const double* dataPoints, const int64_t numDataPoints,
		   const double timePerFFT, const gruel::high_res_timer_type timestamp,
		   const int droppedFrames);

  void SetIntensityRange(const double minIntensity, const double maxIntensity);

  virtual void replot(void);

  int GetIntensityColorMapType()const;
  void SetIntensityColorMapType( const int, const QColor, const QColor );
  const QColor GetUserDefinedLowIntensityColor()const;
  const QColor GetUserDefinedHighIntensityColor()const;

  enum{
    INTENSITY_COLOR_MAP_TYPE_MULTI_COLOR = 0,
    INTENSITY_COLOR_MAP_TYPE_WHITE_HOT = 1,
    INTENSITY_COLOR_MAP_TYPE_BLACK_HOT = 2,
    INTENSITY_COLOR_MAP_TYPE_INCANDESCENT = 3,
    INTENSITY_COLOR_MAP_TYPE_USER_DEFINED = 4
  };

public slots:
  void resizeSlot( QSize *s );

  // Because of the preprocessing of slots in QT, these are no
  // easily separated by the version check. Make one for each
  // version until it's worked out.
  void OnPickerPointSelected(const QwtDoublePoint & p);
  void OnPickerPointSelected6(const QPointF & p);
 
signals:
  void UpdatedLowerIntensityLevel(const double);
  void UpdatedUpperIntensityLevel(const double);
  void plotPointSelected(const QPointF p);

protected:

private:
  void _UpdateIntensityRangeDisplay();

  double _startFrequency;
  double _stopFrequency;
  int    _xAxisMultiplier;

  QwtPlotPanner* _panner;
  QwtPlotZoomer* _zoomer;

  QwtDblClickPlotPicker *_picker;

  WaterfallData *d_data;

#if QWT_VERSION < 0x060000
  PlotWaterfall *d_spectrogram;
#else
  QwtPlotSpectrogram *d_spectrogram;
#endif

  gruel::high_res_timer_type _lastReplot;

  bool _useCenterFrequencyFlag;

  int64_t _numPoints;

  int _intensityColorMapType;
  QColor _userDefinedLowIntensityColor;
  QColor _userDefinedHighIntensityColor;
};

#endif /* WATERFALL_DISPLAY_PLOT_HPP */

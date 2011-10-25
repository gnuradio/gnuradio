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

#ifndef TIME_DOMAIN_DISPLAY_PLOT_HPP
#define TIME_DOMAIN_DISPLAY_PLOT_HPP

#include <stdint.h>
#include <cstdio>
#include <vector>
#include <qwt_plot.h>
#include <qwt_painter.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_engine.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_marker.h>
#include <qwt_symbol.h>
#include <qtgui_util.h>

#if QWT_VERSION >= 0x060000
#include <qwt_compat.h>
#endif

class TimeDomainDisplayPlot:public QwtPlot{
  Q_OBJECT

public:
  TimeDomainDisplayPlot(int nplots, QWidget*);
  virtual ~TimeDomainDisplayPlot();

  void PlotNewData(const std::vector<double*> dataPoints, 
		   const int64_t numDataPoints, const double timeInterval);
    
  virtual void replot();

public slots:
  void setYaxis(double min, double max);
  void setXaxis(double min, double max);
  void setTitle(int which, QString title);
  void setColor(int which, QString color);

  void resizeSlot( QSize *s );
  void SetSampleRate(double sr, double units, 
		     const std::string &strunits);

  // Because of the preprocessing of slots in QT, these are no
  // easily separated by the version check. Make one for each
  // version until it's worked out.
  void OnPickerPointSelected(const QwtDoublePoint & p);
  void OnPickerPointSelected6(const QPointF & p);

signals:
  void plotPointSelected(const QPointF p);

protected slots:
  void LegendEntryChecked(QwtPlotItem *plotItem, bool on);

protected:

private:
  void _resetXAxisPoints();

  int _nplots;
  std::vector<QwtPlotCurve*> _plot_curve;

  QwtPlotPanner* _panner;
  QwtPlotZoomer* _zoomer;
  
  QwtDblClickPlotPicker *_picker;
  QwtPlotMagnifier *_magnifier;
  
  std::vector<double*> _dataPoints;
  double* _xAxisPoints;

  double _sampleRate;

  int64_t _numPoints;
};

#endif /* TIME_DOMAIN_DISPLAY_PLOT_HPP */

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

#ifndef CONSTELLATION_DISPLAY_PLOT_HPP
#define CONSTELLATION_DISPLAY_PLOT_HPP

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
#include <gruel/high_res_timer.h>
#include <qwt_symbol.h>
#include <qtgui_util.h>

#if QWT_VERSION >= 0x060000
#include <qwt_point_3d.h>  // doesn't seem necessary, but is...
#include <qwt_compat.h>
#endif

class ConstellationDisplayPlot : public QwtPlot
{
  Q_OBJECT

public:
  ConstellationDisplayPlot(QWidget*);
  virtual ~ConstellationDisplayPlot();

  void PlotNewData(const double* realDataPoints, 
		   const double* imagDataPoints, 
		   const int64_t numDataPoints,
		   const double timeInterval);
    
  virtual void replot();

  void set_xaxis(double min, double max);
  void set_yaxis(double min, double max);
  void set_axis(double xmin, double xmax,
		double ymin, double ymax);
  void set_pen_size(int size);

public slots:
  void resizeSlot( QSize *s );

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
  QwtPlotCurve* _plot_curve;

  QwtPlotPanner* _panner;
  QwtPlotZoomer* _zoomer;
  
  QwtDblClickPlotPicker *_picker;

  double* _realDataPoints;
  double* _imagDataPoints;

  gruel::high_res_timer_type _lastReplot;

  int64_t _numPoints;
  int64_t _penSize;
};

#endif /* CONSTELLATION_DISPLAY_PLOT_HPP */

/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
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

#ifndef HISTOGRAM_DISPLAY_PLOT_H
#define HISTOGRAM_DISPLAY_PLOT_H

#include <stdint.h>
#include <cstdio>
#include <vector>
#include <gnuradio/qtgui/DisplayPlot.h>

/*!
 * \brief QWidget for displaying time domain plots.
 * \ingroup qtgui_blk
 */
class HistogramDisplayPlot: public DisplayPlot
{
  Q_OBJECT

public:
  HistogramDisplayPlot(int nplots, QWidget*);
  virtual ~HistogramDisplayPlot();

  void plotNewData(const std::vector<double*> dataPoints,
		   const int64_t numDataPoints, const double timeInterval);

  void replot();

public slots:
  void setAutoScale(bool state);
  void setSemilogx(bool en);
  void setSemilogy(bool en);
  void setAccumulate(bool en);

  void setMarkerAlpha(int which, int alpha);
  int getMarkerAlpha(int which) const;
  void setLineColor(int which, QColor color);

  void setNumBins(int bins);

private:
  void _resetXAxisPoints(double bottom, double top);
  void _autoScale(double bottom, double top);

  double* _xAxisPoints;
  std::vector<double*> _yDataPoints;

  int _bins;
  bool _accum;

  bool d_semilogx;
  bool d_semilogy;
};

#endif /* HISTOGRAM_DISPLAY_PLOT_H */

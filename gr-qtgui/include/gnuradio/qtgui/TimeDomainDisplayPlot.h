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

#ifndef TIME_DOMAIN_DISPLAY_PLOT_H
#define TIME_DOMAIN_DISPLAY_PLOT_H

#include <stdint.h>
#include <cstdio>
#include <vector>
#include <gnuradio/qtgui/DisplayPlot.h>

/*!
 * \brief QWidget for displaying time domain plots.
 * \ingroup qtgui_blk
 */
class TimeDomainDisplayPlot: public DisplayPlot
{
  Q_OBJECT

public:
  TimeDomainDisplayPlot(int nplots, QWidget*);
  virtual ~TimeDomainDisplayPlot();

  void plotNewData(const std::vector<double*> dataPoints,
		   const int64_t numDataPoints, const double timeInterval);

  void replot();

  void stemPlot(bool en);

public slots:
  void setSampleRate(double sr, double units,
		     const std::string &strunits);

  void setAutoScale(bool state);
  void setSemilogx(bool en);
  void setSemilogy(bool en);

private:
  void _resetXAxisPoints();
  void _autoScale(double bottom, double top);

  std::vector<double*> _dataPoints;
  double* _xAxisPoints;

  double _sampleRate;

  bool d_semilogx;
  bool d_semilogy;
};

#endif /* TIME_DOMAIN_DISPLAY_PLOT_H */

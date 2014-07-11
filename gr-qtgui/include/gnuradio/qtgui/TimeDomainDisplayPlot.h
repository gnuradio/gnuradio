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
#include <gnuradio/tags.h>

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
		   const int64_t numDataPoints, const double timeInterval,
                   const std::vector< std::vector<gr::tag_t> > &tags \
                   = std::vector< std::vector<gr::tag_t> >());

  void replot();

  void stemPlot(bool en);

public slots:
  void setSampleRate(double sr, double units,
		     const std::string &strunits);

  void setAutoScale(bool state);
  void setSemilogx(bool en);
  void setSemilogy(bool en);

  void legendEntryChecked(QwtPlotItem *plotItem, bool on);
  void legendEntryChecked(const QVariant &plotItem, bool on, int index);

  void enableTagMarker(int which, bool en);

  void setYLabel(const std::string &label,
                 const std::string &unit="");

private:
  void _resetXAxisPoints();
  void _autoScale(double bottom, double top);

  std::vector<double*> d_ydata;
  double* d_xdata;

  double d_sample_rate;

  bool d_semilogx;
  bool d_semilogy;

  std::vector< std::vector<QwtPlotMarker*> > d_tag_markers;
  std::vector<bool> d_tag_markers_en;
};

#endif /* TIME_DOMAIN_DISPLAY_PLOT_H */

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

  Q_PROPERTY ( QColor tag_text_color READ getTagTextColor WRITE setTagTextColor )
  Q_PROPERTY ( QColor tag_background_color READ getTagBackgroundColor WRITE setTagBackgroundColor )
  Q_PROPERTY ( Qt::BrushStyle tag_background_style READ getTagBackgroundStyle WRITE setTagBackgroundStyle )

public:
  TimeDomainDisplayPlot(int nplots, QWidget*);
  virtual ~TimeDomainDisplayPlot();

  void plotNewData(const std::vector<double*> dataPoints,
		   const int64_t numDataPoints, const double timeInterval,
                   const std::vector< std::vector<gr::tag_t> > &tags \
                   = std::vector< std::vector<gr::tag_t> >());

  void replot();

  void stemPlot(bool en);

  double sampleRate() const;

  const QColor getTagTextColor();
  const QColor getTagBackgroundColor();
  const Qt::BrushStyle getTagBackgroundStyle();

public slots:
  void setSampleRate(double sr, double units,
		     const std::string &strunits);

  void setAutoScale(bool state);
  void setAutoScaleShot();
  void setSemilogx(bool en);
  void setSemilogy(bool en);

  void legendEntryChecked(QwtPlotItem *plotItem, bool on);
  void legendEntryChecked(const QVariant &plotItem, bool on, int index);

  void enableTagMarker(int which, bool en);

  void setYLabel(const std::string &label,
                 const std::string &unit="");

  void attachTriggerLines(bool en);
  void setTriggerLines(double x, double y);

  void setTagTextColor(QColor c);
  void setTagBackgroundColor(QColor c);
  void setTagBackgroundStyle(Qt::BrushStyle b);

private:
  void _resetXAxisPoints();
  void _autoScale(double bottom, double top);

  std::vector<double*> d_ydata;
  double* d_xdata;

  double d_sample_rate;

  bool d_semilogx;
  bool d_semilogy;
  bool d_autoscale_shot;

  std::vector< std::vector<QwtPlotMarker*> > d_tag_markers;
  std::vector<bool> d_tag_markers_en;

  QColor d_tag_text_color;
  QColor d_tag_background_color;
  Qt::BrushStyle d_tag_background_style;

  QwtPlotMarker *d_trigger_lines[2];
};

#endif /* TIME_DOMAIN_DISPLAY_PLOT_H */

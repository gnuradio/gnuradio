/* -*- c++ -*- */
/*
 * Copyright 2012,2013 Free Software Foundation, Inc.
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

#ifndef TIMERASTER_DISPLAY_PLOT_H
#define TIMERASTER_DISPLAY_PLOT_H

#include <stdint.h>
#include <cstdio>
#include <vector>
#include <qwt_plot_rasteritem.h>
#include <gnuradio/qtgui/DisplayPlot.h>
#include <gnuradio/qtgui/timeRasterGlobalData.h>
#include <gnuradio/qtgui/plot_raster.h>
#include <gnuradio/high_res_timer.h>

#if QWT_VERSION < 0x060000
#include <gnuradio/qtgui/plot_waterfall.h>
#else
#include <qwt_compat.h>
#endif

/*!
 * \brief QWidget for time raster (time vs. time) plots.
 * \ingroup qtgui_blk
 */
class TimeRasterDisplayPlot: public DisplayPlot
{
  Q_OBJECT

  Q_PROPERTY(int intensity_color_map_type1 READ getIntensityColorMapType1 WRITE setIntensityColorMapType1)
  Q_PROPERTY(int color_map_title_font_size READ getColorMapTitleFontSize WRITE setColorMapTitleFontSize)

public:
  TimeRasterDisplayPlot(int nplots,
			double samp_rate,
			double rows, double cols,
			QWidget*);
  virtual ~TimeRasterDisplayPlot();

  void reset();

  void setNumRows(double rows);
  void setNumCols(double cols);
  void setAlpha(int which, int alpha);
  void setSampleRate(double samprate);

  double numRows() const;
  double numCols() const;

  int getAlpha(int which);

  void setPlotDimensions(const double rows, const double cols,
			 const double units, const std::string &strunits);

  void plotNewData(const std::vector<double*> dataPoints,
		   const int64_t numDataPoints);

  void plotNewData(const double* dataPoints,
		   const int64_t numDataPoints);

  void setIntensityRange(const double minIntensity,
			 const double maxIntensity);

  void replot(void);

  int getIntensityColorMapType(int) const;
  int getIntensityColorMapType1() const;
  void setIntensityColorMapType(const int, const int, const QColor, const QColor);
  void setIntensityColorMapType1(int);
  int getColorMapTitleFontSize() const;
  void setColorMapTitleFontSize(int tfs);
  const QColor getUserDefinedLowIntensityColor() const;
  const QColor getUserDefinedHighIntensityColor() const;

  double getMinIntensity(int which) const;
  double getMaxIntensity(int which) const;

signals:
  void updatedLowerIntensityLevel(const double);
  void updatedUpperIntensityLevel(const double);

private:
  void _updateIntensityRangeDisplay();

  std::vector<TimeRasterData*> d_data;
  std::vector<PlotTimeRaster*> d_raster;

  double d_samp_rate;
  double d_rows, d_cols;

  std::vector<int> d_color_map_type;
  QColor d_low_intensity;
  QColor d_high_intensity;

  int d_color_bar_title_font_size;
};

#endif /* TIMERASTER_DISPLAY_PLOT_H */

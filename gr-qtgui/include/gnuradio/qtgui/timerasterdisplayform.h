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

#ifndef TIMERASTER_DISPLAY_FORM_H
#define TIMERASTER_DISPLAY_FORM_H

#include <gnuradio/qtgui/spectrumUpdateEvents.h>
#include <gnuradio/qtgui/TimeRasterDisplayPlot.h>
#include <QtGui/QtGui>
#include <vector>
#include <gnuradio/filter/firdes.h>

#include <gnuradio/qtgui/displayform.h>

/*!
 * \brief DisplayForm child for managing time raster plots.
 * \ingroup qtgui_blk
 */
class TimeRasterDisplayForm : public DisplayForm
{
  Q_OBJECT

  public:
  TimeRasterDisplayForm(int nplots=1,
			double samp_rate=1,
			double rows=1,
			double cols=1,
			double zmax=1,
			QWidget* parent = 0);
  ~TimeRasterDisplayForm();

  TimeRasterDisplayPlot* getPlot();

  double numRows();
  double numCols();

  int getColorMap(int which);
  int getAlpha(int which);
  double getMinIntensity(int which);
  double getMaxIntensity(int which);

public slots:
  void customEvent(QEvent *e);

  void setNumRows(double rows);
  void setNumCols(double cols);

  void setNumRows(QString rows);
  void setNumCols(QString cols);

  void setSampleRate(const double samprate);
  void setSampleRate(const QString &rate);

  void setIntensityRange(const double minIntensity,
			 const double maxIntensity);
  void setMaxIntensity(const QString &m);
  void setMinIntensity(const QString &m);

  void setColorMap(int which,
		   const int newType,
		   const QColor lowColor=QColor("white"),
		   const QColor highColor=QColor("white"));

  void setAlpha(int which, int alpha);

  void autoScale(bool en=false);

private slots:
  void newData(const QEvent *updateEvent);

private:
  double d_min_val, d_cur_min_val;
  double d_max_val, d_cur_max_val;
};

#endif /* TIMERASTER_DISPLAY_FORM_H */

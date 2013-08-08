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

#ifndef TIMERASTER_GLOBAL_DATA_HPP
#define TIMERASTER_GLOBAL_DATA_HPP

#include <qwt_raster_data.h>
#include <inttypes.h>

#if QWT_VERSION >= 0x060000
#include <qwt_point_3d.h>  // doesn't seem necessary, but is...
#include <qwt_compat.h>
#endif

class TimeRasterData: public QwtRasterData
{
public:
  TimeRasterData(const double rows, const double cols);
  virtual ~TimeRasterData();

  virtual void reset();
  virtual void copy(const TimeRasterData*);

  virtual void resizeData(const double rows, const double cols);

  virtual QwtRasterData *copy() const;

#if QWT_VERSION < 0x060000
  virtual QwtDoubleInterval range() const;
  virtual void setRange(const QwtDoubleInterval&);
#endif

  virtual double value(double x, double y) const;

  virtual double getNumCols()const;
  virtual double getNumRows()const;

  virtual void addData(const double*, const int);

  void incrementResidual();

protected:

  double* d_data;
  double d_rows, d_cols;
  double d_resid;
  int d_nitems, d_totalitems, d_data_size;

#if QWT_VERSION < 0x060000
  QwtDoubleInterval d_intensityRange;
#else
  QwtInterval d_intensityRange;
#endif

private:

};

#endif /* TIMERASTER_GLOBAL_DATA_HPP */

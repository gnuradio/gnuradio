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

#ifndef TIMERASTER_GLOBAL_DATA_CPP
#define TIMERASTER_GLOBAL_DATA_CPP

#include <gnuradio/qtgui/timeRasterGlobalData.h>
#include <cstdio>
#include <cmath>
#include <iostream>

TimeRasterData::TimeRasterData(const double rows, const double cols)
#if QWT_VERSION < 0x060000
  : QwtRasterData(QwtDoubleRect(0,  0, rows, cols))
#else
    : QwtRasterData()
#endif
{
  d_nitems = 0;
  d_rows = rows;
  d_cols = cols;
  d_resid = 0;

  // We add 1 here so we always have the next row already started
  // (helps when d_cols is fractional and we have to slide).
  d_totalitems = static_cast<int>((d_rows+1)*floor(d_cols));
  d_data_size = d_totalitems + static_cast<int>(floor(d_cols));

  d_intensityRange = QwtDoubleInterval(0.0, 10.0);
  d_data = new double[d_data_size];

#if QWT_VERSION >= 0x060000
  setInterval(Qt::XAxis, QwtInterval(0, cols));
  setInterval(Qt::YAxis, QwtInterval(0, rows));
  setInterval(Qt::ZAxis, QwtInterval(0.0, 10.0));
#endif

  reset();
}

TimeRasterData::~TimeRasterData()
{
  delete [] d_data;
}

void TimeRasterData::reset()
{
  d_resid = 0;
  d_nitems = 0;
  memset(d_data, 0x0, d_data_size*sizeof(double));
}

void TimeRasterData::copy(const TimeRasterData* rhs)
{
#if QWT_VERSION < 0x060000
  if((d_cols != rhs->getNumCols()) ||
     (boundingRect() != rhs->boundingRect()) ){
    d_cols = rhs->getNumCols();
    d_rows = rhs->getNumRows();
    d_totalitems = static_cast<int>((d_rows+1)*floor(d_cols));
    d_data_size = d_totalitems + static_cast<int>(floor(d_cols));
    setBoundingRect(rhs->boundingRect());
    delete [] d_data;
    d_data = new double[d_data_size];
  }
#else
  if((d_cols != rhs->getNumCols()) || (d_rows != rhs->getNumRows())) {
    d_cols = rhs->getNumCols();
    d_rows = rhs->getNumRows();
    d_totalitems = static_cast<int>((d_rows+1)*floor(d_cols));
    d_data_size = d_totalitems + static_cast<int>(floor(d_cols));
    delete [] d_data;
    d_data = new double[d_data_size];
  }
#endif

  reset();

#if QWT_VERSION < 0x060000
  setRange(rhs->range());
#else
  setInterval(Qt::XAxis, rhs->interval(Qt::XAxis));
  setInterval(Qt::YAxis, rhs->interval(Qt::YAxis));
  setInterval(Qt::ZAxis, rhs->interval(Qt::ZAxis));
#endif
}

void TimeRasterData::resizeData(const double rows, const double cols)
{
#if QWT_VERSION < 0x060000
  if((cols != getNumCols()) || (boundingRect().width() != cols) ||
     (rows != getNumRows()) || (boundingRect().height() != cols)) {
    setBoundingRect(QwtDoubleRect(0, 0, cols, rows));
    d_cols = cols;
    d_rows = rows;
    d_totalitems = static_cast<int>((d_rows+1)*floor(d_cols));
    d_data_size = d_totalitems + static_cast<int>(floor(d_cols));
    delete [] d_data;
    d_data = new double[d_data_size];
  }

#else
  if((cols != getNumCols()) || (interval(Qt::XAxis).width() != cols) ||
     (rows != getNumRows()) || (interval(Qt::YAxis).width() != rows)) {

    setInterval(Qt::XAxis, QwtInterval(0, cols));
    setInterval(Qt::YAxis, QwtInterval(0, rows));

    d_cols = cols;
    d_rows = rows;
    d_totalitems = static_cast<int>((d_rows+1)*floor(d_cols));
    d_data_size = d_totalitems + static_cast<int>(floor(d_cols));

    delete [] d_data;
    d_data = new double[d_data_size];
  }
#endif

  reset();
}

QwtRasterData *TimeRasterData::copy() const
{
#if QWT_VERSION < 0x060000
  TimeRasterData* returnData =  \
    new TimeRasterData(d_cols, d_rows);
#else
  TimeRasterData* returnData = \
    new TimeRasterData(d_cols, d_rows);
#endif

  returnData->copy(this);
  return returnData;
}


#if QWT_VERSION < 0x060000
QwtDoubleInterval TimeRasterData::range() const
{
  return d_intensityRange;
}

void TimeRasterData::setRange(const QwtDoubleInterval& newRange)
{
  d_intensityRange = newRange;
}

#endif


double
TimeRasterData::value(double x, double y) const
{
  double returnValue = 0.0;

#if QWT_VERSION < 0x060000
  double top = boundingRect().top();
  double bottom = top - boundingRect().height();
  double left = boundingRect().left();
  double right = left - boundingRect().width();
#else
  double top = interval(Qt::YAxis).maxValue();
  double bottom = interval(Qt::YAxis).minValue();
  double left = interval(Qt::XAxis).minValue();
  double right = interval(Qt::XAxis).maxValue();
#endif

  if((x < left) || (x > right) || (y < bottom) || (y > top))
    return 0.0;

  double _y = floor(top - y);
  double _loc = _y*(d_cols) + x + d_resid;
  int location = static_cast<int>(_loc);

  if((location > -1) && (location < d_data_size)) {
    returnValue = d_data[location];
  }

  return returnValue;
}

void
TimeRasterData::incrementResidual()
{
  // After a full set of rows are drawn, we want to add up the
  // residual due to any fractional value of d_cols to appropriately
  // shift the next data in.
  double icols = floor(d_cols);
  d_resid = fmod(d_resid + (d_cols - icols) * d_rows, icols);
}

double
TimeRasterData::getNumCols() const
{
  return d_cols;
}

double
TimeRasterData::getNumRows() const
{
  return d_rows;
}

void
TimeRasterData::addData(const double* data,
			const int dataSize)
{
  int cols = static_cast<int>(d_cols);
  if(dataSize == cols) {
    if(d_nitems < d_totalitems) {
      memcpy(&d_data[d_nitems], data, cols*sizeof(double));
      d_nitems += cols;
    }
    else {
      memcpy(&d_data[d_nitems], data, cols*sizeof(double));
      memmove(d_data, &d_data[cols], d_totalitems*sizeof(double));
    }
  }
}

#endif /* TIMERASTER_GLOBAL_DATA_CPP */

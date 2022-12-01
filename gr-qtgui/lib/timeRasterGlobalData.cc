/* -*- c++ -*- */
/*
 * Copyright 2012,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef TIMERASTER_GLOBAL_DATA_CPP
#define TIMERASTER_GLOBAL_DATA_CPP

#include <gnuradio/qtgui/timeRasterGlobalData.h>

#include <cmath>
#include <cstdio>

TimeRasterData::TimeRasterData(const double rows, const double cols) : QwtRasterData()
{
    d_nitems = 0;
    d_rows = rows;
    d_cols = cols;
    d_resid = 0;

    // We add 1 here so we always have the next row already started
    // (helps when d_cols is fractional and we have to slide).
    d_totalitems = static_cast<int>((d_rows + 1) * floor(d_cols));

    d_intensityRange = QwtInterval(0.0, 10.0);
    d_data.resize(d_totalitems + static_cast<int>(floor(d_cols)));

    setInterval(Qt::XAxis, QwtInterval(0, cols));
    setInterval(Qt::YAxis, QwtInterval(0, rows));
    setInterval(Qt::ZAxis, QwtInterval(0.0, 10.0));

    reset();
}

TimeRasterData::~TimeRasterData() {}

void TimeRasterData::reset()
{
    d_resid = 0;
    d_nitems = 0;
    std::fill(std::begin(d_data), std::end(d_data), 0.0);
}

void TimeRasterData::copy(const TimeRasterData* rhs)
{
    if ((d_cols != rhs->getNumCols()) || (d_rows != rhs->getNumRows())) {
        d_cols = rhs->getNumCols();
        d_rows = rhs->getNumRows();
        d_totalitems = static_cast<int>((d_rows + 1) * floor(d_cols));
        d_data.resize(d_totalitems + static_cast<int>(floor(d_cols)));
    }

    reset();

    setInterval(Qt::XAxis, rhs->interval(Qt::XAxis));
    setInterval(Qt::YAxis, rhs->interval(Qt::YAxis));
    setInterval(Qt::ZAxis, rhs->interval(Qt::ZAxis));
}

void TimeRasterData::resizeData(const double rows, const double cols)
{
    if ((cols != getNumCols()) || (interval(Qt::XAxis).width() != cols) ||
        (rows != getNumRows()) || (interval(Qt::YAxis).width() != rows)) {

        setInterval(Qt::XAxis, QwtInterval(0, cols));
        setInterval(Qt::YAxis, QwtInterval(0, rows));

        d_cols = cols;
        d_rows = rows;
        d_totalitems = static_cast<int>((d_rows + 1) * floor(d_cols));
        d_data.resize(d_totalitems + static_cast<int>(floor(d_cols)));
    }

    reset();
}

QwtRasterData* TimeRasterData::copy() const
{
    TimeRasterData* returnData = new TimeRasterData(d_cols, d_rows);

    returnData->copy(this);
    return returnData;
}


#if QWT_VERSION >= 0x060200
void TimeRasterData::setInterval(Qt::Axis axis, const QwtInterval& interval)
{
    d_intervals[axis] = interval;
}

QwtInterval TimeRasterData::interval(Qt::Axis a) const { return d_intervals[a]; }

#endif


double TimeRasterData::value(double x, double y) const
{
    double top = interval(Qt::YAxis).maxValue();
    double bottom = interval(Qt::YAxis).minValue();
    double left = interval(Qt::XAxis).minValue();
    double right = interval(Qt::XAxis).maxValue();

    if ((x < left) || (x > right) || (y < bottom) || (y > top))
        return 0.0;

    double _y = floor(top - y);
    double _loc = _y * (d_cols) + x + d_resid;
    const auto location = static_cast<size_t>(_loc);

    if (location < d_data.size()) {
        return d_data[location];
    }

    return 0.0;
}

void TimeRasterData::incrementResidual()
{
    // After a full set of rows are drawn, we want to add up the
    // residual due to any fractional value of d_cols to appropriately
    // shift the next data in.
    double icols = floor(d_cols);
    d_resid = fmod(d_resid + (d_cols - icols) * d_rows, icols);
}

double TimeRasterData::getNumCols() const { return d_cols; }

double TimeRasterData::getNumRows() const { return d_rows; }

void TimeRasterData::addData(const double* data, const int dataSize)
{
    int cols = static_cast<int>(d_cols);
    if (dataSize == cols) {
        if (d_nitems < d_totalitems) {
            memcpy(&d_data[d_nitems], data, cols * sizeof(double));
            d_nitems += cols;
        } else {
            memcpy(&d_data[d_nitems], data, cols * sizeof(double));
            memmove(d_data.data(), &d_data[cols], d_totalitems * sizeof(double));
        }
    }
}

#endif /* TIMERASTER_GLOBAL_DATA_CPP */

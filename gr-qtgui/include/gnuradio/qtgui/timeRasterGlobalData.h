/* -*- c++ -*- */
/*
 * Copyright 2012,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef TIMERASTER_GLOBAL_DATA_HPP
#define TIMERASTER_GLOBAL_DATA_HPP

#include <inttypes.h>
#include <qwt_raster_data.h>

#if QWT_VERSION >= 0x060000
#include <qwt_compat.h>
#include <qwt_point_3d.h> // doesn't seem necessary, but is...
#endif

class TimeRasterData : public QwtRasterData
{
public:
    TimeRasterData(const double rows, const double cols);
    virtual ~TimeRasterData();

    virtual void reset();
    virtual void copy(const TimeRasterData*);

    virtual void resizeData(const double rows, const double cols);

    virtual QwtRasterData* copy() const;

#if QWT_VERSION < 0x060000
    virtual QwtDoubleInterval range() const;
    virtual void setRange(const QwtDoubleInterval&);
#endif

    virtual double value(double x, double y) const;

    virtual double getNumCols() const;
    virtual double getNumRows() const;

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

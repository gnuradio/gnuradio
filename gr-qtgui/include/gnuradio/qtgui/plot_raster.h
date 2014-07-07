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

#ifndef PLOT_TIMERASTER_H
#define PLOT_TIMERASTER_H

#include <qglobal.h>
#include <gnuradio/qtgui/timeRasterGlobalData.h>
#include <qwt_plot_rasteritem.h>

#if QWT_VERSION >= 0x060000
#include <qwt_point_3d.h>  // doesn't seem necessary, but is...
#include <qwt_compat.h>
#endif

class QwtColorMap;

/*!
 * \brief A plot item, which displays a time raster.
 * \ingroup qtgui_blk
 *
 * \details
 * A time raster displays threedimenional data, where the 3rd dimension
 * (the intensity) is displayed using colors. The colors are calculated
 * from the values using a color map.
 *
 * \sa QwtRasterData, QwtColorMap
 */
class PlotTimeRaster: public QwtPlotRasterItem
{
public:
    explicit PlotTimeRaster(const QString &title = QString::null);
    virtual ~PlotTimeRaster();

    const TimeRasterData* data()const;

    void setData(TimeRasterData *data);

    void setColorMap(const QwtColorMap *map);

    const QwtColorMap &colorMap() const;

#if QWT_VERSION < 0x060000
    virtual QwtDoubleRect boundingRect() const;
    virtual QSize rasterHint(const QwtDoubleRect &) const;
    virtual QwtDoubleInterval interval(Qt::Axis ax) const;
#else
    virtual QwtInterval interval(Qt::Axis ax) const;
#endif

    virtual int rtti() const;

    virtual void draw(QPainter *p,
		      const QwtScaleMap &xMap,
		      const QwtScaleMap &yMap,
		      const QRect &rect) const;

protected:
#if QWT_VERSION < 0x060000
    QImage renderImage(const QwtScaleMap &xMap,
		       const QwtScaleMap &yMap,
		       const QwtDoubleRect &rect) const;
#else
    QImage renderImage(const QwtScaleMap &xMap,
		       const QwtScaleMap &yMap,
		       const QRectF &rect,
		       const QSize &size=QSize(0,0)) const;
#endif

private:
    class PrivateData;
    PrivateData *d_data;
};

#endif

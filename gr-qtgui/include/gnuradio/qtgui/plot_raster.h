/* -*- c++ -*- */
/*
 * Copyright 2012,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef PLOT_TIMERASTER_H
#define PLOT_TIMERASTER_H

#include <gnuradio/qtgui/timeRasterGlobalData.h>
#include <qglobal.h>
#include <qwt_plot_rasteritem.h>

#if QWT_VERSION >= 0x060000
#include <qwt_compat.h>
#include <qwt_point_3d.h> // doesn't seem necessary, but is...
#endif

class QwtColorMap;

/*!
 * \brief A plot item, which displays a time raster.
 * \ingroup qtgui_blk
 *
 * \details
 * A time raster displays three-dimensional data, where the 3rd dimension
 * (the intensity) is displayed using colors. The colors are calculated
 * from the values using a color map.
 *
 * \sa QwtRasterData, QwtColorMap
 */
class PlotTimeRaster : public QwtPlotRasterItem
{
public:
    explicit PlotTimeRaster(const QString& title = QString());
    ~PlotTimeRaster() override;

    const TimeRasterData* data() const;

    void setData(TimeRasterData* data);

    void setColorMap(const QwtColorMap* map);

    const QwtColorMap& colorMap() const;

#if QWT_VERSION < 0x060000
    virtual QwtDoubleRect boundingRect() const;
    virtual QSize rasterHint(const QwtDoubleRect&) const;
    virtual QwtDoubleInterval interval(Qt::Axis ax) const;
#else
    QwtInterval interval(Qt::Axis ax) const override;
#endif

    int rtti() const override;

protected:
#if QWT_VERSION < 0x060000
    QImage renderImage(const QwtScaleMap& xMap,
                       const QwtScaleMap& yMap,
                       const QwtDoubleRect& rect) const;
#else
    QImage renderImage(const QwtScaleMap& xMap,
                       const QwtScaleMap& yMap,
                       const QRectF& rect,
                       const QSize& size = QSize(0, 0)) const override;
#endif

private:
    class PrivateData;
    PrivateData* d_data;
};

#endif

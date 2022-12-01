/* -*- c++ -*- */
/*
 * Copyright 2008-2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef PLOT_WATERFALL_H
#define PLOT_WATERFALL_H

#include <gnuradio/qtgui/waterfallGlobalData.h>
#include <qglobal.h>
#include <qwt_plot_rasteritem.h>

#include <qsize.h>
#include <qwt_interval.h>

class QwtColorMap;

/*!
 * \brief A plot item, which displays a waterfall spectrogram
 * \ingroup qtgui_blk
 *
 * \details
 * A waterfall displays three-dimensional data, where the 3rd dimension
 * (the intensity) is displayed using colors. The colors are calculated
 * from the values using a color map.
 *
 * \sa QwtRasterData, QwtColorMap
 */
class PlotWaterfall : public QwtPlotRasterItem
{
public:
    explicit PlotWaterfall(WaterfallData* data, const QString& title = QString());
    ~PlotWaterfall() override;

    const WaterfallData* data() const;

    void setColorMap(const QwtColorMap&);

    const QwtColorMap& colorMap() const;

    int rtti() const override;

protected:
    QImage renderImage(const QwtScaleMap& xMap,
                       const QwtScaleMap& yMap,
                       const QRectF& rect,
                       const QSize& size = QSize(0, 0)) const override;

private:
    class PrivateData;
    PrivateData* d_data;
};

#endif

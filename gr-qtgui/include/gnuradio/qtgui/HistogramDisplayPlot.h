/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef HISTOGRAM_DISPLAY_PLOT_H
#define HISTOGRAM_DISPLAY_PLOT_H

#include <gnuradio/qtgui/DisplayPlot.h>
#include <stdint.h>
#include <cstdio>
#include <vector>

/*!
 * \brief QWidget for displaying time domain plots.
 * \ingroup qtgui_blk
 */
class HistogramDisplayPlot : public DisplayPlot
{
    Q_OBJECT

public:
    HistogramDisplayPlot(unsigned int nplots, QWidget*);
    virtual ~HistogramDisplayPlot();

    void plotNewData(const std::vector<double*> dataPoints,
                     const uint64_t numDataPoints,
                     const double timeInterval);

    void replot();

public slots:
    void setAutoScale(bool state);
    void setAutoScaleX();
    void setSemilogx(bool en);
    void setSemilogy(bool en);
    void setAccumulate(bool en);
    bool getAccumulate() const;

    void setMarkerAlpha(unsigned int which, int alpha);
    int getMarkerAlpha(unsigned int which) const;
    void setLineColor(unsigned int which, QColor color);

    void setNumBins(unsigned int bins);
    void setXaxis(double min, double max);

    void clear();

private:
    void _resetXAxisPoints(double left, double right);
    void _autoScaleY(double bottom, double top);

    double* d_xdata;
    std::vector<double*> d_ydata;

    unsigned int d_bins;
    bool d_accum;
    double d_xmin, d_xmax, d_left, d_right;
    double d_width;

    bool d_semilogx;
    bool d_semilogy;
    bool d_autoscalex_state;
};

#endif /* HISTOGRAM_DISPLAY_PLOT_H */

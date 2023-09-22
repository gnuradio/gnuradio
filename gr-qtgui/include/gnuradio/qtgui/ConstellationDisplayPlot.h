/* -*- c++ -*- */
/*
 * Copyright 2008-2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef CONSTELLATION_DISPLAY_PLOT_H
#define CONSTELLATION_DISPLAY_PLOT_H

#include <gnuradio/qtgui/DisplayPlot.h>
#include <cstdint>
#include <cstdio>
#include <vector>

/*!
 * \brief QWidget for displaying constellaton (I&Q) plots.
 * \ingroup qtgui_blk
 */
class ConstellationDisplayPlot : public DisplayPlot
{
    Q_OBJECT

public:
    ConstellationDisplayPlot(int nplots, QWidget*);
    ~ConstellationDisplayPlot() override;

    void plotNewData(const std::vector<double*> realDataPoints,
                     const std::vector<double*> imagDataPoints,
                     const int64_t numDataPoints,
                     const double timeInterval);

    // Old method to be removed
    void plotNewData(const double* realDataPoints,
                     const double* imagDataPoints,
                     const int64_t numDataPoints,
                     const double timeInterval);

    void replot() override;

    void set_xaxis(double min, double max);
    void set_yaxis(double min, double max);
    void set_axis(double xmin, double xmax, double ymin, double ymax);
    void set_pen_size(int size);

public slots:
    void setAutoScale(bool state);

private:
    void _autoScale(double bottom, double top);

    std::vector<std::vector<double>> d_real_data;
    std::vector<std::vector<double>> d_imag_data;

    int64_t d_pen_size;
};

#endif /* CONSTELLATION_DISPLAY_PLOT_H */

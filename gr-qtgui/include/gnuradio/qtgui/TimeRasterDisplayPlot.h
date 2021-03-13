/* -*- c++ -*- */
/*
 * Copyright 2012,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef TIMERASTER_DISPLAY_PLOT_H
#define TIMERASTER_DISPLAY_PLOT_H

#include <gnuradio/high_res_timer.h>
#include <gnuradio/qtgui/DisplayPlot.h>
#include <gnuradio/qtgui/plot_raster.h>
#include <gnuradio/qtgui/timeRasterGlobalData.h>
#include <qwt_plot_rasteritem.h>
#include <cstdint>
#include <cstdio>
#include <vector>

#if QWT_VERSION < 0x060000
#include <gnuradio/qtgui/plot_waterfall.h>
#else
#include <qwt_compat.h>
#endif

/*!
 * \brief QWidget for time raster (time vs. time) plots.
 * \ingroup qtgui_blk
 */
class TimeRasterDisplayPlot : public DisplayPlot
{
    Q_OBJECT

    Q_PROPERTY(int intensity_color_map_type1 READ getIntensityColorMapType1 WRITE
                   setIntensityColorMapType1)
    Q_PROPERTY(int color_map_title_font_size READ getColorMapTitleFontSize WRITE
                   setColorMapTitleFontSize)

public:
    TimeRasterDisplayPlot(
        int nplots, double samp_rate, double rows, double cols, QWidget*);
    ~TimeRasterDisplayPlot() override;

    void reset();

    void setNumRows(double rows);
    void setNumCols(double cols);
    void setAlpha(unsigned int which, int alpha);
    void setSampleRate(double samprate);
    void setXLabel(const std::string& label);
    void setXAxis(double start, double end);
    void setYLabel(const std::string& label);
    void setYAxis(double start, double end);

    double numRows() const;
    double numCols() const;

    int getAlpha(unsigned int which);

    void setPlotDimensions(const double rows,
                           const double cols,
                           const double units,
                           const std::string& strunits);

    void plotNewData(const std::vector<double*> dataPoints, const uint64_t numDataPoints);

    void plotNewData(const double* dataPoints, const uint64_t numDataPoints);

    void setIntensityRange(const double minIntensity, const double maxIntensity);

    void replot(void) override;

    int getIntensityColorMapType(unsigned int) const;
    int getIntensityColorMapType1() const;
    void
    setIntensityColorMapType(const unsigned int, const int, const QColor, const QColor);
    void setIntensityColorMapType1(int);
    int getColorMapTitleFontSize() const;
    void setColorMapTitleFontSize(int tfs);
    const QColor getUserDefinedLowIntensityColor() const;
    const QColor getUserDefinedHighIntensityColor() const;

    double getMinIntensity(unsigned int which) const;
    double getMaxIntensity(unsigned int which) const;

signals:
    void updatedLowerIntensityLevel(const double);
    void updatedUpperIntensityLevel(const double);

private:
    void _updateIntensityRangeDisplay();

    std::vector<TimeRasterData*> d_data;
    std::vector<PlotTimeRaster*> d_raster;

    double d_samp_rate;
    double d_rows, d_cols;

    std::vector<int> d_color_map_type;
    QColor d_low_intensity;
    QColor d_high_intensity;

    int d_color_bar_title_font_size;

    std::string d_x_label;
    double d_x_start_value;
    double d_x_end_value;
    std::string d_y_label;
    double d_y_start_value;
    double d_y_end_value;
};

#endif /* TIMERASTER_DISPLAY_PLOT_H */

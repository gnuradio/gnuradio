/* -*- c++ -*- */
/*
 * Copyright 2008-2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef WATERFALL_DISPLAY_PLOT_H
#define WATERFALL_DISPLAY_PLOT_H

#include <gnuradio/high_res_timer.h>
#include <gnuradio/qtgui/DisplayPlot.h>
#include <gnuradio/qtgui/waterfallGlobalData.h>
#include <qwt_plot_spectrogram.h>
#include <cstdint>
#include <cstdio>
#include <vector>

#if QWT_VERSION < 0x060000
#include <gnuradio/qtgui/plot_waterfall.h>
#else
#include <qwt_compat.h>
#endif

/*!
 * \brief QWidget for displaying waterfall (spectrogram) plots.
 * \ingroup qtgui_blk
 */
class WaterfallDisplayPlot : public DisplayPlot
{
    Q_OBJECT

    Q_PROPERTY(int intensity_color_map_type1 READ getIntensityColorMapType1 WRITE
                   setIntensityColorMapType1)
    Q_PROPERTY(QColor low_intensity_color READ getUserDefinedLowIntensityColor WRITE
                   setUserDefinedLowIntensityColor)
    Q_PROPERTY(QColor high_intensity_color READ getUserDefinedHighIntensityColor WRITE
                   setUserDefinedHighIntensityColor)
    Q_PROPERTY(int color_map_title_font_size READ getColorMapTitleFontSize WRITE
                   setColorMapTitleFontSize)


public:
    WaterfallDisplayPlot(int nplots, QWidget*);
    ~WaterfallDisplayPlot() override;

    void resetAxis();

    void setFrequencyRange(const double,
                           const double,
                           const double units = 1000.0,
                           const std::string& strunits = "kHz");
    double getStartFrequency() const;
    double getStopFrequency() const;

    void plotNewData(const std::vector<double*> dataPoints,
                     const int64_t numDataPoints,
                     const double timePerFFT,
                     const gr::high_res_timer_type timestamp,
                     const int droppedFrames);

    // to be removed
    void plotNewData(const double* dataPoints,
                     const int64_t numDataPoints,
                     const double timePerFFT,
                     const gr::high_res_timer_type timestamp,
                     const int droppedFrames);

    void setIntensityRange(const double minIntensity, const double maxIntensity);
    double getMinIntensity(unsigned int which) const;
    double getMaxIntensity(unsigned int which) const;

    void replot(void) override;
    void clearData();

    int getIntensityColorMapType(unsigned int) const;
    int getIntensityColorMapType1() const;
    int getColorMapTitleFontSize() const;
    const QColor getUserDefinedLowIntensityColor() const;
    const QColor getUserDefinedHighIntensityColor() const;

    int getAlpha(unsigned int which);
    void setAlpha(unsigned int which, int alpha);

    int getNumRows() const;

public slots:
    void
    setIntensityColorMapType(const unsigned int, const int, const QColor, const QColor);
    void setIntensityColorMapType1(int);
    void setColorMapTitleFontSize(int tfs);
    void setUserDefinedLowIntensityColor(QColor);
    void setUserDefinedHighIntensityColor(QColor);
    void setPlotPosHalf(bool half);
    void disableLegend() override;
    void enableLegend();
    void enableLegend(bool en);
    void setNumRows(int nrows);

signals:
    void updatedLowerIntensityLevel(const double);
    void updatedUpperIntensityLevel(const double);

private:
    void _updateIntensityRangeDisplay();

    double d_start_frequency;
    double d_stop_frequency;
    double d_center_frequency;
    int d_xaxis_multiplier;
    bool d_half_freq;
    bool d_legend_enabled;
    int d_nrows;

    std::vector<WaterfallData*> d_data;

#if QWT_VERSION < 0x060000
    std::vector<PlotWaterfall*> d_spectrogram;
#else
    std::vector<QwtPlotSpectrogram*> d_spectrogram;
#endif

    std::vector<int> d_intensity_color_map_type;
    QColor d_user_defined_low_intensity_color;
    QColor d_user_defined_high_intensity_color;
    int d_color_bar_title_font_size;
};

#endif /* WATERFALL_DISPLAY_PLOT_H */

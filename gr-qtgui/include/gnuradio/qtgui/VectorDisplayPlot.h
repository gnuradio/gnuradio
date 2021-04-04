/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef VECTOR_DISPLAY_PLOT_HPP
#define VECTOR_DISPLAY_PLOT_HPP

#include <gnuradio/qtgui/DisplayPlot.h>
#include <cstdint>
#include <cstdio>
#include <vector>

/*!
 * \brief QWidget for displaying 1D-vector plots.
 * \ingroup qtgui_blk
 */
class VectorDisplayPlot : public DisplayPlot
{
    Q_OBJECT

    Q_PROPERTY(QColor min_vec_color READ getMinVecColor WRITE setMinVecColor)
    Q_PROPERTY(QColor max_vec_color READ getMaxVecColor WRITE setMaxVecColor)
    Q_PROPERTY(bool min_vec_visible READ getMinVecVisible WRITE setMinVecVisible)
    Q_PROPERTY(bool max_vec_visible READ getMaxVecVisible WRITE setMaxVecVisible)
    Q_PROPERTY(QColor marker_lower_intensity_color READ getMarkerLowerIntensityColor WRITE
                   setMarkerLowerIntensityColor)
    Q_PROPERTY(bool marker_lower_intensity_visible READ getMarkerLowerIntensityVisible
                   WRITE setMarkerLowerIntensityVisible)
    Q_PROPERTY(QColor marker_upper_intensity_color READ getMarkerUpperIntensityColor WRITE
                   setMarkerUpperIntensityColor)
    Q_PROPERTY(bool marker_upper_intensity_visible READ getMarkerUpperIntensityVisible
                   WRITE setMarkerUpperIntensityVisible)
    Q_PROPERTY(QColor marker_ref_level_color READ getMarkerRefLevelAmplitudeColor WRITE
                   setMarkerRefLevelAmplitudeColor)
    Q_PROPERTY(bool marker_ref_level_visible READ getMarkerRefLevelAmplitudeVisible WRITE
                   setMarkerRefLevelAmplitudeVisible)

public:
    VectorDisplayPlot(int nplots, QWidget*);
    ~VectorDisplayPlot() override;

    // Disable move/copy because of raw QT pointers.
    VectorDisplayPlot(const VectorDisplayPlot&) = delete;
    VectorDisplayPlot(VectorDisplayPlot&&) = delete;
    VectorDisplayPlot& operator=(const VectorDisplayPlot&) = delete;
    VectorDisplayPlot& operator=(VectorDisplayPlot&&) = delete;

    void setXAxisValues(const double start, const double step = 1.0);

    void plotNewData(const std::vector<double*> dataPoints,
                     const int64_t numDataPoints,
                     const double refLevel,
                     const double timeInterval);

    void clearMaxData();
    void clearMinData();

    void replot() override;

    void setYaxis(double min, double max) override;
    double getYMin() const;
    double getYMax() const;

    void setXAxisLabel(const QString& label);
    void setYAxisLabel(const QString& label);

    void setXAxisUnit(const QString& unit);
    void setYAxisUnit(const QString& unit);

    void setTraceColour(QColor);
    void setBGColour(QColor c);

    const bool getMaxVecVisible() const;
    const bool getMinVecVisible() const;
    const QColor getMinVecColor() const;
    const QColor getMaxVecColor() const;
    const QColor getMarkerLowerIntensityColor() const;
    const bool getMarkerLowerIntensityVisible() const;
    const QColor getMarkerUpperIntensityColor() const;
    const bool getMarkerUpperIntensityVisible() const;
    const bool getMarkerRefLevelAmplitudeVisible() const;
    const QColor getMarkerRefLevelAmplitudeColor() const;

public slots:
    void setMaxVecVisible(const bool);
    void setMinVecVisible(const bool);
    void setMinVecColor(QColor c);
    void setMaxVecColor(QColor c);
    void setMarkerLowerIntensityColor(QColor c);
    void setMarkerLowerIntensityVisible(bool visible);
    void setMarkerUpperIntensityColor(QColor c);
    void setMarkerUpperIntensityVisible(bool visible);
    void setMarkerRefLevelAmplitudeVisible(bool visible);
    void setMarkerRefLevelAmplitudeColor(QColor c);

    void setLowerIntensityLevel(const double);
    void setUpperIntensityLevel(const double);

    void onPickerPointSelected(const QwtDoublePoint& p);
    void onPickerPointSelected6(const QPointF& p);

    void setAutoScale(bool state);

private:
    void _resetXAxisPoints();
    void _autoScale(double bottom, double top);

    std::vector<std::vector<double>> d_ydata;

    QwtPlotCurve* d_min_vec_plot_curve;
    QwtPlotCurve* d_max_vec_plot_curve;
    QColor d_min_vec_color;
    bool d_min_vec_visible;
    QColor d_max_vec_color;
    bool d_max_vec_visible;
    QColor d_marker_lower_intensity_color;
    bool d_marker_lower_intensity_visible;
    QColor d_marker_upper_intensity_color;
    bool d_marker_upper_intensity_visible;
    QColor d_marker_ref_level_color;
    bool d_marker_ref_level_visible;

    double d_x_axis_start = 0;
    double d_x_axis_step = 1.0;

    double d_ymax = 10;
    double d_ymin = -10;

    QwtPlotMarker* d_lower_intensity_marker;
    QwtPlotMarker* d_upper_intensity_marker;

    QwtPlotMarker* d_marker_ref_level;

    std::vector<double> d_xdata;

    QString d_x_axis_label;
    QString d_y_axis_label;

    std::vector<double> d_min_vec_data;
    std::vector<double> d_max_vec_data;

    double d_ref_level;
};

#endif /* VECTOR_DISPLAY_PLOT_HPP */

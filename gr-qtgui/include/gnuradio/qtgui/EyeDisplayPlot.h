/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EYE_DISPLAY_PLOT_H
#define EYE_DISPLAY_PLOT_H

#include <gnuradio/qtgui/DisplayPlot.h>
#include <gnuradio/tags.h>
#include <cstdint>
#include <cstdio>
#include <vector>

/*!
 * \brief QWidget for displaying eye pattern plots.
 * \ingroup qtgui_blk
 */
class EyeDisplayPlot : public DisplayPlot
{
    Q_OBJECT

    Q_PROPERTY(QColor tag_text_color READ getTagTextColor WRITE setTagTextColor)
    Q_PROPERTY(QColor tag_background_color READ getTagBackgroundColor WRITE
                   setTagBackgroundColor)
    Q_PROPERTY(Qt::BrushStyle tag_background_style READ getTagBackgroundStyle WRITE
                   setTagBackgroundStyle)

public:
    EyeDisplayPlot(unsigned int nplots, unsigned int curve_index, QWidget* parent);
    ~EyeDisplayPlot() override;

    // Disable copy&move because of raw QT pointers.
    EyeDisplayPlot(const EyeDisplayPlot&) = delete;
    EyeDisplayPlot(EyeDisplayPlot&&) = delete;
    EyeDisplayPlot& operator=(const EyeDisplayPlot&) = delete;
    EyeDisplayPlot& operator=(EyeDisplayPlot&&) = delete;

    void plotNewData(const std::vector<double*> dataPoints,
                     const int64_t numDataPoints,
                     int d_sps,
                     const double timeInterval,
                     const std::vector<std::vector<gr::tag_t>>& tags =
                         std::vector<std::vector<gr::tag_t>>());

    void replot() override;

    void stemPlot(bool en);

    double sampleRate() const;

    const QColor getTagTextColor();
    const QColor getTagBackgroundColor();
    const Qt::BrushStyle getTagBackgroundStyle();
    void setLineColor(unsigned int which, QColor color) override;
    void setLineWidth(unsigned int which, int width) override;
    void setLineMarker(unsigned int which, QwtSymbol::Style marker) override;
    void setLineStyle(unsigned int which, Qt::PenStyle style) override;
    void setMarkerAlpha(unsigned int which, int alpha) override;

public slots:
    void setSampleRate(double sr, double units, const std::string& strunits);

    void setAutoScale(bool state);
    void setAutoScaleShot();

    void legendEntryChecked(QwtPlotItem* plotItem, bool on) override;
    void legendEntryChecked(const QVariant& plotItem, bool on, int index) override;

    void enableTagMarker(unsigned int which, bool en);

    void setYLabel(const std::string& label, const std::string& unit = "");

    void attachTriggerLines(bool en);
    void setTriggerLines(double x, double y);

    void setTagTextColor(QColor c);
    void setTagBackgroundColor(QColor c);
    void setTagBackgroundStyle(Qt::BrushStyle b);

    void setLineLabel(unsigned int which, QString label) override;

private:
    void _resetXAxisPoints();
    void _autoScale(double bottom, double top);

    std::vector<std::vector<double>> d_ydata;

    std::vector<double> d_xdata;

    double d_sample_rate;

    unsigned int d_curve_index;
    int d_sps = 4;
    unsigned int d_numPointsPerPeriod = 2 * d_sps + 1;
    unsigned int d_numPeriods;

    bool d_autoscale_shot;

    std::vector<std::vector<QwtPlotMarker*>> d_tag_markers;
    std::vector<bool> d_tag_markers_en;

    QList<QColor> colors;
    QColor d_tag_text_color;
    QColor d_tag_background_color;
    Qt::BrushStyle d_tag_background_style;

    QwtPlotMarker* d_trigger_lines[2];
};

#endif /* EYE_DISPLAY_PLOT_H */

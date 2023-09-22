/* -*- c++ -*- */
/*
 * Copyright 2008-2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef TIME_DOMAIN_DISPLAY_PLOT_H
#define TIME_DOMAIN_DISPLAY_PLOT_H

#include <gnuradio/qtgui/DisplayPlot.h>
#include <gnuradio/tags.h>
#include <cstdint>
#include <cstdio>
#include <vector>

/*!
 * \brief QWidget for displaying time domain plots.
 * \ingroup qtgui_blk
 */
class TimeDomainDisplayPlot : public DisplayPlot
{
    Q_OBJECT

    Q_PROPERTY(QColor tag_text_color READ getTagTextColor WRITE setTagTextColor)
    Q_PROPERTY(QColor tag_background_color READ getTagBackgroundColor WRITE
                   setTagBackgroundColor)
    Q_PROPERTY(Qt::BrushStyle tag_background_style READ getTagBackgroundStyle WRITE
                   setTagBackgroundStyle)

public:
    TimeDomainDisplayPlot(int nplots, QWidget*);
    ~TimeDomainDisplayPlot() override;

    // Disable move/delete because of raw QT pointers.
    TimeDomainDisplayPlot(const TimeDomainDisplayPlot&) = delete;
    TimeDomainDisplayPlot(TimeDomainDisplayPlot&&) = delete;
    TimeDomainDisplayPlot& operator=(const TimeDomainDisplayPlot&) = delete;
    TimeDomainDisplayPlot& operator=(TimeDomainDisplayPlot&&) = delete;

    void plotNewData(const std::vector<double*> dataPoints,
                     const int64_t numDataPoints,
                     const double timeInterval,
                     const std::vector<std::vector<gr::tag_t>>& tags =
                         std::vector<std::vector<gr::tag_t>>());

    void replot() override;

    void stemPlot(bool en);

    double sampleRate() const;

    const QColor getTagTextColor();
    const QColor getTagBackgroundColor();
    const Qt::BrushStyle getTagBackgroundStyle();

public slots:
    void setSampleRate(double sr, double units, const std::string& strunits);

    void setAutoScale(bool state);
    void setAutoScaleShot();
    void setSemilogx(bool en);
    void setSemilogy(bool en);

    void legendEntryChecked(QwtPlotItem* plotItem, bool on) override;
    void legendEntryChecked(const QVariant& plotItem, bool on, int index) override;

    void enableTagMarker(unsigned int which, bool en);

    void setYLabel(const std::string& label, const std::string& unit = "");

    void attachTriggerLines(bool en);
    void setTriggerLines(double x, double y);

    void setTagTextColor(QColor c);
    void setTagBackgroundColor(QColor c);
    void setTagBackgroundStyle(Qt::BrushStyle b);

private:
    void _resetXAxisPoints();
    void _autoScale(double bottom, double top);

    std::vector<std::vector<double>> d_ydata;
    std::vector<double> d_xdata;

    double d_sample_rate;

    bool d_semilogx;
    bool d_semilogy;
    bool d_autoscale_shot;

    std::vector<std::vector<QwtPlotMarker*>> d_tag_markers;
    std::vector<bool> d_tag_markers_en;

    QColor d_tag_text_color = Qt::black;
    QColor d_tag_background_color = Qt::white;
    Qt::BrushStyle d_tag_background_style = Qt::NoBrush;

    QwtPlotMarker* d_trigger_lines[2];
};

#endif /* TIME_DOMAIN_DISPLAY_PLOT_H */

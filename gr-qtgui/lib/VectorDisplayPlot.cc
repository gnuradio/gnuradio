/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef VECTOR_DISPLAY_PLOT
#define VECTOR_DISPLAY_PLOT

#include <gnuradio/qtgui/VectorDisplayPlot.h>

#include <gnuradio/qtgui/qtgui_types.h>
#include <qwt_legend.h>
#include <qwt_scale_draw.h>
#include <QColor>
#include <iostream>

#if QWT_VERSION < 0x060100
#include <qwt_legend_item.h>
#else /* QWT_VERSION < 0x060100 */
#include <qwt_legend_data.h>
#include <qwt_legend_label.h>
#endif /* QWT_VERSION < 0x060100 */

/***********************************************************************
 * Widget to provide mouse pointer coordinate text
 **********************************************************************/
class VectorDisplayZoomer : public QwtPlotZoomer
{
public:
#if QWT_VERSION < 0x060100
    VectorDisplayZoomer(QwtPlotCanvas* canvas)
#else  /* QWT_VERSION < 0x060100 */
    VectorDisplayZoomer(QWidget* canvas)
#endif /* QWT_VERSION < 0x060100 */
        : QwtPlotZoomer(canvas), d_x_units(" "), d_y_units(" ")
    {
        setTrackerMode(QwtPicker::AlwaysOn);
    }

    virtual void updateTrackerText() { updateDisplay(); }

    void setXUnits(const QString& units) { d_x_units = units; }

    void setYUnits(const QString& units) { d_y_units = units; }

protected:
    using QwtPlotZoomer::trackerText;
    QwtText trackerText(QPoint const& p) const override
    {
        QwtDoublePoint dp = QwtPlotZoomer::invTransform(p);
        QwtText t(QString("%1 %2, %3 %4")
                      .arg(dp.x(), 0, 'f', 2)
                      .arg(d_x_units)
                      .arg(dp.y(), 0, 'f', 2)
                      .arg(d_y_units));
        return t;
    }

private:
    QString d_x_units; //!< Units on x-Axis (e.g. Hz)
    QString d_y_units; //!< Units on y-Axis (e.g. V)
};

namespace {
constexpr int default_numpoints = 1024;
}

/***********************************************************************
 * Main frequency display plotter widget
 **********************************************************************/
VectorDisplayPlot::VectorDisplayPlot(int nplots, QWidget* parent)
    : DisplayPlot(nplots, parent),
      d_xdata(default_numpoints),
      d_x_axis_label("x"),
      d_y_axis_label("y"),
      d_min_vec_data(default_numpoints),
      d_max_vec_data(default_numpoints)
{
    d_numPoints = default_numpoints;

    setAxisTitle(QwtPlot::xBottom, d_x_axis_label);
    setAxisScale(QwtPlot::xBottom, d_x_axis_start, d_numPoints - 1);

    setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);
    setAxisScale(QwtPlot::yLeft, d_ymin, d_ymax);
    setAxisTitle(QwtPlot::yLeft, d_y_axis_label);

    QList<QColor> default_colors;
    default_colors << QColor(Qt::blue) << QColor(Qt::red) << QColor(Qt::green)
                   << QColor(Qt::black) << QColor(Qt::cyan) << QColor(Qt::magenta)
                   << QColor(Qt::yellow) << QColor(Qt::gray) << QColor(Qt::darkRed)
                   << QColor(Qt::darkGreen) << QColor(Qt::darkBlue)
                   << QColor(Qt::darkGray);

    // Create a curve for each input
    // Automatically deleted when parent is deleted
    for (unsigned int i = 0; i < d_nplots; ++i) {
        d_ydata.emplace_back(d_numPoints);

        d_plot_curve.push_back(new QwtPlotCurve(QString("Data %1").arg(i)));
        d_plot_curve[i]->attach(this);

        QwtSymbol* symbol = new QwtSymbol(QwtSymbol::NoSymbol,
                                          QBrush(default_colors[i]),
                                          QPen(default_colors[i]),
                                          QSize(7, 7));

#if QWT_VERSION < 0x060000
        d_plot_curve[i]->setRawData(d_xdata.data(), d_ydata[i].data(), d_numPoints);
        d_plot_curve[i]->setSymbol(*symbol);
#else
        d_plot_curve[i]->setRawSamples(d_xdata.data(), d_ydata[i].data(), d_numPoints);
        d_plot_curve[i]->setSymbol(symbol);
#endif
        setLineColor(i, default_colors[i]);
    }

    // Create min/max plotter curves
    d_min_vec_plot_curve = new QwtPlotCurve("Min Hold");
    d_min_vec_plot_curve->attach(this);
    const QColor default_min_fft_color = Qt::magenta;
    setMinVecColor(default_min_fft_color);
#if QWT_VERSION < 0x060000
    d_min_vec_plot_curve->setRawData(d_xdata.data(), d_min_vec_data.data(), d_numPoints);
#else
    d_min_vec_plot_curve->setRawSamples(
        d_xdata.data(), d_min_vec_data.data(), d_numPoints);
#endif
    d_min_vec_plot_curve->setVisible(false);
    d_min_vec_plot_curve->setZ(0);

    d_max_vec_plot_curve = new QwtPlotCurve("Max Hold");
    d_max_vec_plot_curve->attach(this);
    QColor default_max_fft_color = Qt::darkYellow;
    setMaxVecColor(default_max_fft_color);
#if QWT_VERSION < 0x060000
    d_max_vec_plot_curve->setRawData(d_xdata.data(), d_max_vec_data.data(), d_numPoints);
#else
    d_max_vec_plot_curve->setRawSamples(
        d_xdata.data(), d_max_vec_data.data(), d_numPoints);
#endif
    d_max_vec_plot_curve->setVisible(false);
    d_max_vec_plot_curve->setZ(0);

    d_lower_intensity_marker = new QwtPlotMarker();
    d_lower_intensity_marker->setLineStyle(QwtPlotMarker::HLine);
    QColor default_marker_lower_intensity_color = Qt::cyan;
    setMarkerLowerIntensityColor(default_marker_lower_intensity_color);
    d_lower_intensity_marker->attach(this);

    d_upper_intensity_marker = new QwtPlotMarker();
    d_upper_intensity_marker->setLineStyle(QwtPlotMarker::HLine);
    QColor default_marker_upper_intensity_color = Qt::green;
    setMarkerUpperIntensityColor(default_marker_upper_intensity_color);
    d_upper_intensity_marker->attach(this);

    std::fill(std::begin(d_xdata), std::end(d_xdata), 0.0);

    for (int64_t number = 0; number < d_numPoints; number++) {
        d_min_vec_data[number] = 1e6;
        d_max_vec_data[number] = -1e6;
    }

    d_marker_ref_level = new QwtPlotMarker();
    d_marker_ref_level->setLineStyle(QwtPlotMarker::HLine);
    QColor d_default_marker_ref_level_color = Qt::darkRed;
    setMarkerRefLevelAmplitudeColor(d_default_marker_ref_level_color);
    d_marker_ref_level->attach(this);

    d_ref_level = -HUGE_VAL;

    d_zoomer = new VectorDisplayZoomer(canvas());

#if QWT_VERSION < 0x060000
    d_zoomer->setSelectionFlags(QwtPicker::RectSelection | QwtPicker::DragSelection);
#endif

    d_zoomer->setMousePattern(
        QwtEventPattern::MouseSelect2, Qt::RightButton, Qt::ControlModifier);
    d_zoomer->setMousePattern(QwtEventPattern::MouseSelect3, Qt::RightButton);


    const QColor default_zoomer_color(Qt::darkRed);
    setZoomerColor(default_zoomer_color);

    // Do this after the zoomer has been built
    _resetXAxisPoints();

    // Turn off min/max hold plots in legend
#if QWT_VERSION < 0x060100
    QWidget* w;
    QwtLegend* legendDisplay = legend();
    w = legendDisplay->find(d_min_vec_plot_curve);
    ((QwtLegendItem*)w)->setChecked(true);
    w = legendDisplay->find(d_max_vec_plot_curve);
    ((QwtLegendItem*)w)->setChecked(true);
#else  /* QWT_VERSION < 0x060100 */
    QWidget* w;
    w = ((QwtLegend*)legend())->legendWidget(itemToInfo(d_min_vec_plot_curve));
    ((QwtLegendLabel*)w)->setChecked(true);
    w = ((QwtLegend*)legend())->legendWidget(itemToInfo(d_max_vec_plot_curve));
    ((QwtLegendLabel*)w)->setChecked(true);
#endif /* QWT_VERSION < 0x060100 */

    replot();
}

VectorDisplayPlot::~VectorDisplayPlot() {}

void VectorDisplayPlot::setYaxis(double min, double max)
{
    // Get the new max/min values for the plot
    d_ymin = min;
    d_ymax = max;

    // Set the axis max/min to the new values
    setAxisScale(QwtPlot::yLeft, d_ymin, d_ymax);

    // Reset the base zoom level to the new axis scale set here.
    // But don't do it if we set the axis due to auto scaling.
    if (!d_autoscale_state)
        d_zoomer->setZoomBase();
}

double VectorDisplayPlot::getYMin() const { return d_ymin; }

double VectorDisplayPlot::getYMax() const { return d_ymax; }

void VectorDisplayPlot::setXAxisLabel(const QString& label)
{
    d_x_axis_label = label;
    setAxisTitle(QwtPlot::xBottom, label);
}

void VectorDisplayPlot::setYAxisLabel(const QString& label)
{
    d_y_axis_label = label;
    setAxisTitle(QwtPlot::yLeft, label);
}

void VectorDisplayPlot::setXAxisUnit(const QString& unit)
{
    ((VectorDisplayZoomer*)d_zoomer)->setXUnits(unit);
}

void VectorDisplayPlot::setYAxisUnit(const QString& unit)
{
    ((VectorDisplayZoomer*)d_zoomer)->setYUnits(unit);
}

void VectorDisplayPlot::setXAxisValues(const double start, const double step)
{
    bool reset = false;
    if ((start != d_x_axis_start) || (step != d_x_axis_step))
        reset = true;

    d_x_axis_start = start;
    d_x_axis_step = step;

    if ((axisScaleDraw(QwtPlot::xBottom) != NULL) && (d_zoomer != NULL)) {
        setAxisTitle(QwtPlot::xBottom, d_x_axis_label);
        if (reset) {
            _resetXAxisPoints();
            clearMaxData();
            clearMinData();
        }
    }
}

void VectorDisplayPlot::replot()
{
    d_marker_ref_level->setYValue(d_ref_level);
    QwtPlot::replot();
}

void VectorDisplayPlot::plotNewData(const std::vector<double*> dataPoints,
                                    const int64_t numDataPoints,
                                    const double refLevel,
                                    const double timeInterval)
{
    if (!d_stop) {
        if (numDataPoints > 0) {
            if (numDataPoints != d_numPoints) {
                d_numPoints = numDataPoints;

                d_xdata.resize(d_numPoints);
                d_min_vec_data.resize(d_numPoints);
                d_max_vec_data.resize(d_numPoints);

                for (unsigned int i = 0; i < d_nplots; ++i) {
                    d_ydata[i].resize(d_numPoints);

#if QWT_VERSION < 0x060000
                    d_plot_curve[i]->setRawData(
                        d_xdata.data(), d_ydata[i].data(), d_numPoints);
#else
                    d_plot_curve[i]->setRawSamples(
                        d_xdata.data(), d_ydata[i].data(), d_numPoints);
#endif
                }
#if QWT_VERSION < 0x060000
                d_min_vec_plot_curve->setRawData(
                    d_xdata.data(), d_min_vec_data.data(), d_numPoints);
                d_max_vec_plot_curve->setRawData(
                    d_xdata.data(), d_max_vec_data.data(), d_numPoints);
#else
                d_min_vec_plot_curve->setRawSamples(
                    d_xdata.data(), d_min_vec_data.data(), d_numPoints);
                d_max_vec_plot_curve->setRawSamples(
                    d_xdata.data(), d_max_vec_data.data(), d_numPoints);
#endif
                _resetXAxisPoints();
                clearMaxData();
                clearMinData();
            }

            double bottom = 1e20, top = -1e20;
            for (unsigned int n = 0; n < d_nplots; ++n) {

                memcpy(d_ydata[n].data(), dataPoints[n], numDataPoints * sizeof(double));

                for (int64_t point = 0; point < numDataPoints; point++) {
                    if (dataPoints[n][point] < d_min_vec_data[point]) {
                        d_min_vec_data[point] = dataPoints[n][point];
                    }
                    if (dataPoints[n][point] > d_max_vec_data[point]) {
                        d_max_vec_data[point] = dataPoints[n][point];
                    }

                    // Find overall top and bottom values in plot.
                    // Used for autoscaling y-axis.
                    if (dataPoints[n][point] < bottom) {
                        bottom = dataPoints[n][point];
                    }
                    if (dataPoints[n][point] > top) {
                        top = dataPoints[n][point];
                    }
                }
            }

            if (d_autoscale_state)
                _autoScale(bottom, top);

            d_ref_level = refLevel;

            replot();
        }
    }
}

void VectorDisplayPlot::clearMaxData()
{
    for (int64_t number = 0; number < d_numPoints; number++) {
        d_max_vec_data[number] = d_ymin;
    }
}

void VectorDisplayPlot::clearMinData()
{
    for (int64_t number = 0; number < d_numPoints; number++) {
        d_min_vec_data[number] = d_ymax;
    }
}

void VectorDisplayPlot::_autoScale(double bottom, double top)
{
    // Auto scale the y-axis with a margin of 10 dB on either side.
    d_ymin = bottom - 10;
    d_ymax = top + 10;
    setYaxis(d_ymin, d_ymax);
}

void VectorDisplayPlot::setAutoScale(bool state) { d_autoscale_state = state; }

void VectorDisplayPlot::setMaxVecVisible(const bool visibleFlag)
{
    d_max_vec_visible = visibleFlag;
    d_max_vec_plot_curve->setVisible(visibleFlag);
}

const bool VectorDisplayPlot::getMaxVecVisible() const { return d_max_vec_visible; }

void VectorDisplayPlot::setMinVecVisible(const bool visibleFlag)
{
    d_min_vec_visible = visibleFlag;
    d_min_vec_plot_curve->setVisible(visibleFlag);
}

const bool VectorDisplayPlot::getMinVecVisible() const { return d_min_vec_visible; }

void VectorDisplayPlot::_resetXAxisPoints()
{
    double xValue = d_x_axis_start;
    for (int64_t loc = 0; loc < d_numPoints; loc++) {
        d_xdata[loc] = xValue;
        xValue += d_x_axis_step;
    }

    setAxisScale(QwtPlot::xBottom, d_xdata[0], d_xdata[d_numPoints - 1]);

    // Set up zoomer base for maximum unzoom x-axis
    // and reset to maximum unzoom level
    QwtDoubleRect zbase = d_zoomer->zoomBase();
    d_zoomer->zoom(zbase);
    d_zoomer->setZoomBase(zbase);
    d_zoomer->setZoomBase(true);
    d_zoomer->zoom(0);
}

void VectorDisplayPlot::setLowerIntensityLevel(const double lowerIntensityLevel)
{
    d_lower_intensity_marker->setYValue(lowerIntensityLevel);
}

void VectorDisplayPlot::setUpperIntensityLevel(const double upperIntensityLevel)
{
    d_upper_intensity_marker->setYValue(upperIntensityLevel);
}

void VectorDisplayPlot::setTraceColour(QColor c) { d_plot_curve[0]->setPen(QPen(c)); }

void VectorDisplayPlot::setBGColour(QColor c)
{
    QPalette palette;
    palette.setColor(canvas()->backgroundRole(), c);
    canvas()->setPalette(palette);
}

void VectorDisplayPlot::onPickerPointSelected(const QwtDoublePoint& p)
{
    QPointF point = p;
    point.setX(point.x());
    emit plotPointSelected(point);
}

void VectorDisplayPlot::onPickerPointSelected6(const QPointF& p)
{
    QPointF point = p;
    point.setX(point.x());
    emit plotPointSelected(point);
}

void VectorDisplayPlot::setMinVecColor(QColor c)
{
    d_min_vec_color = c;
    d_min_vec_plot_curve->setPen(QPen(c));
}

const QColor VectorDisplayPlot::getMinVecColor() const { return d_min_vec_color; }

void VectorDisplayPlot::setMaxVecColor(QColor c)
{
    d_max_vec_color = c;
    d_max_vec_plot_curve->setPen(QPen(c));
}

const QColor VectorDisplayPlot::getMaxVecColor() const { return d_max_vec_color; }

void VectorDisplayPlot::setMarkerLowerIntensityColor(QColor c)
{
    d_marker_lower_intensity_color = c;
    d_lower_intensity_marker->setLinePen(QPen(c));
}
const QColor VectorDisplayPlot::getMarkerLowerIntensityColor() const
{
    return d_marker_lower_intensity_color;
}

void VectorDisplayPlot::setMarkerLowerIntensityVisible(bool visible)
{
    d_marker_lower_intensity_visible = visible;
    if (visible)
        d_lower_intensity_marker->setLineStyle(QwtPlotMarker::HLine);
    else
        d_lower_intensity_marker->setLineStyle(QwtPlotMarker::NoLine);
}
const bool VectorDisplayPlot::getMarkerLowerIntensityVisible() const
{
    return d_marker_lower_intensity_visible;
}

void VectorDisplayPlot::setMarkerUpperIntensityColor(QColor c)
{
    d_marker_upper_intensity_color = c;
    d_upper_intensity_marker->setLinePen(QPen(c, 0, Qt::DotLine));
}

const QColor VectorDisplayPlot::getMarkerUpperIntensityColor() const
{
    return d_marker_upper_intensity_color;
}

void VectorDisplayPlot::setMarkerUpperIntensityVisible(bool visible)
{
    d_marker_upper_intensity_visible = visible;
    if (visible)
        d_upper_intensity_marker->setLineStyle(QwtPlotMarker::HLine);
    else
        d_upper_intensity_marker->setLineStyle(QwtPlotMarker::NoLine);
}

const bool VectorDisplayPlot::getMarkerUpperIntensityVisible() const
{
    return d_marker_upper_intensity_visible;
}

void VectorDisplayPlot::setMarkerRefLevelAmplitudeColor(QColor c)
{
    d_marker_ref_level_color = c;
    d_marker_ref_level->setLinePen(QPen(c, 0, Qt::DotLine));
}

const QColor VectorDisplayPlot::getMarkerRefLevelAmplitudeColor() const
{
    return d_marker_ref_level_color;
}

void VectorDisplayPlot::setMarkerRefLevelAmplitudeVisible(bool visible)
{
    d_marker_ref_level_visible = visible;
    if (visible)
        d_marker_ref_level->setLineStyle(QwtPlotMarker::HLine);
    else
        d_marker_ref_level->setLineStyle(QwtPlotMarker::NoLine);
}

const bool VectorDisplayPlot::getMarkerRefLevelAmplitudeVisible() const
{
    return d_marker_ref_level_visible;
}

#endif /* VECTOR_DISPLAY_PLOT */

/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EYE_DISPLAY_PLOT_C
#define EYE_DISPLAY_PLOT_C

#include <gnuradio/qtgui/EyeDisplayPlot.h>

#include <qwt_legend.h>
#include <qwt_scale_draw.h>
#include <volk/volk.h>
#include <QColor>
#include <cmath>
#include <iostream>

class TimePrecisionClass
{
public:
    TimePrecisionClass(const int timePrecision) { d_timePrecision = timePrecision; }

    virtual ~TimePrecisionClass() {}

    virtual unsigned int getTimePrecision() const { return d_timePrecision; }

    virtual void setTimePrecision(const unsigned int newPrecision)
    {
        d_timePrecision = newPrecision;
    }

protected:
    unsigned int d_timePrecision;
};

class EyeDisplayZoomer : public QwtPlotZoomer, public TimePrecisionClass
{
public:
#if QWT_VERSION < 0x060100
    EyeDisplayZoomer(QwtPlotCanvas* canvas, const unsigned int timePrecision)
#else  /* QWT_VERSION < 0x060100 */
    EyeDisplayZoomer(QWidget* canvas, const unsigned int timePrecision)
#endif /* QWT_VERSION < 0x060100 */
        : QwtPlotZoomer(canvas), TimePrecisionClass(timePrecision), d_yUnitType("V")
    {
        setTrackerMode(QwtPicker::AlwaysOn);
    }

    ~EyeDisplayZoomer() override {}

    virtual void updateTrackerText() { updateDisplay(); }

    void setUnitType(const std::string& type) { d_unitType = type; }

    std::string unitType() { return d_unitType; }

    void setYUnitType(const std::string& type) { d_yUnitType = type; }

protected:
    using QwtPlotZoomer::trackerText;
    QwtText trackerText(const QPoint& p) const override
    {
        QwtText t;
        QwtDoublePoint dp = QwtPlotZoomer::invTransform(p);
        if ((fabs(dp.y()) > 0.0001) && (fabs(dp.y()) < 10000)) {
            t.setText(QString("%1 %2, %3 %4")
                          .arg(dp.x(), 0, 'f', getTimePrecision())
                          .arg(d_unitType.c_str())
                          .arg(dp.y(), 0, 'f', 4)
                          .arg(d_yUnitType.c_str()));
        } else {
            t.setText(QString("%1 %2, %3 %4")
                          .arg(dp.x(), 0, 'f', getTimePrecision())
                          .arg(d_unitType.c_str())
                          .arg(dp.y(), 0, 'e', 4)
                          .arg(d_yUnitType.c_str()));
        }

        return t;
    }

private:
    std::string d_unitType;
    std::string d_yUnitType;
};

/***********************************************************************
 * Main Eye Pattern plotter widget
 **********************************************************************/

EyeDisplayPlot::EyeDisplayPlot(unsigned int nplots,
                               unsigned int curve_index,
                               QWidget* parent)
    : DisplayPlot(1, parent)
{
    d_numPoints = 1024;
    d_nplots = nplots;
    d_numPeriods = std::floor((d_numPoints - 1) / 2 / d_sps);

    d_xdata.resize(d_numPointsPerPeriod);
    d_curve_index = curve_index;

    d_tag_text_color = Qt::black;
    d_tag_background_color = Qt::white;
    d_tag_background_style = Qt::NoBrush;

    d_zoomer = new EyeDisplayZoomer(canvas(), 0);

#if QWT_VERSION < 0x060000
    d_zoomer->setSelectionFlags(QwtPicker::RectSelection | QwtPicker::DragSelection);
#endif

    d_zoomer->setMousePattern(
        QwtEventPattern::MouseSelect2, Qt::RightButton, Qt::ControlModifier);
    d_zoomer->setMousePattern(QwtEventPattern::MouseSelect3, Qt::RightButton);

    const QColor c(Qt::darkRed);
    d_zoomer->setRubberBandPen(c);
    d_zoomer->setTrackerPen(c);

    d_autoscale_shot = false;
    QwtText title;
    title.setText(QString("Eye [Data %1]").arg(d_curve_index));
    setTitle(title);

    setAxisScaleEngine(QwtPlot::xBottom, new QwtLinearScaleEngine);
    setXaxis(-1.0 * d_numPointsPerPeriod, d_numPointsPerPeriod - 1);
    QwtText xAxisTitle(QString("Time (sec)"));
    xAxisTitle.setRenderFlags(Qt::AlignRight | Qt::AlignVCenter);
    setAxisTitle(QwtPlot::xBottom, xAxisTitle);

    setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);
    setYaxis(-2.0, 2.0);
    QwtText yAxisTitle(QString("Amplitude"));
    setAxisTitle(QwtPlot::yLeft, yAxisTitle);

    colors << QColor(Qt::blue) << QColor(Qt::red) << QColor(Qt::green)
           << QColor(Qt::black) << QColor(Qt::cyan) << QColor(Qt::magenta)
           << QColor(Qt::yellow) << QColor(Qt::gray) << QColor(Qt::darkRed)
           << QColor(Qt::darkGreen) << QColor(Qt::darkBlue)
           << QColor(Qt::darkGray)
           // cycle through all colors again to increase eye_sink_f input limit
           // from 12 to 24, otherwise you get a segfault
           << QColor(Qt::blue) << QColor(Qt::red) << QColor(Qt::green)
           << QColor(Qt::black) << QColor(Qt::cyan) << QColor(Qt::magenta)
           << QColor(Qt::yellow) << QColor(Qt::gray) << QColor(Qt::darkRed)
           << QColor(Qt::darkGreen) << QColor(Qt::darkBlue) << QColor(Qt::darkGray);

    // Setup dataPoints and plot vectors
    // Automatically deleted when parent is deleted
    for (unsigned int i = 0; i < d_numPeriods; ++i) {
        d_ydata.emplace_back(d_numPointsPerPeriod);
        d_plot_curve.push_back(
            new QwtPlotCurve(QString("Eye [Data %1]").arg(d_curve_index)));
        d_plot_curve[i]->attach(this);
        d_plot_curve[i]->setPen(QPen(colors[d_curve_index]));
        d_plot_curve[i]->setRenderHint(QwtPlotItem::RenderAntialiased);

        QwtSymbol* symbol = new QwtSymbol(QwtSymbol::NoSymbol,
                                          QBrush(colors[d_curve_index]),
                                          QPen(colors[d_curve_index]),
                                          QSize(7, 7));

#if QWT_VERSION < 0x060000
        d_plot_curve[i]->setRawData(
            d_xdata.data(), d_ydata[i].data(), d_numPointsPerPeriod);
        d_plot_curve[i]->setSymbol(*symbol);
#else
        d_plot_curve[i]->setRawSamples(
            d_xdata.data(), d_ydata[i].data(), d_numPointsPerPeriod);
        d_plot_curve[i]->setSymbol(symbol);
#endif
    }

    d_sample_rate = 1;
    _resetXAxisPoints();

    // d_tag_markers and d_tag_markers are 2 std::vector of size 1 for Eye Plot
    d_tag_markers.resize(1);
    d_tag_markers_en = std::vector<bool>(1, true);

    d_trigger_lines[0] = new QwtPlotMarker();
    d_trigger_lines[0]->setLineStyle(QwtPlotMarker::HLine);
    d_trigger_lines[0]->setLinePen(QPen(Qt::red, 0.6, Qt::DashLine));
    d_trigger_lines[0]->setRenderHint(QwtPlotItem::RenderAntialiased);
    d_trigger_lines[0]->setXValue(0.0);
    d_trigger_lines[0]->setYValue(0.0);

    d_trigger_lines[1] = new QwtPlotMarker();
    d_trigger_lines[1]->setLineStyle(QwtPlotMarker::VLine);
    d_trigger_lines[1]->setLinePen(QPen(Qt::red, 0.6, Qt::DashLine));
    d_trigger_lines[1]->setRenderHint(QwtPlotItem::RenderAntialiased);
    d_trigger_lines[1]->setXValue(0.0);
    d_trigger_lines[1]->setYValue(0.0);
}

EyeDisplayPlot::~EyeDisplayPlot()
{

    // Delete d_ydata set used by this EyeDisplayPlot
    // d_zoomer and _panner deleted when parent deleted
}

void EyeDisplayPlot::replot() { QwtPlot::replot(); }

void EyeDisplayPlot::plotNewData(const std::vector<double*> dataPoints,
                                 const int64_t numDataPoints,
                                 int sps,
                                 const double timeInterval,
                                 const std::vector<std::vector<gr::tag_t>>& tags)
{

    if (!d_stop) {
        if ((numDataPoints > 0)) {
            if ((numDataPoints != d_numPoints) || (d_sps != sps)) {
                // Detach all curves
                for (unsigned int i = 0; i < d_numPeriods; ++i) {
                    d_plot_curve[i]->detach();
                }
                d_numPoints = numDataPoints;
                d_sps = sps;
                d_numPointsPerPeriod = 2 * d_sps + 1;
                d_numPeriods = std::floor((d_numPoints - 1) / 2 / d_sps);

                // clear d_plot_curve, d_xdata, d_ydata
                d_plot_curve.clear();
                d_ydata.clear();
                d_xdata.resize(1 + d_numPointsPerPeriod);
                _resetXAxisPoints();

                // New data structure and data
                for (unsigned int i = 0; i < d_numPeriods; ++i) {
                    int64_t time_index = i * (d_numPointsPerPeriod - 1);
                    d_ydata.emplace_back(
                        &dataPoints[d_curve_index][time_index],
                        &dataPoints[d_curve_index][time_index + d_numPointsPerPeriod]);
                    d_plot_curve.push_back(
                        new QwtPlotCurve(QString("Eye [Data %1]").arg(d_curve_index)));
                    d_plot_curve[i]->attach(this);
                    d_plot_curve[i]->setPen(QPen(colors[d_curve_index]));
                    d_plot_curve[i]->setRenderHint(QwtPlotItem::RenderAntialiased);

                    QwtSymbol* symbol = new QwtSymbol(QwtSymbol::NoSymbol,
                                                      QBrush(colors[d_curve_index]),
                                                      QPen(colors[d_curve_index]),
                                                      QSize(7, 7));

#if QWT_VERSION < 0x060000
                    d_plot_curve[i]->setRawData(
                        d_xdata.data(), d_ydata[i].data(), d_numPointsPerPeriod);
                    d_plot_curve[i]->setSymbol(*symbol);
#else
                    d_plot_curve[i]->setRawSamples(
                        d_xdata.data(), d_ydata[i].data(), d_numPointsPerPeriod);
                    d_plot_curve[i]->setSymbol(symbol);
#endif
                }
            } else {
                // New data
                for (unsigned int i = 0; i < d_numPeriods; ++i) {
                    int64_t time_index = i * (d_numPointsPerPeriod - 1);
                    memcpy(d_ydata[i].data(),
                           &(dataPoints[d_curve_index][time_index]),
                           d_numPointsPerPeriod * sizeof(double));
                }
            }

            // Detach and delete any tags that were plotted last time
            for (size_t i = 0; i < d_tag_markers[0].size(); i++) {
                d_tag_markers[0][i]->detach();
                delete d_tag_markers[0][i];
            }
            d_tag_markers[0].clear();

            // Plot and attach any new tags found.
            // First test if this was a complex input where real/imag get
            // split here into two stream.
            if (!tags.empty()) {
                unsigned int mult = d_nplots / tags.size();
                unsigned int tags_index = d_curve_index / mult;
                std::vector<std::vector<gr::tag_t>>::const_iterator tag = tags.begin();
                // Move iterator to curve[d_curve_index] tags set i.e. tags[tags_index]
                for (unsigned int i = 0; i < tags_index; i++) {
                    tag++;
                }

                std::vector<gr::tag_t>::const_iterator t;
                for (t = tag->begin(); t != tag->end(); t++) {
                    uint64_t offset = (*t).offset;

                    // Ignore tag if its offset is outside our plottable vector.
                    if (offset >= (uint64_t)((d_numPointsPerPeriod - 1) * d_numPeriods)) {
                        continue;
                    }

                    // Adjust the offset in range of the eye pattern length (0 to
                    // d_numPointsPerPeriod - 1)
                    unsigned int period = offset / (d_numPointsPerPeriod - 1);
                    unsigned int eye_offset = offset % (d_numPointsPerPeriod - 1);
                    double sample_offset = double(eye_offset) / d_sample_rate;

                    std::stringstream s;
                    s << (*t).key << ": " << (*t).value;

                    double yval = d_ydata[period][eye_offset];

                    // Find if we already have a marker at this location
                    std::vector<QwtPlotMarker*>::iterator mitr;
                    for (mitr = d_tag_markers[0].begin(); mitr != d_tag_markers[0].end();
                         mitr++) {
                        if ((*mitr)->xValue() == sample_offset) {
                            break;
                        }
                    }
                    // If no matching marker, create a new one
                    if (mitr == d_tag_markers[0].end()) {
                        bool show = d_plot_curve[0]->isVisible();

                        QwtPlotMarker* m = new QwtPlotMarker();
                        m->setXValue(sample_offset);
                        m->setYValue(yval);

                        QBrush brush(getTagBackgroundColor(), getTagBackgroundStyle());

                        QPen pen;
                        pen.setColor(Qt::black);
                        pen.setWidth(1);

                        QwtSymbol* sym =
                            new QwtSymbol(QwtSymbol::NoSymbol, brush, pen, QSize(12, 12));

                        if (yval >= 0) {
                            sym->setStyle(QwtSymbol::DTriangle);
                            m->setLabelAlignment(Qt::AlignTop);
                        } else {
                            sym->setStyle(QwtSymbol::UTriangle);
                            m->setLabelAlignment(Qt::AlignBottom);
                        }

#if QWT_VERSION < 0x060000
                        m->setSymbol(*sym);
#else
                        m->setSymbol(sym);
#endif
                        QwtText tag_label(s.str().c_str());
                        tag_label.setColor(getTagTextColor());
                        m->setLabel(tag_label);

                        m->attach(this);

                        if (!(show && d_tag_markers_en[0])) {
                            m->hide();
                        }

                        d_tag_markers[0].push_back(m);
                    } else {
                        // Prepend the new tag to the existing marker
                        // And set it at the max value
                        if (fabs(yval) < fabs((*mitr)->yValue()))
                            (*mitr)->setYValue(yval);
                        QString orig = (*mitr)->label().text();
                        s << std::endl;
                        orig.prepend(s.str().c_str());

                        QwtText newtext(orig);
                        newtext.setColor(getTagTextColor());

                        QBrush brush(getTagBackgroundColor(), getTagBackgroundStyle());
                        newtext.setBackgroundBrush(brush);

                        (*mitr)->setLabel(newtext);
                    }
                }
            }

            if (d_autoscale_state) {
                double bottom = 1e20, top = -1e20;
                for (unsigned int i = 0; i < d_numPeriods; ++i) {
                    for (int64_t point = 0; point < d_numPointsPerPeriod; point++) {
                        if (d_ydata[i][point] < bottom) {
                            bottom = d_ydata[i][point];
                        }
                        if (d_ydata[i][point] > top) {
                            top = d_ydata[i][point];
                        }
                    }
                }
                _autoScale(bottom, top);
                if (d_autoscale_shot) {
                    d_autoscale_state = false;
                    d_autoscale_shot = false;
                }
            }
            replot();
        }
    }
}

void EyeDisplayPlot::legendEntryChecked(QwtPlotItem* plotItem, bool on)
{
    // When line is turned on/off, immediately show/hide tag markers
    for (size_t i = 0; i < d_tag_markers[0].size(); i++) {
        if (!(!on && d_tag_markers_en[0]))
            d_tag_markers[0][i]->hide();
        else
            d_tag_markers[0][i]->show();
    }
    DisplayPlot::legendEntryChecked(plotItem, on);
}

void EyeDisplayPlot::legendEntryChecked(const QVariant& plotItem, bool on, int index)
{
#if QWT_VERSION < 0x060100
    std::runtime_error("EyeDisplayPlot::legendEntryChecked with QVariant not "
                       "enabled in this version of QWT.");
#else
    QwtPlotItem* p = infoToItem(plotItem);
    legendEntryChecked(p, on);
#endif /* QWT_VERSION < 0x060100 */
}

void EyeDisplayPlot::_resetXAxisPoints()
{
    double delt = 1.0 / d_sample_rate;
    for (int64_t loc = 0; loc < d_numPointsPerPeriod; loc++) {
        d_xdata[loc] = delt * loc;
    }

    setAxisScale(QwtPlot::xBottom, 0, 2.0 * delt * d_sps);
    // Set up zoomer base for maximum unzoom x-axis
    // and reset to maximum unzoom level
    QwtDoubleRect zbase = d_zoomer->zoomBase();
    zbase.setLeft(0.0);
    zbase.setRight(2.0 * delt * d_sps);
    d_zoomer->zoom(zbase);
    d_zoomer->setZoomBase(zbase);
    d_zoomer->zoom(0);
}

void EyeDisplayPlot::_autoScale(double bottom, double top)
{
    // Auto scale the y-axis with a margin of 20% (10 dB for log scale)
    double _bot = bottom - fabs(bottom) * 0.20;
    double _top = top + fabs(top) * 0.20;
    setYaxis(_bot, _top);
}

void EyeDisplayPlot::setAutoScale(bool state) { d_autoscale_state = state; }

void EyeDisplayPlot::setAutoScaleShot()
{
    d_autoscale_state = true;
    d_autoscale_shot = true;
}

void EyeDisplayPlot::setSampleRate(double sr, double units, const std::string& strunits)
{
    double newsr = sr / units;
    if ((newsr != d_sample_rate) ||
        (((EyeDisplayZoomer*)d_zoomer)->unitType() != strunits)) {
        d_sample_rate = sr / units;
        _resetXAxisPoints();

        // While we could change the displayed sigfigs based on the unit being
        // displayed, I think it looks better by just setting it to 4 regardless.
        // double display_units = ceil(log10(units)/2.0);
        double display_units = 4;
        QwtText axisTitle(QString("Time (%1)").arg(strunits.c_str()));
        axisTitle.setRenderFlags(Qt::AlignRight | Qt::AlignVCenter);
        setAxisTitle(QwtPlot::xBottom, axisTitle);
        ((EyeDisplayZoomer*)d_zoomer)->setTimePrecision(display_units);
        ((EyeDisplayZoomer*)d_zoomer)->setUnitType(strunits);
    }
}

double EyeDisplayPlot::sampleRate() const { return d_sample_rate; }

void EyeDisplayPlot::stemPlot(bool en)
{
    for (unsigned int i = 0; i < d_nplots; ++i) {
        d_plot_curve[i]->setStyle(QwtPlotCurve::Lines);
        setLineMarker(i, QwtSymbol::NoSymbol);
    }
}

void EyeDisplayPlot::enableTagMarker(unsigned int which, bool en)
{
    // which always equal 0
    if ((size_t)which < d_tag_markers_en.size())
        d_tag_markers_en[0] = en;
    else
        throw std::runtime_error(
            "TimeDomainDisplayPlot: enabled tag marker does not exist.");
}

const QColor EyeDisplayPlot::getTagTextColor() { return d_tag_text_color; }

const QColor EyeDisplayPlot::getTagBackgroundColor() { return d_tag_background_color; }

const Qt::BrushStyle EyeDisplayPlot::getTagBackgroundStyle()
{
    return d_tag_background_style;
}

void EyeDisplayPlot::setTagTextColor(QColor c) { d_tag_text_color = c; }

void EyeDisplayPlot::setTagBackgroundColor(QColor c) { d_tag_background_color = c; }

void EyeDisplayPlot::setTagBackgroundStyle(Qt::BrushStyle b)
{
    d_tag_background_style = b;
}

void EyeDisplayPlot::setYLabel(const std::string& label, const std::string& unit)
{
    std::string l = label;
    if (unit.length() > 0)
        l += " (" + unit + ")";
    QwtText yAxisTitle(QString(l.c_str()));
    setAxisTitle(QwtPlot::yLeft, yAxisTitle);

    ((EyeDisplayZoomer*)d_zoomer)->setYUnitType(unit);
}

void EyeDisplayPlot::attachTriggerLines(bool en)
{
    if (en) {
        d_trigger_lines[0]->attach(this);
        d_trigger_lines[1]->attach(this);
    } else {
        d_trigger_lines[0]->detach();
        d_trigger_lines[1]->detach();
    }
}

void EyeDisplayPlot::setTriggerLines(double x, double y)
{
    d_trigger_lines[0]->setXValue(x);
    d_trigger_lines[0]->setYValue(y);
    d_trigger_lines[1]->setXValue(x);
    d_trigger_lines[1]->setYValue(y);
}

// DisplayPlot base class methods overriding
void EyeDisplayPlot::setLineLabel(unsigned int which, QString label)
{

    if (which >= d_plot_curve.size())
        throw std::runtime_error("EyeDisplayPlot::setLineLabel: index out of bounds");
    d_plot_curve[which]->setTitle(label);
}

// Overriding of method DisplayPlot::setLineColor
void EyeDisplayPlot::setLineColor(unsigned int which, QColor color)
{
    for (unsigned int i = 0; i < d_plot_curve.size(); ++i) {
        // Set the color of the pen
        QPen pen(d_plot_curve[i]->pen());
        pen.setColor(color);
        d_plot_curve[i]->setPen(pen);
        // And set the color of the markers
#if QWT_VERSION < 0x060000
        d_plot_curve[i]->setPen(pen);
        QwtSymbol sym = (QwtSymbol)d_plot_curve[i]->symbol();
        setLineMarker(i, sym.style());
#else
        QwtSymbol* sym = (QwtSymbol*)d_plot_curve[i]->symbol();
        if (sym) {
            sym->setColor(color);
            sym->setPen(pen);
            d_plot_curve[i]->setSymbol(sym);
        }
#endif
    }
}

// Overriding of method DisplayPlot::setLineWidth
void EyeDisplayPlot::setLineWidth(unsigned int which, int width)
{
    for (unsigned int i = 0; i < d_plot_curve.size(); ++i) {
        // Set the new line width
        QPen pen(d_plot_curve[i]->pen());
        pen.setWidth(width);
        d_plot_curve[i]->setPen(pen);

        // Scale the marker size proportionally
#if QWT_VERSION < 0x060000
        QwtSymbol sym = (QwtSymbol)d_plot_curve[i]->symbol();
        sym.setSize(7 + 10 * log10(1.0 * width), 7 + 10 * log10(1.0 * width));
        d_plot_curve[i]->setSymbol(sym);
#else
        QwtSymbol* sym = (QwtSymbol*)d_plot_curve[i]->symbol();
        if (sym) {
            sym->setSize(7 + 10 * log10(1.0 * i), 7 + 10 * log10(1.0 * i));
            d_plot_curve[i]->setSymbol(sym);
        }
#endif
    }
}

// Overriding of method DisplayPlot::setLineMarker
void EyeDisplayPlot::setLineMarker(unsigned int which, QwtSymbol::Style marker)
{
    for (unsigned int i = 0; i < d_plot_curve.size(); ++i) {
#if QWT_VERSION < 0x060000
        QwtSymbol sym = (QwtSymbol)d_plot_curve[i]->symbol();
        QPen pen(d_plot_curve[i]->pen());
        QBrush brush(pen.color());
        sym.setStyle(marker);
        sym.setPen(pen);
        sym.setBrush(brush);
        d_plot_curve[i]->setSymbol(sym);
#else
        QwtSymbol* sym = (QwtSymbol*)d_plot_curve[i]->symbol();
        if (sym) {
            sym->setStyle(marker);
            d_plot_curve[i]->setSymbol(sym);
        }
#endif
    }
}

// Overriding of method DisplayPlot::setLineStyle
void EyeDisplayPlot::setLineStyle(unsigned int which, Qt::PenStyle style)
{
    for (unsigned int i = 0; i < d_plot_curve.size(); ++i) {
        QPen pen(d_plot_curve[i]->pen());
        pen.setStyle(style);
        d_plot_curve[i]->setPen(pen);
    }
}

// Overriding of method DisplayPlot::setMarkerAlpha
void EyeDisplayPlot::setMarkerAlpha(unsigned int which, int alpha)
{
    for (unsigned int i = 0; i < d_plot_curve.size(); ++i) {
        // Get the pen color
        QPen pen(d_plot_curve[i]->pen());
        QColor color = pen.color();

        // Set new alpha and update pen
        color.setAlpha(alpha);
        pen.setColor(color);
        d_plot_curve[i]->setPen(pen);

        // And set the new color for the markers
#if QWT_VERSION < 0x060000
        QwtSymbol sym = (QwtSymbol)d_plot_curve[i]->symbol();
        setLineMarker(i, sym.style());
#else
        QwtSymbol* sym = (QwtSymbol*)d_plot_curve[i]->symbol();
        if (sym) {
            sym->setColor(color);
            sym->setPen(pen);
            d_plot_curve[i]->setSymbol(sym);
        }
#endif
    }
}

#endif /* EYE_DISPLAY_PLOT_C */

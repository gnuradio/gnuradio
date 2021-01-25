/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <gnuradio/qtgui/DisplayPlot.h>

#include <qwt_legend.h>
#include <qwt_scale_draw.h>
#include <QColor>
#include <QDebug>
#include <cmath>
#include <iostream>
#include <stdexcept>

DisplayPlot::DisplayPlot(int nplots, QWidget* parent)
    : QwtPlot(parent), d_nplots(nplots), d_stop(false)
{
    qRegisterMetaType<QColorList>("QColorList");
    resize(parent->width(), parent->height());

    d_autoscale_state = false;

    // Disable polygon clipping
#if QWT_VERSION < 0x060000
    QwtPainter::setDeviceClipping(false);
#else
    QwtPainter::setPolylineSplitting(false);
#endif

#if QWT_VERSION < 0x060000
    // We don't need the cache here
    canvas()->setPaintAttribute(QwtPlotCanvas::PaintCached, false);
    canvas()->setPaintAttribute(QwtPlotCanvas::PaintPacked, false);
#endif

    QColor default_palette_color = QColor("white");
    setPaletteColor(default_palette_color);

    d_panner = new QwtPlotPanner(canvas());
    d_panner->setAxisEnabled(QwtPlot::yRight, false);
    d_panner->setMouseButton(Qt::MiddleButton, Qt::ControlModifier);

    // emit the position of clicks on widget
    d_picker = new QwtDblClickPlotPicker(canvas());

#if QWT_VERSION < 0x060000
    connect(d_picker,
            SIGNAL(selected(const QwtDoublePoint&)),
            this,
            SLOT(onPickerPointSelected(const QwtDoublePoint&)));
#else
    d_picker->setStateMachine(new QwtPickerDblClickPointMachine());
    connect(d_picker,
            SIGNAL(selected(const QPointF&)),
            this,
            SLOT(onPickerPointSelected6(const QPointF&)));
#endif

    // Configure magnify on mouse wheel
    d_magnifier = new QwtPlotMagnifier(canvas());
    d_magnifier->setAxisEnabled(QwtPlot::xBottom, false);

    // Avoid jumping when labels with more/less digits
    // appear/disappear when scrolling vertically

    const QFontMetrics fm(axisWidget(QwtPlot::yLeft)->font());
    QwtScaleDraw* sd = axisScaleDraw(QwtPlot::yLeft);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
    int min_ext = fm.horizontalAdvance("100.00");
#else
    int min_ext = fm.width("100.00");
#endif
    sd->setMinimumExtent(min_ext);

    QwtLegend* legendDisplay = new QwtLegend(this);

#if QWT_VERSION < 0x060100
    legendDisplay->setItemMode(QwtLegend::CheckableItem);
    insertLegend(legendDisplay);
    connect(this,
            SIGNAL(legendChecked(QwtPlotItem*, bool)),
            this,
            SLOT(legendEntryChecked(QwtPlotItem*, bool)));
#else  /* QWT_VERSION < 0x060100 */
    legendDisplay->setDefaultItemMode(QwtLegendData::Checkable);
    insertLegend(legendDisplay);
    connect(legendDisplay,
            SIGNAL(checked(const QVariant&, bool, int)),
            this,
            SLOT(legendEntryChecked(const QVariant&, bool, int)));
#endif /* QWT_VERSION < 0x060100 */
}

DisplayPlot::~DisplayPlot()
{
    // d_zoomer and d_panner deleted when parent deleted
}

void DisplayPlot::disableLegend()
{
    // Haven't found a good way to toggle it on/off
    insertLegend(NULL);
}

void DisplayPlot::setYaxis(double min, double max)
{
    setAxisScale(QwtPlot::yLeft, min, max);
    if (!d_autoscale_state)
        d_zoomer->setZoomBase();
}

void DisplayPlot::setXaxis(double min, double max)
{
    setAxisScale(QwtPlot::xBottom, min, max);
    d_zoomer->setZoomBase();
}

void DisplayPlot::setLineLabel(unsigned int which, QString label)
{
    if (which >= d_plot_curve.size())
        throw std::runtime_error("DisplayPlot::setLineLabel: index out of bounds");
    d_plot_curve[which]->setTitle(label);
}

QString DisplayPlot::getLineLabel(unsigned int which)
{
    if (which >= d_plot_curve.size())
        throw std::runtime_error("DisplayPlot::getLineLabel: index out of bounds");
    return d_plot_curve[which]->title().text();
}

void DisplayPlot::setLineColor(unsigned int which, QColor color)
{
    if (which < d_nplots) {
        // Set the color of the pen
        QPen pen(d_plot_curve[which]->pen());
        pen.setColor(color);
        d_plot_curve[which]->setPen(pen);
        // And set the color of the markers
#if QWT_VERSION < 0x060000
        // d_plot_curve[which]->setBrush(QBrush(QColor(color)));
        d_plot_curve[which]->setPen(pen);
        QwtSymbol sym = (QwtSymbol)d_plot_curve[which]->symbol();
        setLineMarker(which, sym.style());
#else
        QwtSymbol* sym = (QwtSymbol*)d_plot_curve[which]->symbol();
        if (sym) {
            sym->setColor(color);
            sym->setPen(pen);
            d_plot_curve[which]->setSymbol(sym);
        }
#endif
    }
}

QColor DisplayPlot::getLineColor(unsigned int which) const
{
    // If that plot doesn't exist then return black.
    if (which < d_nplots)
        return d_plot_curve[which]->pen().color();
    return QColor("black");
}

// Use a preprocessor macro to create a bunch of hooks for Q_PROPERTY and hence the
// stylesheet.
#define SETUPLINE(i, im1)                                                               \
    void DisplayPlot::setLineColor##i(QColor c) { setLineColor(im1, c); }               \
    const QColor DisplayPlot::getLineColor##i() const { return getLineColor(im1); }     \
    void DisplayPlot::setLineWidth##i(int width) { setLineWidth(im1, width); }          \
    int DisplayPlot::getLineWidth##i() const { return getLineWidth(im1); }              \
    void DisplayPlot::setLineStyle##i(Qt::PenStyle ps) { setLineStyle(im1, ps); }       \
    const Qt::PenStyle DisplayPlot::getLineStyle##i() const                             \
    {                                                                                   \
        return getLineStyle(im1);                                                       \
    }                                                                                   \
    void DisplayPlot::setLineMarker##i(QwtSymbol::Style ms) { setLineMarker(im1, ms); } \
    const QwtSymbol::Style DisplayPlot::getLineMarker##i() const                        \
    {                                                                                   \
        return getLineMarker(im1);                                                      \
    }                                                                                   \
    void DisplayPlot::setMarkerAlpha##i(int alpha) { setMarkerAlpha(im1, alpha); }      \
    int DisplayPlot::getMarkerAlpha##i() const { return getMarkerAlpha(im1); }
SETUPLINE(1, 0)
SETUPLINE(2, 1)
SETUPLINE(3, 2)
SETUPLINE(4, 3)
SETUPLINE(5, 4)
SETUPLINE(6, 5)
SETUPLINE(7, 6)
SETUPLINE(8, 7)
SETUPLINE(9, 8)

void DisplayPlot::setZoomerColor(QColor c)
{
    d_zoomer->setRubberBandPen(c);
    d_zoomer->setTrackerPen(c);
}

QColor DisplayPlot::getZoomerColor() const { return d_zoomer->rubberBandPen().color(); }

void DisplayPlot::setPaletteColor(QColor c)
{
    QPalette palette;
    palette.setColor(canvas()->backgroundRole(), c);
    canvas()->setPalette(palette);
}

QColor DisplayPlot::getPaletteColor() const
{
    return canvas()->palette().color(canvas()->backgroundRole());
}

void DisplayPlot::setAxisLabelFontSize(int axisId, int fs)
{
    QwtText axis_title = QwtText(axisWidget(axisId)->title());
    QFont font = QFont(axis_title.font());
    font.setPointSize(fs);
    axis_title.setFont(font);
    axisWidget(axisId)->setTitle(axis_title);
}

int DisplayPlot::getAxisLabelFontSize(int axisId) const
{
    return axisWidget(axisId)->title().font().pointSize();
}

void DisplayPlot::setYaxisLabelFontSize(int fs)
{
    setAxisLabelFontSize(QwtPlot::yLeft, fs);
}

int DisplayPlot::getYaxisLabelFontSize() const
{
    int fs = getAxisLabelFontSize(QwtPlot::yLeft);
    return fs;
}

void DisplayPlot::setXaxisLabelFontSize(int fs)
{
    setAxisLabelFontSize(QwtPlot::xBottom, fs);
}

int DisplayPlot::getXaxisLabelFontSize() const
{
    int fs = getAxisLabelFontSize(QwtPlot::xBottom);
    return fs;
}

void DisplayPlot::setAxesLabelFontSize(int fs)
{
    setAxisLabelFontSize(QwtPlot::yLeft, fs);
    setAxisLabelFontSize(QwtPlot::xBottom, fs);
}

int DisplayPlot::getAxesLabelFontSize() const
{
    // Returns 0 if all axes do not have the same font size.
    int fs = getAxisLabelFontSize(QwtPlot::yLeft);
    if (getAxisLabelFontSize(QwtPlot::xBottom) != fs)
        return 0;
    return fs;
}

void DisplayPlot::setLineWidth(unsigned int which, int width)
{
    if (which < d_nplots) {
        // Set the new line width
        QPen pen(d_plot_curve[which]->pen());
        pen.setWidth(width);
        d_plot_curve[which]->setPen(pen);

        // Scale the marker size proportionally
#if QWT_VERSION < 0x060000
        QwtSymbol sym = (QwtSymbol)d_plot_curve[which]->symbol();
        sym.setSize(7 + 10 * log10(1.0 * width), 7 + 10 * log10(1.0 * width));
        d_plot_curve[which]->setSymbol(sym);
#else
        QwtSymbol* sym = (QwtSymbol*)d_plot_curve[which]->symbol();
        if (sym) {
            sym->setSize(7 + 10 * log10(1.0 * width), 7 + 10 * log10(1.0 * width));
            d_plot_curve[which]->setSymbol(sym);
        }
#endif
    }
}

int DisplayPlot::getLineWidth(unsigned int which) const
{
    if (which < d_nplots) {
        return d_plot_curve[which]->pen().width();
    } else {
        return 0;
    }
}

void DisplayPlot::setLineStyle(unsigned int which, Qt::PenStyle style)
{
    if (which < d_nplots) {
        QPen pen(d_plot_curve[which]->pen());
        pen.setStyle(style);
        d_plot_curve[which]->setPen(pen);
    }
}

const Qt::PenStyle DisplayPlot::getLineStyle(unsigned int which) const
{
    if (which < d_nplots) {
        return d_plot_curve[which]->pen().style();
    } else {
        return Qt::SolidLine;
    }
}

void DisplayPlot::setLineMarker(unsigned int which, QwtSymbol::Style marker)
{
    if (which < d_nplots) {
#if QWT_VERSION < 0x060000
        QwtSymbol sym = (QwtSymbol)d_plot_curve[which]->symbol();
        QPen pen(d_plot_curve[which]->pen());
        QBrush brush(pen.color());
        sym.setStyle(marker);
        sym.setPen(pen);
        sym.setBrush(brush);
        d_plot_curve[which]->setSymbol(sym);
#else
        QwtSymbol* sym = (QwtSymbol*)d_plot_curve[which]->symbol();
        if (sym) {
            sym->setStyle(marker);
            d_plot_curve[which]->setSymbol(sym);
        }
#endif
    }
}

const QwtSymbol::Style DisplayPlot::getLineMarker(unsigned int which) const
{
    if (which < d_nplots) {
#if QWT_VERSION < 0x060000
        QwtSymbol sym = (QwtSymbol)d_plot_curve[which]->symbol();
        return sym.style();
#else
        QwtSymbol* sym = (QwtSymbol*)d_plot_curve[which]->symbol();
        return sym->style();
#endif
    } else {
        return QwtSymbol::NoSymbol;
    }
}

void DisplayPlot::setMarkerAlpha(unsigned int which, int alpha)
{
    if (which < d_nplots) {
        // Get the pen color
        QPen pen(d_plot_curve[which]->pen());
        QColor color = pen.color();

        // Set new alpha and update pen
        color.setAlpha(alpha);
        pen.setColor(color);
        d_plot_curve[which]->setPen(pen);

        // And set the new color for the markers
#if QWT_VERSION < 0x060000
        QwtSymbol sym = (QwtSymbol)d_plot_curve[which]->symbol();
        setLineMarker(which, sym.style());
#else
        QwtSymbol* sym = (QwtSymbol*)d_plot_curve[which]->symbol();
        if (sym) {
            sym->setColor(color);
            sym->setPen(pen);
            d_plot_curve[which]->setSymbol(sym);
        }
#endif
    }
}

int DisplayPlot::getMarkerAlpha(unsigned int which) const
{
    if (which < d_nplots) {
        return d_plot_curve[which]->pen().color().alpha();
    } else {
        return 0;
    }
}

void DisplayPlot::setStop(bool on) { d_stop = on; }

void DisplayPlot::resizeSlot(QSize* s)
{
    // -10 is to spare some room for the legend and x-axis label
    resize(s->width() - 10, s->height() - 10);
}

void DisplayPlot::legendEntryChecked(QwtPlotItem* plotItem, bool on)
{
    plotItem->setVisible(!on);
    replot();
}

void DisplayPlot::legendEntryChecked(const QVariant& plotItem, bool on, int index)
{
#if QWT_VERSION < 0x060100
    std::runtime_error("DisplayPlot::legendEntryChecked with QVariant not enabled in "
                       "this version of QWT.");
#else
    QwtPlotItem* p = infoToItem(plotItem);
    legendEntryChecked(p, on);
#endif /* QWT_VERSION < 0x060100 */
}

void DisplayPlot::onPickerPointSelected(const QwtDoublePoint& p)
{
    QPointF point = p;
    // fprintf(stderr,"onPickerPointSelected %f %f\n", point.x(), point.y());
    emit plotPointSelected(point);
}

void DisplayPlot::onPickerPointSelected6(const QPointF& p)
{
    QPointF point = p;
    // fprintf(stderr,"onPickerPointSelected %f %f\n", point.x(), point.y());
    emit plotPointSelected(point);
}

void DisplayPlot::setAxisLabels(bool en)
{
    enableAxis(0, en);
    enableAxis(2, en);
}

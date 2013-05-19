/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include <gnuradio/qtgui/DisplayPlot.h>

#include <qwt_scale_draw.h>
#include <qwt_legend.h>
#include <QColor>
#include <cmath>
#include <iostream>
#include <QDebug>

DisplayPlot::DisplayPlot(int nplots, QWidget* parent)
  : QwtPlot(parent), _nplots(nplots), _stop(false)
{
  qRegisterMetaType<QColorList>("QColorList");
  resize(parent->width(), parent->height());

  _autoscale_state = false;

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

  _panner = new QwtPlotPanner(canvas());
  _panner->setAxisEnabled(QwtPlot::yRight, false);
  _panner->setMouseButton(Qt::MidButton, Qt::ControlModifier);

  // emit the position of clicks on widget
  _picker = new QwtDblClickPlotPicker(canvas());

#if QWT_VERSION < 0x060000
  connect(_picker, SIGNAL(selected(const QwtDoublePoint &)),
      this, SLOT(onPickerPointSelected(const QwtDoublePoint &)));
#else
  _picker->setStateMachine(new QwtPickerDblClickPointMachine());
  connect(_picker, SIGNAL(selected(const QPointF &)),
	  this, SLOT(onPickerPointSelected6(const QPointF &)));
#endif

  // Configure magnify on mouse wheel
  _magnifier = new QwtPlotMagnifier(canvas());
  _magnifier->setAxisEnabled(QwtPlot::xBottom, false);

  // Avoid jumping when labels with more/less digits
  // appear/disappear when scrolling vertically

  const QFontMetrics fm(axisWidget(QwtPlot::yLeft)->font());
  QwtScaleDraw *sd = axisScaleDraw(QwtPlot::yLeft);
  sd->setMinimumExtent( fm.width("100.00") );

  QwtLegend* legendDisplay = new QwtLegend(this);
  legendDisplay->setItemMode(QwtLegend::CheckableItem);
  insertLegend(legendDisplay);

  connect(this, SIGNAL(legendChecked(QwtPlotItem *, bool)),
	  this, SLOT(legendEntryChecked(QwtPlotItem *, bool)));
}

DisplayPlot::~DisplayPlot()
{
  // _zoomer and _panner deleted when parent deleted
}


void
DisplayPlot::setYaxis(double min, double max)
{
  setAxisScale(QwtPlot::yLeft, min, max);
  if(!_autoscale_state)
    _zoomer->setZoomBase();
}

void
DisplayPlot::setXaxis(double min, double max)
{
  setAxisScale(QwtPlot::xBottom, min, max);
  _zoomer->setZoomBase();
}

void
DisplayPlot::setLineLabel(int which, QString label)
{
  _plot_curve[which]->setTitle(label);
}

QString
DisplayPlot::getLineLabel(int which)
{
  return _plot_curve[which]->title().text();
}

void
DisplayPlot::setLineColor(int which, QColor color)
{
  if (which < _nplots) {
    // Set the color of the pen
    QPen pen(_plot_curve[which]->pen());
    pen.setColor(color);
    _plot_curve[which]->setPen(pen);
    // And set the color of the markers
#if QWT_VERSION < 0x060000
    //_plot_curve[which]->setBrush(QBrush(QColor(color)));
    _plot_curve[which]->setPen(pen);
    QwtSymbol sym = (QwtSymbol)_plot_curve[which]->symbol();
    setLineMarker(which, sym.style());
#else
    QwtSymbol *sym = (QwtSymbol*)_plot_curve[which]->symbol();
    if(sym) {
      sym->setColor(color);
      sym->setPen(pen);
      _plot_curve[which]->setSymbol(sym);
    }
#endif
  }
}

QColor
DisplayPlot::getLineColor(int which) const {
  // If that plot doesn't exist then return black.
  if (which < _nplots)
    return _plot_curve[which]->pen().color();
  return QColor("black");
}

// Use a preprocessor macro to create a bunch of hooks for Q_PROPERTY and hence the stylesheet.
#define SETUPLINE(i, im1) \
    void DisplayPlot::setLineColor ## i (QColor c) {setLineColor(im1, c);} \
    const QColor DisplayPlot::getLineColor ## i () const {return getLineColor(im1);} \
    void DisplayPlot::setLineWidth ## i (int width) {setLineWidth(im1, width);} \
    int DisplayPlot::getLineWidth ## i () const {return getLineWidth(im1);} \
    void DisplayPlot::setLineStyle ## i (Qt::PenStyle ps) {setLineStyle(im1, ps);} \
    const Qt::PenStyle DisplayPlot::getLineStyle ## i () const {return getLineStyle(im1);} \
    void DisplayPlot::setLineMarker ## i (QwtSymbol::Style ms) {setLineMarker(im1, ms);} \
    const QwtSymbol::Style DisplayPlot::getLineMarker ## i () const {return getLineMarker(im1);} \
    void DisplayPlot::setMarkerAlpha ## i (int alpha) {setMarkerAlpha(im1, alpha);} \
    int DisplayPlot::getMarkerAlpha ## i () const {return getMarkerAlpha(im1);}
SETUPLINE(1, 0)
SETUPLINE(2, 1)
SETUPLINE(3, 2)
SETUPLINE(4, 3)
SETUPLINE(5, 4)
SETUPLINE(6, 5)
SETUPLINE(7, 6)
SETUPLINE(8, 7)
SETUPLINE(9, 8)

void
DisplayPlot::setZoomerColor(QColor c) {
  _zoomer->setRubberBandPen(c);
  _zoomer->setTrackerPen(c);
}

QColor
DisplayPlot::getZoomerColor() const {
  return _zoomer->rubberBandPen().color();
}

void
DisplayPlot::setPaletteColor(QColor c) {
  QPalette palette;
  palette.setColor(canvas()->backgroundRole(), c);
  canvas()->setPalette(palette);
}

QColor
DisplayPlot::getPaletteColor() const {
  return canvas()->palette().color(canvas()->backgroundRole());
}

void
DisplayPlot::setAxisLabelFontSize(int axisId, int fs) {
  QwtText axis_title = QwtText(axisWidget(axisId)->title());
  QFont font = QFont(axis_title.font());
  font.setPointSize(fs);
  axis_title.setFont(font);
  axisWidget(axisId)->setTitle(axis_title);
}

int
DisplayPlot::getAxisLabelFontSize(int axisId) const {
  return axisWidget(axisId)->title().font().pointSize();
}

void
DisplayPlot::setYaxisLabelFontSize(int fs) {
  setAxisLabelFontSize(QwtPlot::yLeft, fs);
}

int
DisplayPlot::getYaxisLabelFontSize() const {
  int fs = getAxisLabelFontSize(QwtPlot::yLeft);
  return fs;
}

void
DisplayPlot::setXaxisLabelFontSize(int fs) {
  setAxisLabelFontSize(QwtPlot::xBottom, fs);
}

int
DisplayPlot::getXaxisLabelFontSize() const {
  int fs = getAxisLabelFontSize(QwtPlot::xBottom);
  return fs;
}

void
DisplayPlot::setAxesLabelFontSize(int fs) {
  setAxisLabelFontSize(QwtPlot::yLeft, fs);
  setAxisLabelFontSize(QwtPlot::xBottom, fs);
}

int
DisplayPlot::getAxesLabelFontSize() const {
  // Returns 0 if all axes do not have the same font size.
  int fs = getAxisLabelFontSize(QwtPlot::yLeft);
  if (getAxisLabelFontSize(QwtPlot::xBottom) != fs)
    return 0;
  return fs;
}

void
DisplayPlot::setLineWidth(int which, int width)
{
  if (which < _nplots) {
    // Set the new line width
    QPen pen(_plot_curve[which]->pen());
    pen.setWidth(width);
    _plot_curve[which]->setPen(pen);
    
    // Scale the marker size proportionally
#if QWT_VERSION < 0x060000
    QwtSymbol sym = (QwtSymbol)_plot_curve[which]->symbol();
    sym.setSize(7+10*log10(1.0*width), 7+10*log10(1.0*width));
    _plot_curve[which]->setSymbol(sym);
#else
    QwtSymbol *sym = (QwtSymbol*)_plot_curve[which]->symbol();
    if(sym) {
      sym->setSize(7+10*log10(1.0*width), 7+10*log10(1.0*width));
      _plot_curve[which]->setSymbol(sym);
    }
#endif
  }
}

int
DisplayPlot::getLineWidth(int which) const {
  if (which < _nplots) {
    return _plot_curve[which]->pen().width();
  } else {
    return 0;
  }
}

void
DisplayPlot::setLineStyle(int which, Qt::PenStyle style)
{
  if (which < _nplots) {
    QPen pen(_plot_curve[which]->pen());
    pen.setStyle(style);
    _plot_curve[which]->setPen(pen);
  }
}

const Qt::PenStyle
DisplayPlot::getLineStyle(int which) const
{
  if (which < _nplots) {
    return _plot_curve[which]->pen().style();
  } else {
    return Qt::SolidLine;
  }
}

void
DisplayPlot::setLineMarker(int which, QwtSymbol::Style marker)
{
  if (which < _nplots) {
#if QWT_VERSION < 0x060000
    QwtSymbol sym = (QwtSymbol)_plot_curve[which]->symbol();
    QPen pen(_plot_curve[which]->pen());
    QBrush brush(pen.color());
    sym.setStyle(marker);
    sym.setPen(pen);
    sym.setBrush(brush);
    _plot_curve[which]->setSymbol(sym);
#else
    QwtSymbol *sym = (QwtSymbol*)_plot_curve[which]->symbol();
    if(sym) {
      sym->setStyle(marker);
      _plot_curve[which]->setSymbol(sym);
    }
#endif
  }
}

const QwtSymbol::Style
DisplayPlot::getLineMarker(int which) const
{
  if(which < _nplots) {
#if QWT_VERSION < 0x060000
    QwtSymbol sym = (QwtSymbol)_plot_curve[which]->symbol();
    return sym.style();
#else
    QwtSymbol *sym = (QwtSymbol*)_plot_curve[which]->symbol();
    return sym->style();
#endif
  }
  else {
    return QwtSymbol::NoSymbol;
  }
}

void
DisplayPlot::setMarkerAlpha(int which, int alpha)
{
  if (which < _nplots) {
    // Get the pen color
    QPen pen(_plot_curve[which]->pen());
    QColor color = pen.color();
    
    // Set new alpha and update pen
    color.setAlpha(alpha);
    pen.setColor(color);
    _plot_curve[which]->setPen(pen);
    
    // And set the new color for the markers
#if QWT_VERSION < 0x060000
    QwtSymbol sym = (QwtSymbol)_plot_curve[which]->symbol();
    setLineMarker(which, sym.style());
#else
    QwtSymbol *sym = (QwtSymbol*)_plot_curve[which]->symbol();
    if(sym) {
      sym->setColor(color);
      sym->setPen(pen);
      _plot_curve[which]->setSymbol(sym);
    }
#endif
  }
}

int
DisplayPlot::getMarkerAlpha(int which) const
{
  if (which < _nplots) {
    return _plot_curve[which]->pen().color().alpha();
  } else {
    return 0;
  }
}

void
DisplayPlot::setStop(bool on)
{
  _stop = on;  
}

void
DisplayPlot::resizeSlot( QSize *s )
{
  // -10 is to spare some room for the legend and x-axis label
  resize(s->width()-10, s->height()-10);
}

void DisplayPlot::legendEntryChecked(QwtPlotItem* plotItem, bool on)
{
  plotItem->setVisible(!on);
  replot();
}

void
DisplayPlot::onPickerPointSelected(const QwtDoublePoint & p)
{
  QPointF point = p;
  //fprintf(stderr,"onPickerPointSelected %f %f\n", point.x(), point.y());
  emit plotPointSelected(point);
}

void
DisplayPlot::onPickerPointSelected6(const QPointF & p)
{
  QPointF point = p;
  //fprintf(stderr,"onPickerPointSelected %f %f\n", point.x(), point.y());
  emit plotPointSelected(point);
}

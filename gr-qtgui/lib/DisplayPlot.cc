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

#include <DisplayPlot.h>

#include <qwt_scale_draw.h>
#include <qwt_legend.h>
#include <QColor>
#include <cmath>
#include <iostream>

DisplayPlot::DisplayPlot(int nplots, QWidget* parent)
  : QwtPlot(parent), _nplots(nplots), _stop(false)
{
  resize(parent->width(), parent->height());

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

  QPalette palette;
  palette.setColor(canvas()->backgroundRole(), QColor("white"));
  canvas()->setPalette(palette);

  _panner = new QwtPlotPanner(canvas());
  _panner->setAxisEnabled(QwtPlot::yRight, false);
  _panner->setMouseButton(Qt::MidButton);

  // emit the position of clicks on widget
  _picker = new QwtDblClickPlotPicker(canvas());

#if QWT_VERSION < 0x060000
  connect(_picker, SIGNAL(selected(const QwtDoublePoint &)),
	  this, SLOT(OnPickerPointSelected(const QwtDoublePoint &)));
#else
  _picker->setStateMachine(new QwtPickerDblClickPointMachine());
  connect(_picker, SIGNAL(selected(const QPointF &)),
	  this, SLOT(OnPickerPointSelected6(const QPointF &)));
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
	  this, SLOT(LegendEntryChecked(QwtPlotItem *, bool)));
}

DisplayPlot::~DisplayPlot()
{
  // _zoomer and _panner deleted when parent deleted
}

void
DisplayPlot::setYaxis(double min, double max)
{
  setAxisScale(QwtPlot::yLeft, min, max);
  _zoomer->setZoomBase();
}

void
DisplayPlot::setXaxis(double min, double max)
{
  setAxisScale(QwtPlot::xBottom, min, max);
  _zoomer->setZoomBase();
}

void
DisplayPlot::setTitle(int which, QString title)
{
  _plot_curve[which]->setTitle(title);
}

QString
DisplayPlot::title(int which)
{
  return _plot_curve[which]->title().text();
}

void
DisplayPlot::setColor(int which, QString color)
{
  // Set the color of the pen
  QPen pen(_plot_curve[which]->pen());
  pen.setColor(color);
  _plot_curve[which]->setPen(pen);

  // And set the color of the markers
#if QWT_VERSION < 0x060000
  _plot_curve[which]->setBrush(QBrush(QColor(color)));
#else
  QwtSymbol *sym = (QwtSymbol*)_plot_curve[which]->symbol();
  sym->setColor(color);
  sym->setPen(pen);
  _plot_curve[which]->setSymbol(sym);
#endif
}

void
DisplayPlot::setLineWidth(int which, int width)
{
  // Set the new line width
  QPen pen(_plot_curve[which]->pen());
  pen.setWidth(width);
  _plot_curve[which]->setPen(pen);

  // Scale the marker size proportionally
#if QWT_VERSION < 0x060000
  QwtSymbol *sym = (QwtSymbol*)&_plot_curve[which]->symbol();
  sym->setSize(7+10*log10(width), 7+10*log10(width));
  _plot_curve[which]->setSymbol(*sym);
#else
  QwtSymbol *sym = (QwtSymbol*)_plot_curve[which]->symbol();
  sym->setSize(7+10*log10(width), 7+10*log10(width));
  _plot_curve[which]->setSymbol(sym);
#endif
}

void
DisplayPlot::setLineStyle(int which, Qt::PenStyle style)
{
  QPen pen(_plot_curve[which]->pen());
  pen.setStyle(style);
  _plot_curve[which]->setPen(pen);
}

void
DisplayPlot::setLineMarker(int which, QwtSymbol::Style marker)
{
#if QWT_VERSION < 0x060000
  QwtSymbol sym = (QwtSymbol)_plot_curve[which]->symbol();
  sym.setStyle(marker);
  _plot_curve[which]->setSymbol(sym);
#else
  QwtSymbol *sym = (QwtSymbol*)_plot_curve[which]->symbol();
  sym->setStyle(marker);
  _plot_curve[which]->setSymbol(sym);
#endif
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

void DisplayPlot::LegendEntryChecked(QwtPlotItem* plotItem, bool on)
{
  plotItem->setVisible(!on);
  replot();
}

void
DisplayPlot::OnPickerPointSelected(const QwtDoublePoint & p)
{
  QPointF point = p;
  //fprintf(stderr,"OnPickerPointSelected %f %f\n", point.x(), point.y());
  emit plotPointSelected(point);
}

void
DisplayPlot::OnPickerPointSelected6(const QPointF & p)
{
  QPointF point = p;
  //fprintf(stderr,"OnPickerPointSelected %f %f\n", point.x(), point.y());
  emit plotPointSelected(point);
}

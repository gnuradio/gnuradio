/* -*- c++ -*- */
/*
 * Copyright 2008-2012 Free Software Foundation, Inc.
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

#ifndef CONSTELLATION_DISPLAY_PLOT_C
#define CONSTELLATION_DISPLAY_PLOT_C

#include <ConstellationDisplayPlot.h>

#include <qwt_scale_draw.h>
#include <qwt_legend.h>
#include <QColor>
#include <iostream>

class ConstellationDisplayZoomer: public QwtPlotZoomer
{
public:
  ConstellationDisplayZoomer(QwtPlotCanvas* canvas):QwtPlotZoomer(canvas)
  {
    setTrackerMode(QwtPicker::AlwaysOn);
  }

  virtual ~ConstellationDisplayZoomer(){

  }

  virtual void updateTrackerText(){
    updateDisplay();
  }

protected:
  using QwtPlotZoomer::trackerText;
  virtual QwtText trackerText( const QPoint& p ) const
  {
    QwtDoublePoint dp = QwtPlotZoomer::invTransform(p);
    QwtText t(QString("(%1, %2)").arg(dp.x(), 0, 'f', 4).
	      arg(dp.y(), 0, 'f', 4));
    return t;
  }
};

ConstellationDisplayPlot::ConstellationDisplayPlot(int nplots, QWidget* parent)
  : DisplayPlot(nplots, parent)
{
  resize(parent->width(), parent->height());

  _numPoints = 1024;
  _penSize = 5;

  setAxisScaleEngine(QwtPlot::xBottom, new QwtLinearScaleEngine);
  set_xaxis(-2.0, 2.0);
  setAxisTitle(QwtPlot::xBottom, "In-phase");

  setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);
  set_yaxis(-2.0, 2.0);
  setAxisTitle(QwtPlot::yLeft, "Quadrature");

  _zoomer = new ConstellationDisplayZoomer(canvas());

#if QWT_VERSION < 0x060000
  _zoomer->setSelectionFlags(QwtPicker::RectSelection | QwtPicker::DragSelection);
#endif

  _zoomer->setMousePattern(QwtEventPattern::MouseSelect2,
			   Qt::RightButton, Qt::ControlModifier);
  _zoomer->setMousePattern(QwtEventPattern::MouseSelect3,
			   Qt::RightButton);

  const QColor c(Qt::darkRed);
  _zoomer->setRubberBandPen(c);
  _zoomer->setTrackerPen(c);

  QList<QColor> colors;
  colors << QColor(Qt::blue) << QColor(Qt::red) << QColor(Qt::green)
	 << QColor(Qt::black) << QColor(Qt::cyan) << QColor(Qt::magenta)
	 << QColor(Qt::yellow) << QColor(Qt::gray) << QColor(Qt::darkRed)
	 << QColor(Qt::darkGreen) << QColor(Qt::darkBlue) << QColor(Qt::darkGray);

  // Setup dataPoints and plot vectors
  // Automatically deleted when parent is deleted
  for(int i = 0; i < _nplots; i++) {
    _realDataPoints.push_back(new double[_numPoints]);
    _imagDataPoints.push_back(new double[_numPoints]);
    memset(_realDataPoints[i], 0x0, _numPoints*sizeof(double));
    memset(_imagDataPoints[i], 0x0, _numPoints*sizeof(double));

    _plot_curve.push_back(new QwtPlotCurve(QString("Data %1").arg(i)));
    _plot_curve[i]->attach(this);
    _plot_curve[i]->setPen(QPen(colors[i]));
    
    QwtSymbol *symbol = new QwtSymbol(QwtSymbol::NoSymbol);
    symbol->setPen(QPen(colors[i]));
    symbol->setColor(colors[i]);
    symbol->setSize(7, 7);
    _plot_curve[i]->setSymbol(symbol);

    setLineStyle(i, Qt::NoPen);
    setLineMarker(i, QwtSymbol::Ellipse);

#if QWT_VERSION < 0x060000
    _plot_curve[i]->setRawData(_realDataPoints[i], _imagDataPoints[i], _numPoints);
#else
    _plot_curve[i]->setRawSamples(_realDataPoints[i], _imagDataPoints[i], _numPoints);
#endif
  }
}

ConstellationDisplayPlot::~ConstellationDisplayPlot()
{
  for(int i = 0; i < _nplots; i++) {
    delete [] _realDataPoints[i];
    delete [] _imagDataPoints[i];
  }

  // _fft_plot_curves deleted when parent deleted
  // _zoomer and _panner deleted when parent deleted
}

void
ConstellationDisplayPlot::set_xaxis(double min, double max)
{
  setAxisScale(QwtPlot::xBottom, min, max);
}

void
ConstellationDisplayPlot::set_yaxis(double min, double max)
{
  setAxisScale(QwtPlot::yLeft, min, max);
}

void
ConstellationDisplayPlot::set_axis(double xmin, double xmax,
				   double ymin, double ymax)
{
  set_xaxis(xmin, xmax);
  set_yaxis(ymin, ymax);
}

void
ConstellationDisplayPlot::set_pen_size(int size)
{
  if(size > 0 && size < 30){
    _penSize = size;
    for(int i = 0; i < _nplots; i++) {
      _plot_curve[i]->setPen(QPen(Qt::blue, _penSize, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    }
  }
}

void
ConstellationDisplayPlot::replot()
{
  QwtPlot::replot();
}


void
ConstellationDisplayPlot::PlotNewData(const std::vector<double*> realDataPoints,
				      const std::vector<double*> imagDataPoints,
				      const int64_t numDataPoints,
				      const double timeInterval)
{
  if(!_stop) {
    if((numDataPoints > 0)) {
      if(numDataPoints != _numPoints) {
	_numPoints = numDataPoints;
      
	for(int i = 0; i < _nplots; i++) {
	  delete [] _realDataPoints[i];
	  delete [] _imagDataPoints[i];
	  _realDataPoints[i] = new double[_numPoints];
	  _imagDataPoints[i] = new double[_numPoints];

#if QWT_VERSION < 0x060000
	  _plot_curve[i]->setRawData(_realDataPoints[i], _imagDataPoints[i], _numPoints);
#else
	  _plot_curve[i]->setRawSamples(_realDataPoints[i], _imagDataPoints[i], _numPoints);
#endif
	}
      }

      for(int i = 0; i < _nplots; i++) {
	memcpy(_realDataPoints[i], realDataPoints[i], numDataPoints*sizeof(double));
	memcpy(_imagDataPoints[i], imagDataPoints[i], numDataPoints*sizeof(double));
      }

      replot();

    }
  }
}

void
ConstellationDisplayPlot::PlotNewData(const double* realDataPoints,
				      const double* imagDataPoints,
				      const int64_t numDataPoints,
				      const double timeInterval)
{
  std::vector<double*> vecRealDataPoints;
  std::vector<double*> vecImagDataPoints;
  vecRealDataPoints.push_back((double*)realDataPoints);
  vecImagDataPoints.push_back((double*)imagDataPoints);
  PlotNewData(vecRealDataPoints, vecImagDataPoints,
	      numDataPoints, timeInterval);
}

#endif /* CONSTELLATION_DISPLAY_PLOT_C */

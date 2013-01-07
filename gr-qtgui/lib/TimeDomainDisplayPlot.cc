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

#ifndef TIME_DOMAIN_DISPLAY_PLOT_C
#define TIME_DOMAIN_DISPLAY_PLOT_C

#include <TimeDomainDisplayPlot.h>

#include <qwt_scale_draw.h>
#include <qwt_legend.h>
#include <QColor>
#include <cmath>
#include <iostream>

class TimePrecisionClass
{
public:
  TimePrecisionClass(const int timePrecision)
  {
    _timePrecision = timePrecision;
  }

  virtual ~TimePrecisionClass()
  {
  }

  virtual unsigned int getTimePrecision() const
  {
    return _timePrecision;
  }

  virtual void setTimePrecision(const unsigned int newPrecision)
  {
    _timePrecision = newPrecision;
  }
protected:
  unsigned int _timePrecision;
};


class TimeDomainDisplayZoomer: public QwtPlotZoomer, public TimePrecisionClass
{
public:
  TimeDomainDisplayZoomer(QwtPlotCanvas* canvas, const unsigned int timePrecision)
    : QwtPlotZoomer(canvas),TimePrecisionClass(timePrecision)
  {
    setTrackerMode(QwtPicker::AlwaysOn);
  }

  virtual ~TimeDomainDisplayZoomer(){

  }

  virtual void updateTrackerText(){
    updateDisplay();
  }

  void setUnitType(const std::string &type)
  {
    _unitType = type;
  }

protected:
  using QwtPlotZoomer::trackerText;
  virtual QwtText trackerText( const QPoint& p ) const
  {
    QwtDoublePoint dp = QwtPlotZoomer::invTransform(p);
    QwtText t(QString("%1 %2, %3 V").arg(dp.x(), 0, 'f', getTimePrecision()).
	      arg(_unitType.c_str()).
	      arg(dp.y(), 0, 'f', 4));

    return t;
  }

private:
  std::string _unitType;
};


/***********************************************************************
 * Main Time domain plotter widget
 **********************************************************************/
TimeDomainDisplayPlot::TimeDomainDisplayPlot(int nplots, QWidget* parent)
  : DisplayPlot(nplots, parent)
{
  _numPoints = 1024;
  _xAxisPoints = new double[_numPoints];
  memset(_xAxisPoints, 0x0, _numPoints*sizeof(double));

  _zoomer = new TimeDomainDisplayZoomer(canvas(), 0);

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

  setAxisScaleEngine(QwtPlot::xBottom, new QwtLinearScaleEngine);
  setXaxis(0, _numPoints);
  setAxisTitle(QwtPlot::xBottom, "Time (sec)");

  setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);
  setYaxis(-2.0, 2.0);
  setAxisTitle(QwtPlot::yLeft, "Amplitude");

  QList<QColor> colors;
  colors << QColor(Qt::blue) << QColor(Qt::red) << QColor(Qt::green)
	 << QColor(Qt::black) << QColor(Qt::cyan) << QColor(Qt::magenta)
	 << QColor(Qt::yellow) << QColor(Qt::gray) << QColor(Qt::darkRed)
	 << QColor(Qt::darkGreen) << QColor(Qt::darkBlue) << QColor(Qt::darkGray);

  // Setup dataPoints and plot vectors
  // Automatically deleted when parent is deleted
  for(int i = 0; i < _nplots; i++) {
    _dataPoints.push_back(new double[_numPoints]);
    memset(_dataPoints[i], 0x0, _numPoints*sizeof(double));

    _plot_curve.push_back(new QwtPlotCurve(QString("Data %1").arg(i)));
    _plot_curve[i]->attach(this);
    _plot_curve[i]->setPen(QPen(colors[i]));

    QwtSymbol *symbol = new QwtSymbol(QwtSymbol::NoSymbol, QBrush(colors[i]), QPen(colors[i]), QSize(7,7));
    
#if QWT_VERSION < 0x060000
    _plot_curve[i]->setRawData(_xAxisPoints, _dataPoints[i], _numPoints);
    _plot_curve[i]->setSymbol(*symbol);
#else
    _plot_curve[i]->setRawSamples(_xAxisPoints, _dataPoints[i], _numPoints);
    _plot_curve[i]->setSymbol(symbol);
#endif
  }

  _sampleRate = 1;
  _resetXAxisPoints();
}

TimeDomainDisplayPlot::~TimeDomainDisplayPlot()
{
  for(int i = 0; i < _nplots; i++)
    delete [] _dataPoints[i];
  delete[] _xAxisPoints;

  // _zoomer and _panner deleted when parent deleted
}

void
TimeDomainDisplayPlot::replot()
{
  QwtPlot::replot();
}

void
TimeDomainDisplayPlot::plotNewData(const std::vector<double*> dataPoints,
				   const int64_t numDataPoints,
				   const double timeInterval)
{
  if(!_stop) {
    if((numDataPoints > 0)) {
      if(numDataPoints != _numPoints){
	_numPoints = numDataPoints;

	delete[] _xAxisPoints;
	_xAxisPoints = new double[_numPoints];

	for(int i = 0; i < _nplots; i++) {
	  delete[] _dataPoints[i];
	  _dataPoints[i] = new double[_numPoints];

#if QWT_VERSION < 0x060000
	  _plot_curve[i]->setRawData(_xAxisPoints, _dataPoints[i], _numPoints);
#else
	  _plot_curve[i]->setRawSamples(_xAxisPoints, _dataPoints[i], _numPoints);
#endif
	}

	setXaxis(0, numDataPoints);
	_resetXAxisPoints();
      }

      for(int i = 0; i < _nplots; i++) {
	memcpy(_dataPoints[i], dataPoints[i], numDataPoints*sizeof(double));
      }

      replot();
    }
  }
}

void TimeDomainDisplayPlot::_resetXAxisPoints()
{
  double delt = 1.0/_sampleRate;
  for(long loc = 0; loc < _numPoints; loc++){
    _xAxisPoints[loc] = loc*delt;
  }
  setAxisScale(QwtPlot::xBottom, 0, _numPoints*delt);

  // Set up zoomer base for maximum unzoom x-axis
  // and reset to maximum unzoom level
  QwtDoubleRect zbase = _zoomer->zoomBase();
  zbase.setLeft(0);
  zbase.setRight(_numPoints*delt);
  _zoomer->zoom(zbase);
  _zoomer->setZoomBase(zbase);
  _zoomer->zoom(0);
}

void
TimeDomainDisplayPlot::setSampleRate(double sr, double units,
				     const std::string &strunits)
{
  double newsr = sr/units;
  if(newsr != _sampleRate) {
    _sampleRate = sr/units;
    _resetXAxisPoints();

    // While we could change the displayed sigfigs based on the unit being
    // displayed, I think it looks better by just setting it to 4 regardless.
    //double display_units = ceil(log10(units)/2.0);
    double display_units = 4;
    setAxisTitle(QwtPlot::xBottom, QString("Time (%1)").arg(strunits.c_str()));
    ((TimeDomainDisplayZoomer*)_zoomer)->setTimePrecision(display_units);
    ((TimeDomainDisplayZoomer*)_zoomer)->setUnitType(strunits);
  }
}

#endif /* TIME_DOMAIN_DISPLAY_PLOT_C */

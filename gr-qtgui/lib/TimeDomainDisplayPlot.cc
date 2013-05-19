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

#include <gnuradio/qtgui/TimeDomainDisplayPlot.h>

#include <qwt_scale_draw.h>
#include <qwt_legend.h>
#include <QColor>
#include <cmath>
#include <iostream>
#include <volk/volk.h>

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
    QwtText t;
    QwtDoublePoint dp = QwtPlotZoomer::invTransform(p);
    if((dp.y() > 0.0001) && (dp.y() < 10000)) {
      t.setText(QString("%1 %2, %3 V").
		arg(dp.x(), 0, 'f', getTimePrecision()).
		arg(_unitType.c_str()).
		arg(dp.y(), 0, 'f', 4));
    }
    else {
      t.setText(QString("%1 %2, %3 V").
		arg(dp.x(), 0, 'f', getTimePrecision()).
		arg(_unitType.c_str()).
		arg(dp.y(), 0, 'e', 4));
    }

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

  d_semilogx = false;
  d_semilogy = false;

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
    _plot_curve[i]->setRenderHint(QwtPlotItem::RenderAntialiased);

    QwtSymbol *symbol = new QwtSymbol(QwtSymbol::NoSymbol, QBrush(colors[i]),
				      QPen(colors[i]), QSize(7,7));
    
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

	_resetXAxisPoints();
      }

      for(int i = 0; i < _nplots; i++) {
	if(d_semilogy) {
	  for(int n = 0; n < numDataPoints; n++)
	    _dataPoints[i][n] = fabs(dataPoints[i][n]);
	}
	else {
	  memcpy(_dataPoints[i], dataPoints[i], numDataPoints*sizeof(double));
	}
      }

      if(_autoscale_state) {
	double bottom=1e20, top=-1e20;
	for(int n = 0; n < _nplots; n++) {
	  for(int64_t point = 0; point < numDataPoints; point++) {
	    if(_dataPoints[n][point] < bottom) {
	      bottom = _dataPoints[n][point];
	    }
	    if(_dataPoints[n][point] > top) {
	      top = _dataPoints[n][point];
	    }
	  }
	}
	_autoScale(bottom, top);
      }      

      replot();
    }
  }
}

void
TimeDomainDisplayPlot::_resetXAxisPoints()
{
  double delt = 1.0/_sampleRate;
  for(long loc = 0; loc < _numPoints; loc++){
    _xAxisPoints[loc] = loc*delt;
  }

  // Set up zoomer base for maximum unzoom x-axis
  // and reset to maximum unzoom level
  QwtDoubleRect zbase = _zoomer->zoomBase();

  if(d_semilogx) {
    setAxisScale(QwtPlot::xBottom, 1e-1, _numPoints*delt);
    zbase.setLeft(1e-1);
  }
  else {
    setAxisScale(QwtPlot::xBottom, 0, _numPoints*delt);
    zbase.setLeft(0);
  }

  zbase.setRight(_numPoints*delt);
  _zoomer->zoom(zbase);
  _zoomer->setZoomBase(zbase);
  _zoomer->zoom(0);

}

void
TimeDomainDisplayPlot::_autoScale(double bottom, double top)
{
  // Auto scale the y-axis with a margin of 20% (10 dB for log scale)
  double _bot = bottom - fabs(bottom)*0.20;
  double _top = top + fabs(top)*0.20;
  if(d_semilogy) {
    if(bottom > 0) {
      setYaxis(_bot-10, _top+10);
    }
    else {
      setYaxis(1e-3, _top+10);
    }
  }
  else {
    setYaxis(_bot, _top);
  }
}

void
TimeDomainDisplayPlot::setAutoScale(bool state)
{
  _autoscale_state = state;
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

void
TimeDomainDisplayPlot::stemPlot(bool en)
{
  if(en) {
    for(int i = 0; i < _nplots; i++) {
      _plot_curve[i]->setStyle(QwtPlotCurve::Sticks);
      setLineMarker(i, QwtSymbol::Ellipse);
    }
  }
  else {
    for(int i = 0; i < _nplots; i++) {
      _plot_curve[i]->setStyle(QwtPlotCurve::Lines);
      setLineMarker(i, QwtSymbol::NoSymbol);
    }
  }
}

void
TimeDomainDisplayPlot::setSemilogx(bool en)
{
  d_semilogx = en;
  if(!d_semilogx) {
    setAxisScaleEngine(QwtPlot::xBottom, new QwtLinearScaleEngine);
  }
  else {
    setAxisScaleEngine(QwtPlot::xBottom, new QwtLog10ScaleEngine);
  }
  _resetXAxisPoints();
}

void
TimeDomainDisplayPlot::setSemilogy(bool en)
{
  if(d_semilogy != en) {
    d_semilogy = en;
    double max = axisScaleDiv(QwtPlot::yLeft)->upperBound();
    if(!d_semilogy) {
      setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);
      setYaxis(-pow(10.0, max/10.0), pow(10.0, max/10.0));
    }
    else {
      setAxisScaleEngine(QwtPlot::yLeft, new QwtLog10ScaleEngine);
      setYaxis(1e-10, 10.0*log10(max));
    }
  }
}

#endif /* TIME_DOMAIN_DISPLAY_PLOT_C */

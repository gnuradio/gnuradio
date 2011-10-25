/* -*- c++ -*- */
/*
 * Copyright 2008,2009,2010,2011 Free Software Foundation, Inc.
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

  virtual unsigned int GetTimePrecision() const
  {
    return _timePrecision;
  }

  virtual void SetTimePrecision(const unsigned int newPrecision)
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

  void SetUnitType(const std::string &type)
  {
    _unitType = type;
  }

protected:
  using QwtPlotZoomer::trackerText;
  virtual QwtText trackerText( const QwtDoublePoint& p ) const 
  {
    QwtText t(QString("%1 %2, %3 V").arg(p.x(), 0, 'f', GetTimePrecision()).
	      arg(_unitType.c_str()).
	      arg(p.y(), 0, 'f', 4));

    return t;
  }

private:
  std::string _unitType;
};

TimeDomainDisplayPlot::TimeDomainDisplayPlot(int nplots, QWidget* parent)
  : QwtPlot(parent), _nplots(nplots)
{
  resize(parent->width(), parent->height());

  _numPoints = 1024;
  _xAxisPoints = new double[_numPoints];
  memset(_xAxisPoints, 0x0, _numPoints*sizeof(double));

  _zoomer = new TimeDomainDisplayZoomer(canvas(), 0);

#if QWT_VERSION < 0x060000
  _zoomer->setSelectionFlags(QwtPicker::RectSelection | QwtPicker::DragSelection);
#endif

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

#if QWT_VERSION < 0x060000
    _plot_curve[i]->setRawData(_xAxisPoints, _dataPoints[i], _numPoints);
#else
    _plot_curve[i]->setRawSamples(_xAxisPoints, _dataPoints[i], _numPoints);
#endif
}

  _sampleRate = 1;
  _resetXAxisPoints();

  _zoomer->setMousePattern(QwtEventPattern::MouseSelect2,
			   Qt::RightButton, Qt::ControlModifier);
  _zoomer->setMousePattern(QwtEventPattern::MouseSelect3,
			   Qt::RightButton);

  _panner = new QwtPlotPanner(canvas());
  _panner->setAxisEnabled(QwtPlot::yRight, false);
  _panner->setMouseButton(Qt::MidButton);

  // emit the position of clicks on widget
  _picker = new QwtDblClickPlotPicker(canvas());

#if QWT_VERSION < 0x060000
  connect(_picker, SIGNAL(selected(const QwtDoublePoint &)),
	  this, SLOT(OnPickerPointSelected(const QwtDoublePoint &)));
#else
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

  const QColor c(Qt::darkRed);
  _zoomer->setRubberBandPen(c);
  _zoomer->setTrackerPen(c);

  QwtLegend* legendDisplay = new QwtLegend(this);
  legendDisplay->setItemMode(QwtLegend::CheckableItem);
  insertLegend(legendDisplay);

  connect(this, SIGNAL( legendChecked(QwtPlotItem *, bool ) ), 
	  this, SLOT( LegendEntryChecked(QwtPlotItem *, bool ) ));
}

TimeDomainDisplayPlot::~TimeDomainDisplayPlot()
{
  for(int i = 0; i < _nplots; i++)
    delete [] _dataPoints[i];
  delete[] _xAxisPoints;

  // _zoomer and _panner deleted when parent deleted
}

void
TimeDomainDisplayPlot::setYaxis(double min, double max)
{
  setAxisScale(QwtPlot::yLeft, min, max);
  _zoomer->setZoomBase();
}

void
TimeDomainDisplayPlot::setXaxis(double min, double max)
{
  setAxisScale(QwtPlot::xBottom, min, max);
  _zoomer->setZoomBase();
}

void
TimeDomainDisplayPlot::setTitle(int which, QString title)
{
  _plot_curve[which]->setTitle(title);
}

void
TimeDomainDisplayPlot::setColor(int which, QString color)
{
  _plot_curve[which]->setPen(QPen(color));
}

void TimeDomainDisplayPlot::replot()
{
  QwtPlot::replot();
}

void
TimeDomainDisplayPlot::resizeSlot( QSize *s )
{
  // -10 is to spare some room for the legend and x-axis label
  resize(s->width()-10, s->height()-10);
}

void TimeDomainDisplayPlot::PlotNewData(const std::vector<double*> dataPoints,
					const int64_t numDataPoints,
					const double timeInterval)
{
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

void TimeDomainDisplayPlot::LegendEntryChecked(QwtPlotItem* plotItem, bool on)
{
  plotItem->setVisible(!on);
}

void
TimeDomainDisplayPlot::SetSampleRate(double sr, double units,
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
    ((TimeDomainDisplayZoomer*)_zoomer)->SetTimePrecision(display_units);
    ((TimeDomainDisplayZoomer*)_zoomer)->SetUnitType(strunits);
  }
}


void
TimeDomainDisplayPlot::OnPickerPointSelected(const QwtDoublePoint & p)
{
  QPointF point = p;
  //fprintf(stderr,"OnPickerPointSelected %f %f\n", point.x(), point.y());
  emit plotPointSelected(point);
}

void
TimeDomainDisplayPlot::OnPickerPointSelected6(const QPointF & p)
{
  QPointF point = p;
  //fprintf(stderr,"OnPickerPointSelected %f %f\n", point.x(), point.y());
  emit plotPointSelected(point);
}

#endif /* TIME_DOMAIN_DISPLAY_PLOT_C */

/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
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

#ifndef HISTOGRAM_DISPLAY_PLOT_C
#define HISTOGRAM_DISPLAY_PLOT_C

#include <gnuradio/qtgui/HistogramDisplayPlot.h>

#include <qwt_scale_draw.h>
#include <qwt_legend.h>
#include <QColor>
#include <cmath>
#include <iostream>
#include <volk/volk.h>
#include <gnuradio/math.h>
#include <boost/math/special_functions/round.hpp>

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


class HistogramDisplayZoomer: public QwtPlotZoomer, public TimePrecisionClass
{
public:
  HistogramDisplayZoomer(QwtPlotCanvas* canvas, const unsigned int timePrecision)
    : QwtPlotZoomer(canvas),TimePrecisionClass(timePrecision)
  {
    setTrackerMode(QwtPicker::AlwaysOn);
  }

  virtual ~HistogramDisplayZoomer()
  {
  }

  virtual void updateTrackerText()
  {
    updateDisplay();
  }

  void setUnitType(const std::string &type)
  {
    _unitType = type;
  }

protected:
  using QwtPlotZoomer::trackerText;
  virtual QwtText trackerText(const QPoint& p) const
  {
    QwtText t;
    QwtDoublePoint dp = QwtPlotZoomer::invTransform(p);
    if((dp.y() > 0.0001) && (dp.y() < 10000)) {
      t.setText(QString("%1, %2").
		arg(dp.x(), 0, 'f', 4).
		arg(dp.y(), 0, 'f', 0));
    }
    else {
      t.setText(QString("%1, %2").
		arg(dp.x(), 0, 'f', 4).
		arg(dp.y(), 0, 'e', 0));
    }

    return t;
  }

private:
  std::string _unitType;
};


/***********************************************************************
 * Main Time domain plotter widget
 **********************************************************************/
HistogramDisplayPlot::HistogramDisplayPlot(int nplots, QWidget* parent)
  : DisplayPlot(nplots, parent)
{
  _bins = 100;
  _accum = false;

  // Initialize x-axis data array
  _xAxisPoints = new double[_bins];
  memset(_xAxisPoints, 0x0, _bins*sizeof(double));

  _zoomer = new HistogramDisplayZoomer(canvas(), 0);

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

  _magnifier->setAxisEnabled(QwtPlot::xBottom, true);
  _magnifier->setAxisEnabled(QwtPlot::yLeft, false);

  d_semilogx = false;
  d_semilogy = false;

  setAxisScaleEngine(QwtPlot::xBottom, new QwtLinearScaleEngine);
  setXaxis(-1, 1);
  setAxisTitle(QwtPlot::xBottom, "Value");

  setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);
  setYaxis(-2.0, _bins);
  setAxisTitle(QwtPlot::yLeft, "Count");
  
  QList<QColor> colors;
  colors << QColor(Qt::blue) << QColor(Qt::red) << QColor(Qt::green)
	 << QColor(Qt::black) << QColor(Qt::cyan) << QColor(Qt::magenta)
	 << QColor(Qt::yellow) << QColor(Qt::gray) << QColor(Qt::darkRed)
	 << QColor(Qt::darkGreen) << QColor(Qt::darkBlue) << QColor(Qt::darkGray);

  // Setup dataPoints and plot vectors
  // Automatically deleted when parent is deleted
  for(int i = 0; i < _nplots; i++) {
    _yDataPoints.push_back(new double[_bins]);
    memset(_yDataPoints[i], 0, _bins*sizeof(double));

    _plot_curve.push_back(new QwtPlotCurve(QString("Data %1").arg(i)));
    _plot_curve[i]->attach(this);
    _plot_curve[i]->setPen(QPen(colors[i]));
    _plot_curve[i]->setRenderHint(QwtPlotItem::RenderAntialiased);

    // Adjust color's transparency for the brush
    colors[i].setAlpha(127 / _nplots);
    _plot_curve[i]->setBrush(QBrush(colors[i]));

    colors[i].setAlpha(255 / _nplots);
    QwtSymbol *symbol = new QwtSymbol(QwtSymbol::NoSymbol, QBrush(colors[i]),
				      QPen(colors[i]), QSize(7,7));

#if QWT_VERSION < 0x060000
    _plot_curve[i]->setRawData(_xAxisPoints, _yDataPoints[i], _bins);
    _plot_curve[i]->setSymbol(*symbol);
#else
    _plot_curve[i]->setRawSamples(_xAxisPoints, _yDataPoints[i], _bins);
    _plot_curve[i]->setSymbol(symbol);
#endif
  }

  _resetXAxisPoints(-1, 1);
}

HistogramDisplayPlot::~HistogramDisplayPlot()
{
  for(int i = 0; i < _nplots; i++)
    delete[] _yDataPoints[i];
  delete[] _xAxisPoints;

  // _zoomer and _panner deleted when parent deleted
}

void
HistogramDisplayPlot::replot()
{
  QwtPlot::replot();
}

void
HistogramDisplayPlot::plotNewData(const std::vector<double*> dataPoints,
				   const int64_t numDataPoints,
				   const double timeInterval)
{
  if(!_stop) {
    if((numDataPoints > 0)) {

      double bottom = *std::min_element(dataPoints[0], dataPoints[0]+numDataPoints);
      double top = *std::max_element(dataPoints[0], dataPoints[0]+numDataPoints);
      for(int n = 1; n < _nplots; n++) {
        bottom = std::min(bottom, *std::min_element(dataPoints[n], dataPoints[n]+numDataPoints));
        top = std::max(top, *std::max_element(dataPoints[n], dataPoints[n]+numDataPoints));
      }

      _resetXAxisPoints(bottom, top);

      int index;
      double width = (top - bottom)/(_bins-1);

      // Something's wrong with the data (NaN, Inf, or something else)
      if((bottom == top) || (bottom > top))
        return;

      for(int n = 0; n < _nplots; n++) {
        if(!_accum)
          memset(_yDataPoints[n], 0, _bins*sizeof(double));
        for(int64_t point = 0; point < numDataPoints; point++) {
          index = boost::math::iround(1e-20 + (dataPoints[n][point] - bottom)/width);
          index = std::max(std::min(index, _bins), 0);
          _yDataPoints[n][static_cast<int>(index)] += 1;
        }
      }

      double height = *std::max_element(_yDataPoints[0], _yDataPoints[0]+_bins);
      for(int n = 1; n < _nplots; n++) {
        height = std::max(height, *std::max_element(_yDataPoints[n], _yDataPoints[n]+_bins));
      }
      _autoScale(0, height);

      replot();
    }
  }
}

void
HistogramDisplayPlot::_resetXAxisPoints(double bottom, double top)
{
  double width = (top - bottom)/(_bins-1);
  for(long loc = 0; loc < _bins; loc++){
    _xAxisPoints[loc] = bottom + loc*width;
  }

  if(!_autoscale_state) {
    bottom = axisScaleDiv(QwtPlot::xBottom)->lowerBound();
    top = axisScaleDiv(QwtPlot::xBottom)->upperBound();
  }

  // Set up zoomer base for maximum unzoom x-axis
  // and reset to maximum unzoom level
  QwtDoubleRect zbase = _zoomer->zoomBase();

  if(d_semilogx) {
    setAxisScale(QwtPlot::xBottom, 1e-1, top);
    zbase.setLeft(1e-1);
  }
  else {
    setAxisScale(QwtPlot::xBottom, bottom, top);
    zbase.setLeft(bottom);
  }

  zbase.setRight(top);
  _zoomer->zoom(zbase);
  _zoomer->setZoomBase(zbase);
  _zoomer->zoom(0);
}

void
HistogramDisplayPlot::_autoScale(double bottom, double top)
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
HistogramDisplayPlot::setAutoScale(bool state)
{
  _autoscale_state = state;
}

void
HistogramDisplayPlot::setSemilogx(bool en)
{
  d_semilogx = en;
  if(!d_semilogx) {
    setAxisScaleEngine(QwtPlot::xBottom, new QwtLinearScaleEngine);
  }
  else {
    setAxisScaleEngine(QwtPlot::xBottom, new QwtLog10ScaleEngine);
  }
}

void
HistogramDisplayPlot::setSemilogy(bool en)
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
      setYaxis(1e-10, 10.0*log10(100*max));
    }
  }
}

void
HistogramDisplayPlot::setAccumulate(bool state)
{
  _accum = state;
}

void
HistogramDisplayPlot::setMarkerAlpha(int which, int alpha)
{
  if(which < _nplots) {
    // Get the pen color
    QPen pen(_plot_curve[which]->pen());
    QBrush brush(_plot_curve[which]->brush());
    QColor color = brush.color();
    
    // Set new alpha and update pen
    color.setAlpha(alpha);
    brush.setColor(color);
    color.setAlpha(std::min(255, static_cast<int>(alpha*1.5)));
    pen.setColor(color);
    _plot_curve[which]->setBrush(brush);
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
HistogramDisplayPlot::getMarkerAlpha(int which) const
{
  if(which < _nplots) {
    return _plot_curve[which]->brush().color().alpha();
  }
  else {
    return 0;
  }
}

void
HistogramDisplayPlot::setLineColor(int which, QColor color)
{
  if(which < _nplots) {
    // Adjust color's transparency for the brush
    color.setAlpha(127 / _nplots);

    QBrush brush(_plot_curve[which]->brush());
    brush.setColor(color);
    _plot_curve[which]->setBrush(brush);

    // Adjust color's transparency darker for the pen and markers
    color.setAlpha(255 / _nplots);

    QPen pen(_plot_curve[which]->pen());
    pen.setColor(color);
    _plot_curve[which]->setPen(pen);

#if QWT_VERSION < 0x060000
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

void
HistogramDisplayPlot::setNumBins(int bins)
{
  _bins = bins;
  
  delete [] _xAxisPoints;
  _xAxisPoints = new double[_bins];
  memset(_xAxisPoints, 0x0, _bins*sizeof(double));

  for(int i = 0; i < _nplots; i++) {
    delete [] _yDataPoints[i];
    _yDataPoints[i] = new double[_bins];
    memset(_yDataPoints[i], 0, _bins*sizeof(double));

#if QWT_VERSION < 0x060000
    _plot_curve[i]->setRawData(_xAxisPoints, _yDataPoints[i], _bins);
#else
    _plot_curve[i]->setRawSamples(_xAxisPoints, _yDataPoints[i], _bins);
#endif
  }
}

#endif /* HISTOGRAM_DISPLAY_PLOT_C */

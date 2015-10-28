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

#ifdef _MSC_VER
#define copysign _copysign
#endif

class TimePrecisionClass
{
public:
  TimePrecisionClass(const int timeprecision)
  {
    d_time_precision = timeprecision;
  }

  virtual ~TimePrecisionClass()
  {
  }

  virtual unsigned int getTimePrecision() const
  {
    return d_time_precision;
  }

  virtual void setTimePrecision(const unsigned int newprecision)
  {
    d_time_precision = newprecision;
  }
protected:
  unsigned int d_time_precision;
};


class HistogramDisplayZoomer: public QwtPlotZoomer, public TimePrecisionClass
{
public:
#if QWT_VERSION < 0x060100
  HistogramDisplayZoomer(QwtPlotCanvas* canvas, const unsigned int timeprecision)
#else /* QWT_VERSION < 0x060100 */
  HistogramDisplayZoomer(QWidget* canvas, const unsigned int timeprecision)
#endif /* QWT_VERSION < 0x060100 */
    : QwtPlotZoomer(canvas),TimePrecisionClass(timeprecision)
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
    d_unit_type = type;
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
  std::string d_unit_type;
};


/***********************************************************************
 * Main Time domain plotter widget
 **********************************************************************/
HistogramDisplayPlot::HistogramDisplayPlot(int nplots, QWidget* parent)
  : DisplayPlot(nplots, parent)
{
  d_bins = 100;
  d_accum = false;

  // Initialize x-axis data array
  d_xdata = new double[d_bins];
  memset(d_xdata, 0x0, d_bins*sizeof(double));

  d_zoomer = new HistogramDisplayZoomer(canvas(), 0);

#if QWT_VERSION < 0x060000
  d_zoomer->setSelectionFlags(QwtPicker::RectSelection | QwtPicker::DragSelection);
#endif

  d_zoomer->setMousePattern(QwtEventPattern::MouseSelect2,
                            Qt::RightButton, Qt::ControlModifier);
  d_zoomer->setMousePattern(QwtEventPattern::MouseSelect3,
                            Qt::RightButton);

  const QColor c(Qt::darkRed);
  d_zoomer->setRubberBandPen(c);
  d_zoomer->setTrackerPen(c);

  d_semilogx = false;
  d_semilogy = false;
  d_autoscale_state = true;
  d_autoscalex_state = false;

  setAxisScaleEngine(QwtPlot::xBottom, new QwtLinearScaleEngine);
  setXaxis(-1, 1);
  setAxisTitle(QwtPlot::xBottom, "Value");

  setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);
  setYaxis(-2.0, d_bins);
  setAxisTitle(QwtPlot::yLeft, "Count");

  QList<QColor> colors;
  colors << QColor(Qt::blue) << QColor(Qt::red) << QColor(Qt::green)
	 << QColor(Qt::black) << QColor(Qt::cyan) << QColor(Qt::magenta)
	 << QColor(Qt::yellow) << QColor(Qt::gray) << QColor(Qt::darkRed)
	 << QColor(Qt::darkGreen) << QColor(Qt::darkBlue) << QColor(Qt::darkGray);

  // Setup dataPoints and plot vectors
  // Automatically deleted when parent is deleted
  for(int i = 0; i < d_nplots; i++) {
    d_ydata.push_back(new double[d_bins]);
    memset(d_ydata[i], 0, d_bins*sizeof(double));

    d_plot_curve.push_back(new QwtPlotCurve(QString("Data %1").arg(i)));
    d_plot_curve[i]->attach(this);
    d_plot_curve[i]->setPen(QPen(colors[i]));
    d_plot_curve[i]->setRenderHint(QwtPlotItem::RenderAntialiased);

    // Adjust color's transparency for the brush
    colors[i].setAlpha(127 / d_nplots);
    d_plot_curve[i]->setBrush(QBrush(colors[i]));

    colors[i].setAlpha(255 / d_nplots);
    QwtSymbol *symbol = new QwtSymbol(QwtSymbol::NoSymbol, QBrush(colors[i]),
				      QPen(colors[i]), QSize(7,7));

#if QWT_VERSION < 0x060000
    d_plot_curve[i]->setRawData(d_xdata, d_ydata[i], d_bins);
    d_plot_curve[i]->setSymbol(*symbol);
#else
    d_plot_curve[i]->setRawSamples(d_xdata, d_ydata[i], d_bins);
    d_plot_curve[i]->setSymbol(symbol);
#endif
  }

  _resetXAxisPoints(-1, 1);
}

HistogramDisplayPlot::~HistogramDisplayPlot()
{
  for(int i = 0; i < d_nplots; i++)
    delete[] d_ydata[i];
  delete[] d_xdata;

  // d_zoomer and _panner deleted when parent deleted
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
  if(!d_stop) {
    if((numDataPoints > 0)) {

      // keep track of the min/max values for when autoscaleX is called.
      d_xmin = 1e20;
      d_xmax = -1e20;
      for(int n = 0; n < d_nplots; n++) {
        d_xmin = std::min(d_xmin, *std::min_element(dataPoints[n], dataPoints[n]+numDataPoints));
        d_xmax = std::max(d_xmax, *std::max_element(dataPoints[n], dataPoints[n]+numDataPoints));
      }

      // If autoscalex has been clicked, clear the data for the new
      // bin widths and reset the x-axis.
      if(d_autoscalex_state) {
        clear();
        _resetXAxisPoints(d_xmin, d_xmax);
        d_autoscalex_state = false;
      }

      if(!d_accum) {
        clear();
      }

      int index;
      for(int n = 0; n < d_nplots; n++) {
        for(int64_t point = 0; point < numDataPoints; point++) {
          index = boost::math::iround(1e-20 + (dataPoints[n][point] - d_left)/d_width);
          if((index >= 0) && (index < d_bins))
            d_ydata[n][static_cast<int>(index)] += 1;
        }
      }

      double height = *std::max_element(d_ydata[0], d_ydata[0]+d_bins);
      for(int n = 1; n < d_nplots; n++) {
        height = std::max(height, *std::max_element(d_ydata[n], d_ydata[n]+d_bins));
      }

      if(d_autoscale_state)
        _autoScaleY(0, height);

      replot();
    }
  }
}

void
HistogramDisplayPlot::setXaxis(double min, double max)
{
  _resetXAxisPoints(min, max);
}

void
HistogramDisplayPlot::_resetXAxisPoints(double left, double right)
{
  // Something's wrong with the data (NaN, Inf, or something else)
  if((left == right) || (left > right))
    throw std::runtime_error("HistogramDisplayPlot::_resetXAxisPoints left and/or right values are invalid");

  d_left  = left *(1 - copysign(0.1, left));
  d_right = right*(1 + copysign(0.1, right));
  d_width = (d_right - d_left)/(d_bins);
  for(long loc = 0; loc < d_bins; loc++){
    d_xdata[loc] = d_left + loc*d_width;
  }
#if QWT_VERSION < 0x060100
  axisScaleDiv(QwtPlot::xBottom)->setInterval(d_left, d_right);
#else /* QWT_VERSION < 0x060100 */
  QwtScaleDiv scalediv(d_left, d_right);
  setAxisScaleDiv(QwtPlot::xBottom, scalediv);
#endif /* QWT_VERSION < 0x060100 */

  // Set up zoomer base for maximum unzoom x-axis
  // and reset to maximum unzoom level
  QwtDoubleRect zbase = d_zoomer->zoomBase();

  if(d_semilogx) {
    setAxisScale(QwtPlot::xBottom, 1e-1, d_right);
    zbase.setLeft(1e-1);
  }
  else {
    setAxisScale(QwtPlot::xBottom, d_left, d_right);
    zbase.setLeft(d_left);
  }

  zbase.setRight(d_right);
  d_zoomer->zoom(zbase);
  d_zoomer->setZoomBase(zbase);
  d_zoomer->zoom(0);
}

void
HistogramDisplayPlot::_autoScaleY(double bottom, double top)
{
  // Auto scale the y-axis with a margin of 20% (10 dB for log scale)
  double b = bottom - fabs(bottom)*0.20;
  double t = top + fabs(top)*0.20;
  if(d_semilogy) {
    if(bottom > 0) {
      setYaxis(b-10, t+10);
    }
    else {
      setYaxis(1e-3, t+10);
    }
  }
  else {
    setYaxis(b, t);
  }
}

void
HistogramDisplayPlot::setAutoScaleX()
{
  d_autoscalex_state = true;
}

void
HistogramDisplayPlot::setAutoScale(bool state)
{
  d_autoscale_state = state;
}

void
HistogramDisplayPlot::setSemilogx(bool en)
{
  d_semilogx = en;
  if(!d_semilogx) {
    setAxisScaleEngine(QwtPlot::xBottom, new QwtLinearScaleEngine);
  }
  else {
#if QWT_VERSION < 0x060100
    setAxisScaleEngine(QwtPlot::xBottom, new QwtLog10ScaleEngine);
#else /* QWT_VERSION < 0x060100 */
    setAxisScaleEngine(QwtPlot::xBottom, new QwtLogScaleEngine);
#endif /* QWT_VERSION < 0x060100 */
  }
}

void
HistogramDisplayPlot::setSemilogy(bool en)
{
  if(d_semilogy != en) {
    d_semilogy = en;

#if QWT_VERSION < 0x060100
    double max = axisScaleDiv(QwtPlot::yLeft)->upperBound();
#else /* QWT_VERSION < 0x060100 */
    double max = axisScaleDiv(QwtPlot::yLeft).upperBound();
#endif /* QWT_VERSION < 0x060100 */

    if(!d_semilogy) {
      setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);
      setYaxis(-pow(10.0, max/10.0), pow(10.0, max/10.0));
    }
    else {
#if QWT_VERSION < 0x060100
      setAxisScaleEngine(QwtPlot::yLeft, new QwtLog10ScaleEngine);
#else /* QWT_VERSION < 0x060100 */
    setAxisScaleEngine(QwtPlot::yLeft, new QwtLogScaleEngine);
#endif /* QWT_VERSION < 0x060100 */
      setYaxis(1e-10, 10.0*log10(100*max));
    }
  }
}

void
HistogramDisplayPlot::setAccumulate(bool state)
{
  d_accum = state;
}

bool
HistogramDisplayPlot::getAccumulate() const
{
  return d_accum;
}

void
HistogramDisplayPlot::setMarkerAlpha(int which, int alpha)
{
  if(which < d_nplots) {
    // Get the pen color
    QPen pen(d_plot_curve[which]->pen());
    QBrush brush(d_plot_curve[which]->brush());
    QColor color = brush.color();

    // Set new alpha and update pen
    color.setAlpha(alpha);
    brush.setColor(color);
    color.setAlpha(std::min(255, static_cast<int>(alpha*1.5)));
    pen.setColor(color);
    d_plot_curve[which]->setBrush(brush);
    d_plot_curve[which]->setPen(pen);

    // And set the new color for the markers
#if QWT_VERSION < 0x060000
    QwtSymbol sym = (QwtSymbol)d_plot_curve[which]->symbol();
    setLineMarker(which, sym.style());
#else
    QwtSymbol *sym = (QwtSymbol*)d_plot_curve[which]->symbol();
    if(sym) {
      sym->setColor(color);
      sym->setPen(pen);
      d_plot_curve[which]->setSymbol(sym);
    }
#endif
  }
}

int
HistogramDisplayPlot::getMarkerAlpha(int which) const
{
  if(which < d_nplots) {
    return d_plot_curve[which]->brush().color().alpha();
  }
  else {
    return 0;
  }
}

void
HistogramDisplayPlot::setLineColor(int which, QColor color)
{
  if(which < d_nplots) {
    // Adjust color's transparency for the brush
    color.setAlpha(127 / d_nplots);

    QBrush brush(d_plot_curve[which]->brush());
    brush.setColor(color);
    d_plot_curve[which]->setBrush(brush);

    // Adjust color's transparency darker for the pen and markers
    color.setAlpha(255 / d_nplots);

    QPen pen(d_plot_curve[which]->pen());
    pen.setColor(color);
    d_plot_curve[which]->setPen(pen);

#if QWT_VERSION < 0x060000
    d_plot_curve[which]->setPen(pen);
    QwtSymbol sym = (QwtSymbol)d_plot_curve[which]->symbol();
    setLineMarker(which, sym.style());
#else
    QwtSymbol *sym = (QwtSymbol*)d_plot_curve[which]->symbol();
    if(sym) {
      sym->setColor(color);
      sym->setPen(pen);
      d_plot_curve[which]->setSymbol(sym);
    }
#endif
  }
}

void
HistogramDisplayPlot::setNumBins(int bins)
{
  d_bins = bins;

  delete [] d_xdata;
  d_xdata = new double[d_bins];
  _resetXAxisPoints(d_left, d_right);

  for(int i = 0; i < d_nplots; i++) {
    delete [] d_ydata[i];
    d_ydata[i] = new double[d_bins];
    memset(d_ydata[i], 0, d_bins*sizeof(double));

#if QWT_VERSION < 0x060000
    d_plot_curve[i]->setRawData(d_xdata, d_ydata[i], d_bins);
#else
    d_plot_curve[i]->setRawSamples(d_xdata, d_ydata[i], d_bins);
#endif
  }
}


void
HistogramDisplayPlot::clear()
{
  if(!d_stop) {
    for(int n = 0; n < d_nplots; n++) {
      memset(d_ydata[n], 0, d_bins*sizeof(double));
    }
  }
}

#endif /* HISTOGRAM_DISPLAY_PLOT_C */

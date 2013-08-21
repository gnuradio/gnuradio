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

#ifndef WATERFALL_DISPLAY_PLOT_C
#define WATERFALL_DISPLAY_PLOT_C

#include <gnuradio/qtgui/WaterfallDisplayPlot.h>

#include <gnuradio/qtgui/qtgui_types.h>
#include <qwt_color_map.h>
#include <qwt_scale_draw.h>
#include <qwt_legend.h>
#include <qwt_legend_item.h>
#include <qwt_plot_layout.h>
#include <QColor>
#include <iostream>

#include <boost/date_time/posix_time/posix_time.hpp>
namespace pt = boost::posix_time;

#include <QDebug>

/***********************************************************************
 * Text scale widget to provide Y (time) axis text
 **********************************************************************/
class QwtTimeScaleDraw: public QwtScaleDraw, public TimeScaleData
{
public:
  QwtTimeScaleDraw():QwtScaleDraw(),TimeScaleData()
  {
  }

  virtual ~QwtTimeScaleDraw()
  {
  }

  virtual QwtText label(double value) const
  {
    double secs = double(value * getSecondsPerLine());
    return QwtText(QString("").sprintf("%.1f", secs));
  }

  virtual void initiateUpdate()
  {
    // Do this in one call rather than when zeroTime and secondsPerLine
    // updates is to prevent the display from being updated too often...
    invalidateCache();
  }

protected:

private:

};

/***********************************************************************
 * Widget to provide mouse pointer coordinate text
 **********************************************************************/
class WaterfallZoomer: public QwtPlotZoomer, public TimeScaleData, 
		       public FreqOffsetAndPrecisionClass
{
public:
  WaterfallZoomer(QwtPlotCanvas* canvas, const unsigned int freqPrecision)
    : QwtPlotZoomer(canvas), TimeScaleData(),
      FreqOffsetAndPrecisionClass(freqPrecision)
  {
    setTrackerMode(QwtPicker::AlwaysOn);
  }

  virtual ~WaterfallZoomer()
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
  virtual QwtText trackerText( QPoint const &p ) const
  {
    QwtDoublePoint dp = QwtPlotZoomer::invTransform(p);
    double secs = double(dp.y() * getSecondsPerLine());
    QwtText t(QString("%1 %2, %3 s")
 	          .arg(dp.x(), 0, 'f', getFrequencyPrecision())
	          .arg(_unitType.c_str())
              .arg(secs, 0, 'f', 2));
    return t;
  }

private:
  std::string _unitType;
};

/*********************************************************************
* Main waterfall plot widget
*********************************************************************/
WaterfallDisplayPlot::WaterfallDisplayPlot(int nplots, QWidget* parent)
  : DisplayPlot(nplots, parent)
{
  _zoomer = NULL;  // need this for proper init
  _startFrequency = -1;
  _stopFrequency = 1;

  resize(parent->width(), parent->height());
  _numPoints = 1024;

  setAxisTitle(QwtPlot::xBottom, "Frequency (Hz)");
  setAxisScaleDraw(QwtPlot::xBottom, new FreqDisplayScaleDraw(0));

  setAxisTitle(QwtPlot::yLeft, "Time (s)");
  setAxisScaleDraw(QwtPlot::yLeft, new QwtTimeScaleDraw());

  for(int i = 0; i < _nplots; i++) {
    d_data.push_back(new WaterfallData(_startFrequency, _stopFrequency,
				       _numPoints, 200));

#if QWT_VERSION < 0x060000
    d_spectrogram.push_back(new PlotWaterfall(d_data[i], "Spectrogram"));

#else
    d_spectrogram.push_back(new QwtPlotSpectrogram("Spectrogram"));
    d_spectrogram[i]->setData(d_data[i]);
    d_spectrogram[i]->setDisplayMode(QwtPlotSpectrogram::ImageMode, true);
    d_spectrogram[i]->setColorMap(new ColorMap_MultiColor());
#endif

    // a hack around the fact that we aren't using plot curves for the
    // spectrogram plots.
    _plot_curve.push_back(new QwtPlotCurve(QString("Data %1").arg(i)));

    d_spectrogram[i]->attach(this);

    _intensityColorMapType.push_back(INTENSITY_COLOR_MAP_TYPE_MULTI_COLOR);
    setIntensityColorMapType(i, _intensityColorMapType[i],
			     QColor("white"), QColor("white"));    

    setAlpha(i, 255/_nplots);
  }

  // Set bottom plot with no transparency as a base
  setAlpha(0, 255);

  // LeftButton for the zooming
  // MidButton for the panning
  // RightButton: zoom out by 1
  // Ctrl+RighButton: zoom out to full size
  _zoomer = new WaterfallZoomer(canvas(), 0);
#if QWT_VERSION < 0x060000
  _zoomer->setSelectionFlags(QwtPicker::RectSelection | QwtPicker::DragSelection);
#endif
  _zoomer->setMousePattern(QwtEventPattern::MouseSelect2,
			   Qt::RightButton, Qt::ControlModifier);
  _zoomer->setMousePattern(QwtEventPattern::MouseSelect3,
			   Qt::RightButton);

  const QColor c(Qt::black);
  _zoomer->setRubberBandPen(c);
  _zoomer->setTrackerPen(c);

  _updateIntensityRangeDisplay();

  _xAxisMultiplier = 1;
}

WaterfallDisplayPlot::~WaterfallDisplayPlot()
{
}

void
WaterfallDisplayPlot::resetAxis()
{
  for(int i = 0; i < _nplots; i++) {
    d_data[i]->resizeData(_startFrequency, _stopFrequency, _numPoints);
    d_data[i]->reset();
  }

  setAxisScale(QwtPlot::xBottom, _startFrequency, _stopFrequency);

  // Load up the new base zoom settings
  QwtDoubleRect zbase = _zoomer->zoomBase();
  _zoomer->zoom(zbase);
  _zoomer->setZoomBase(zbase);
  _zoomer->setZoomBase(true);
  _zoomer->zoom(0);
}

void
WaterfallDisplayPlot::setFrequencyRange(const double centerfreq,
					const double bandwidth,
					const double units, const std::string &strunits)
{
  double startFreq  = (centerfreq - bandwidth/2.0f) / units;
  double stopFreq   = (centerfreq + bandwidth/2.0f) / units;

  _xAxisMultiplier = units;

  bool reset = false;
  if((startFreq != _startFrequency) || (stopFreq != _stopFrequency))
    reset = true;

  if(stopFreq > startFreq) {
    _startFrequency = startFreq;
    _stopFrequency = stopFreq;

    if((axisScaleDraw(QwtPlot::xBottom) != NULL) && (_zoomer != NULL)) {
      double display_units = ceil(log10(units)/2.0);
      setAxisScaleDraw(QwtPlot::xBottom, new FreqDisplayScaleDraw(display_units));
      setAxisTitle(QwtPlot::xBottom, QString("Frequency (%1)").arg(strunits.c_str()));

      if(reset) {
	resetAxis();
      }

      ((WaterfallZoomer*)_zoomer)->setFrequencyPrecision(display_units);
      ((WaterfallZoomer*)_zoomer)->setUnitType(strunits);
    }
  }
}


double
WaterfallDisplayPlot::getStartFrequency() const
{
  return _startFrequency;
}

double
WaterfallDisplayPlot::getStopFrequency() const
{
  return _stopFrequency;
}

void
WaterfallDisplayPlot::plotNewData(const std::vector<double*> dataPoints,
				  const int64_t numDataPoints,
				  const double timePerFFT,
				  const gr::high_res_timer_type timestamp,
				  const int droppedFrames)
{
  if(!_stop) {
    if(numDataPoints > 0){
      if(numDataPoints != _numPoints){
	_numPoints = numDataPoints;

	resetAxis();

	for(int i = 0; i < _nplots; i++) {
	  d_spectrogram[i]->invalidateCache();
	  d_spectrogram[i]->itemChanged();
	}

	if(isVisible()) {
	  replot();
	}
      }

      QwtTimeScaleDraw* timeScale = (QwtTimeScaleDraw*)axisScaleDraw(QwtPlot::yLeft);
      timeScale->setSecondsPerLine(timePerFFT);
      timeScale->setZeroTime(timestamp);

      ((WaterfallZoomer*)_zoomer)->setSecondsPerLine(timePerFFT);
      ((WaterfallZoomer*)_zoomer)->setZeroTime(timestamp);

      for(int i = 0; i < _nplots; i++) {
	d_data[i]->addFFTData(dataPoints[i], numDataPoints, droppedFrames);
	d_data[i]->incrementNumLinesToUpdate();
	d_spectrogram[i]->invalidateCache();
	d_spectrogram[i]->itemChanged();
      }

      replot();
    }
  }
}

void
WaterfallDisplayPlot::plotNewData(const double* dataPoints,
				  const int64_t numDataPoints,
				  const double timePerFFT,
				  const gr::high_res_timer_type timestamp,
				  const int droppedFrames)
{
  std::vector<double*> vecDataPoints;
  vecDataPoints.push_back((double*)dataPoints);
  plotNewData(vecDataPoints, numDataPoints, timePerFFT,
	      timestamp, droppedFrames);
}

void
WaterfallDisplayPlot::setIntensityRange(const double minIntensity,
					const double maxIntensity)
{
  for(int i = 0; i < _nplots; i++) {
#if QWT_VERSION < 0x060000
    d_data[i]->setRange(QwtDoubleInterval(minIntensity, maxIntensity));
#else
    d_data[i]->setInterval(Qt::ZAxis, QwtInterval(minIntensity, maxIntensity));
#endif

    emit updatedLowerIntensityLevel(minIntensity);
    emit updatedUpperIntensityLevel(maxIntensity);

    _updateIntensityRangeDisplay();
  }
}

double
WaterfallDisplayPlot::getMinIntensity(int which) const
{
#if QWT_VERSION < 0x060000
  QwtDoubleInterval r = d_data[which]->range();
#else
  QwtInterval r = d_data[which]->interval(Qt::ZAxis);
#endif

  return r.minValue();
}

double
WaterfallDisplayPlot::getMaxIntensity(int which) const
{
#if QWT_VERSION < 0x060000
  QwtDoubleInterval r = d_data[which]->range();
#else
  QwtInterval r = d_data[which]->interval(Qt::ZAxis);
#endif

  return r.maxValue();
}

void
WaterfallDisplayPlot::replot()
{
  QwtTimeScaleDraw* timeScale = (QwtTimeScaleDraw*)axisScaleDraw(QwtPlot::yLeft);
  timeScale->initiateUpdate();

  FreqDisplayScaleDraw* freqScale = \
    (FreqDisplayScaleDraw*)axisScaleDraw(QwtPlot::xBottom);
  freqScale->initiateUpdate();

  // Update the time axis display
  if(axisWidget(QwtPlot::yLeft) != NULL){
    axisWidget(QwtPlot::yLeft)->update();
  }

  // Update the Frequency Offset Display
  if(axisWidget(QwtPlot::xBottom) != NULL){
    axisWidget(QwtPlot::xBottom)->update();
  }

  if(_zoomer != NULL){
    ((WaterfallZoomer*)_zoomer)->updateTrackerText();
  }

  QwtPlot::replot();
}

void
WaterfallDisplayPlot::clearData()
{
  for(int i = 0; i < _nplots; i++) {
    d_data[i]->reset();
  }
}


int
WaterfallDisplayPlot::getIntensityColorMapType(int which) const
{
  return _intensityColorMapType[which];
}

void
WaterfallDisplayPlot::setIntensityColorMapType(const int which,
					       const int newType,
					       const QColor lowColor,
					       const QColor highColor)
{
  if((_intensityColorMapType[which] != newType) ||
     ((newType == INTENSITY_COLOR_MAP_TYPE_USER_DEFINED) &&
      (lowColor.isValid() && highColor.isValid()))){
    switch(newType){
    case INTENSITY_COLOR_MAP_TYPE_MULTI_COLOR:{
      _intensityColorMapType[which] = newType;
#if QWT_VERSION < 0x060000
      ColorMap_MultiColor colorMap;
      d_spectrogram[which]->setColorMap(colorMap);
#else
      d_spectrogram[which]->setColorMap(new ColorMap_MultiColor());
#endif
      break;
    }
    case INTENSITY_COLOR_MAP_TYPE_WHITE_HOT:{
      _intensityColorMapType[which] = newType;
#if QWT_VERSION < 0x060000
      ColorMap_WhiteHot colorMap;
      d_spectrogram[which]->setColorMap(colorMap);
#else
      d_spectrogram[which]->setColorMap(new ColorMap_WhiteHot());
#endif
      break;
    }
    case INTENSITY_COLOR_MAP_TYPE_BLACK_HOT:{
      _intensityColorMapType[which] = newType;
#if QWT_VERSION < 0x060000
      ColorMap_BlackHot colorMap;
      d_spectrogram[which]->setColorMap(colorMap);
#else
      d_spectrogram[which]->setColorMap(new ColorMap_BlackHot());
#endif
      break;
    }
    case INTENSITY_COLOR_MAP_TYPE_INCANDESCENT:{
      _intensityColorMapType[which] = newType;
#if QWT_VERSION < 0x060000
      ColorMap_Incandescent colorMap;
      d_spectrogram[which]->setColorMap(colorMap);
#else
      d_spectrogram[which]->setColorMap(new ColorMap_Incandescent());
#endif
      break;
    }
    case INTENSITY_COLOR_MAP_TYPE_USER_DEFINED:{
      _userDefinedLowIntensityColor = lowColor;
      _userDefinedHighIntensityColor = highColor;
      _intensityColorMapType[which] = newType;
#if QWT_VERSION < 0x060000
      ColorMap_UserDefined colorMap(lowColor, highColor);
      d_spectrogram[which]->setColorMap(colorMap);
#else
      d_spectrogram[which]->setColorMap(new ColorMap_UserDefined(lowColor, highColor));
#endif
      break;
    }
    default: break;
    }

    _updateIntensityRangeDisplay();
  }
}

void
WaterfallDisplayPlot::setIntensityColorMapType1(int newType)
{
  setIntensityColorMapType(0, newType, _userDefinedLowIntensityColor, _userDefinedHighIntensityColor);
}

int
WaterfallDisplayPlot::getIntensityColorMapType1() const
{
  return getIntensityColorMapType(0);
}

void
WaterfallDisplayPlot::setUserDefinedLowIntensityColor(QColor c)
{
  _userDefinedLowIntensityColor = c;
}

const QColor
WaterfallDisplayPlot::getUserDefinedLowIntensityColor() const
{
  return _userDefinedLowIntensityColor;
}

void
WaterfallDisplayPlot::setUserDefinedHighIntensityColor(QColor c)
{
  _userDefinedHighIntensityColor = c;
}

const QColor
WaterfallDisplayPlot::getUserDefinedHighIntensityColor() const
{
  return _userDefinedHighIntensityColor;
}

int
WaterfallDisplayPlot::getAlpha(int which)
{
  return d_spectrogram[which]->alpha();
}

void
WaterfallDisplayPlot::setAlpha(int which, int alpha)
{
  d_spectrogram[which]->setAlpha(alpha);
}

void
WaterfallDisplayPlot::_updateIntensityRangeDisplay()
{
  QwtScaleWidget *rightAxis = axisWidget(QwtPlot::yRight);
  rightAxis->setTitle("Intensity (dB)");
  rightAxis->setColorBarEnabled(true);

  for(int i = 0; i < _nplots; i++) {
#if QWT_VERSION < 0x060000
    rightAxis->setColorMap(d_spectrogram[i]->data()->range(),
			   d_spectrogram[i]->colorMap());
    setAxisScale(QwtPlot::yRight,
		 d_spectrogram[i]->data()->range().minValue(),
		 d_spectrogram[i]->data()->range().maxValue());
#else
    QwtInterval intv = d_spectrogram[i]->interval(Qt::ZAxis);
    switch(_intensityColorMapType[i]) {
    case INTENSITY_COLOR_MAP_TYPE_MULTI_COLOR:
      rightAxis->setColorMap(intv, new ColorMap_MultiColor()); break;
    case INTENSITY_COLOR_MAP_TYPE_WHITE_HOT:
      rightAxis->setColorMap(intv, new ColorMap_WhiteHot()); break;
    case INTENSITY_COLOR_MAP_TYPE_BLACK_HOT:
      rightAxis->setColorMap(intv, new ColorMap_BlackHot()); break;
    case INTENSITY_COLOR_MAP_TYPE_INCANDESCENT:
      rightAxis->setColorMap(intv, new ColorMap_Incandescent()); break;
    case INTENSITY_COLOR_MAP_TYPE_USER_DEFINED:
      rightAxis->setColorMap(intv, new ColorMap_UserDefined(_userDefinedLowIntensityColor,
							    _userDefinedHighIntensityColor)); break;
    default:
      rightAxis->setColorMap(intv, new ColorMap_MultiColor()); break;
    }
    setAxisScale(QwtPlot::yRight, intv.minValue(), intv.maxValue());
#endif

    enableAxis(QwtPlot::yRight);

    plotLayout()->setAlignCanvasToScales(true);

    // Tell the display to redraw everything
    d_spectrogram[i]->invalidateCache();
    d_spectrogram[i]->itemChanged();
  }

  // Draw again
  replot();
}

#endif /* WATERFALL_DISPLAY_PLOT_C */

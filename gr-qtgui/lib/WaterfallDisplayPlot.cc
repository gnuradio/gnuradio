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

#include <WaterfallDisplayPlot.h>

#include "qtgui_types.h"
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
 * Create a time label HH:MM:SS.SSS from an input time
 **********************************************************************/
static QString
make_time_label(double secs)
{
  std::string time_str = pt::to_simple_string(pt::from_time_t(time_t(secs)));

  // lops off the YYYY-mmm-DD part of the string
  size_t ind =  time_str.find(" ");
  if(ind != std::string::npos)
    time_str = time_str.substr(ind);

  return QString("").sprintf("%s.%03ld", time_str.c_str(), long(std::fmod(secs*1000, 1000)));
}

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
    double secs = double(value * GetSecondsPerLine());
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

  void SetUnitType(const std::string &type)
  {
    _unitType = type;
  }

protected:
  using QwtPlotZoomer::trackerText;
  virtual QwtText trackerText( QPoint const &p ) const
  {
    QwtDoublePoint dp = QwtPlotZoomer::invTransform(p);
    double secs = double(dp.y() * GetSecondsPerLine());
    QwtText t(QString("%1 %2, %3 s")
 	          .arg(dp.x(), 0, 'f', GetFrequencyPrecision())
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
  _startFrequency = 0;
  _stopFrequency = 4000;
  _useCenterFrequencyFlag = false;

  resize(parent->width(), parent->height());
  _numPoints = 1024;

  setAxisTitle(QwtPlot::xBottom, "Frequency (Hz)");
  setAxisScaleDraw(QwtPlot::xBottom, new FreqDisplayScaleDraw(0));

  setAxisTitle(QwtPlot::yLeft, "Time (s)");
  setAxisScaleDraw(QwtPlot::yLeft, new QwtTimeScaleDraw());

  _lastReplot = 0;

  QList<int> colormaps;
  colormaps << INTENSITY_COLOR_MAP_TYPE_MULTI_COLOR 
	    << INTENSITY_COLOR_MAP_TYPE_WHITE_HOT
	    << INTENSITY_COLOR_MAP_TYPE_BLACK_HOT
	    << INTENSITY_COLOR_MAP_TYPE_INCANDESCENT
	    << INTENSITY_COLOR_MAP_TYPE_USER_DEFINED;

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

    _intensityColorMapType.push_back(colormaps[i%colormaps.size()]);
    SetIntensityColorMapType(i, _intensityColorMapType[i],
			     QColor("white"), QColor("white"));    
  }

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

  _UpdateIntensityRangeDisplay();

  _xAxisMultiplier = 1;
}

WaterfallDisplayPlot::~WaterfallDisplayPlot()
{
}

void
WaterfallDisplayPlot::Reset()
{
  for(int i = 0; i < _nplots; i++) {
    d_data[i]->ResizeData(_startFrequency, _stopFrequency, _numPoints);
    d_data[i]->Reset();
  }

  setAxisScale(QwtPlot::xBottom, _startFrequency, _stopFrequency);

  // Load up the new base zoom settings
  QwtDoubleRect newSize = _zoomer->zoomBase();
  newSize.setLeft(_startFrequency);
  newSize.setWidth(_stopFrequency-_startFrequency);
  _zoomer->zoom(newSize);
  _zoomer->setZoomBase(newSize);
  _zoomer->zoom(0);
}

void
WaterfallDisplayPlot::SetFrequencyRange(const double constStartFreq,
					const double constStopFreq,
					const double constCenterFreq,
					const bool useCenterFrequencyFlag,
					const double units, const std::string &strunits)
{
  double startFreq = constStartFreq / units;
  double stopFreq = constStopFreq / units;
  double centerFreq = constCenterFreq / units;

  _xAxisMultiplier = units;

  _useCenterFrequencyFlag = useCenterFrequencyFlag;

  if(_useCenterFrequencyFlag) {
    startFreq = (startFreq + centerFreq);
    stopFreq = (stopFreq + centerFreq);
  }

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
	Reset();
      }

      ((WaterfallZoomer*)_zoomer)->SetFrequencyPrecision(display_units);
      ((WaterfallZoomer*)_zoomer)->SetUnitType(strunits);
    }
  }
}


double
WaterfallDisplayPlot::GetStartFrequency() const
{
  return _startFrequency;
}

double
WaterfallDisplayPlot::GetStopFrequency() const
{
  return _stopFrequency;
}

void
WaterfallDisplayPlot::PlotNewData(const std::vector<double*> dataPoints,
				  const int64_t numDataPoints,
				  const double timePerFFT,
				  const gruel::high_res_timer_type timestamp,
				  const int droppedFrames)
{
  if(!_stop) {
    if(numDataPoints > 0){
      if(numDataPoints != _numPoints){
	_numPoints = numDataPoints;

	Reset();

	for(int i = 0; i < _nplots; i++) {
	  d_spectrogram[i]->invalidateCache();
	  d_spectrogram[i]->itemChanged();
	}

	if(isVisible()) {
	  replot();
	}

	_lastReplot = gruel::high_res_timer_now();
      }

      for(int i = 0; i < _nplots; i++) {
	d_data[i]->addFFTData(dataPoints[i], numDataPoints, droppedFrames);
	d_data[i]->IncrementNumLinesToUpdate();
      }

      QwtTimeScaleDraw* timeScale = (QwtTimeScaleDraw*)axisScaleDraw(QwtPlot::yLeft);
      timeScale->SetSecondsPerLine(timePerFFT);
      timeScale->SetZeroTime(timestamp);

      ((WaterfallZoomer*)_zoomer)->SetSecondsPerLine(timePerFFT);
      ((WaterfallZoomer*)_zoomer)->SetZeroTime(timestamp);

      for(int i = 0; i < _nplots; i++) {
	d_spectrogram[i]->invalidateCache();
	d_spectrogram[i]->itemChanged();
      }

      replot();
    }
  }
}

void
WaterfallDisplayPlot::PlotNewData(const double* dataPoints,
				  const int64_t numDataPoints,
				  const double timePerFFT,
				  const gruel::high_res_timer_type timestamp,
				  const int droppedFrames)
{
  std::vector<double*> vecDataPoints;
  vecDataPoints.push_back((double*)dataPoints);
  PlotNewData(vecDataPoints, numDataPoints, timePerFFT,
	      timestamp, droppedFrames);
}

void
WaterfallDisplayPlot::SetIntensityRange(const double minIntensity,
					const double maxIntensity)
{
  for(int i = 0; i < _nplots; i++) {
#if QWT_VERSION < 0x060000
    d_data[i]->setRange(QwtDoubleInterval(minIntensity, maxIntensity));
#else
    d_data[i]->setInterval(Qt::ZAxis, QwtInterval(minIntensity, maxIntensity));
#endif

    emit UpdatedLowerIntensityLevel(minIntensity);
    emit UpdatedUpperIntensityLevel(maxIntensity);

    _UpdateIntensityRangeDisplay();
  }
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

int
WaterfallDisplayPlot::GetIntensityColorMapType(int which) const
{
  return _intensityColorMapType[which];
}

void
WaterfallDisplayPlot::SetIntensityColorMapType(const int which,
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

    _UpdateIntensityRangeDisplay();
  }
}

const QColor
WaterfallDisplayPlot::GetUserDefinedLowIntensityColor() const
{
  return _userDefinedLowIntensityColor;
}

const QColor
WaterfallDisplayPlot::GetUserDefinedHighIntensityColor() const
{
  return _userDefinedHighIntensityColor;
}

void
WaterfallDisplayPlot::_UpdateIntensityRangeDisplay()
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

  // Update the last replot timer
  _lastReplot = gruel::high_res_timer_now();
}

#endif /* WATERFALL_DISPLAY_PLOT_C */

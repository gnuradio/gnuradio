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

#ifndef WATERFALL_DISPLAY_PLOT_C
#define WATERFALL_DISPLAY_PLOT_C

#include <WaterfallDisplayPlot.h>

#include <qwt_color_map.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_layout.h>

#include <qapplication.h>

#include <boost/date_time/posix_time/posix_time.hpp>
namespace pt = boost::posix_time;

class FreqOffsetAndPrecisionClass
{
public:
  FreqOffsetAndPrecisionClass(const int freqPrecision)
  {
    _frequencyPrecision = freqPrecision;
    _centerFrequency = 0;
  }

  virtual ~FreqOffsetAndPrecisionClass()
  {
  }

  virtual unsigned int GetFrequencyPrecision() const
  {
    return _frequencyPrecision;
  }

  virtual void SetFrequencyPrecision(const unsigned int newPrecision)
  {
    _frequencyPrecision = newPrecision;
  }

  virtual double GetCenterFrequency() const
  {
    return _centerFrequency;
  }

  virtual void SetCenterFrequency(const double newFreq)
  {
    _centerFrequency = newFreq;
  }

protected:
  unsigned int _frequencyPrecision;
  double _centerFrequency;

private:

};

class WaterfallFreqDisplayScaleDraw: public QwtScaleDraw, public FreqOffsetAndPrecisionClass{
public:
  WaterfallFreqDisplayScaleDraw(const unsigned int precision)
    : QwtScaleDraw(), FreqOffsetAndPrecisionClass(precision)
  {
  }

  virtual ~WaterfallFreqDisplayScaleDraw()
  {
  }

  QwtText label(double value) const
  {
    return QString("%1").arg(value, 0, 'f', GetFrequencyPrecision());
  }

  virtual void initiateUpdate()
  {
    invalidateCache();
  }

protected:

private:

};

class TimeScaleData
{
public:
  TimeScaleData()
  {
    _zeroTime = 0;
    _secondsPerLine = 1.0;
  }
  
  virtual ~TimeScaleData()
  {    
  }

  virtual gruel::high_res_timer_type GetZeroTime() const
  {
    return _zeroTime;
  }
  
  virtual void SetZeroTime(const gruel::high_res_timer_type newTime)
  {
    _zeroTime = newTime - gruel::high_res_timer_epoch();
  }

  virtual void SetSecondsPerLine(const double newTime)
  {
    _secondsPerLine = newTime;
  }

  virtual double GetSecondsPerLine() const
  {
    return _secondsPerLine;
  }

  
protected:
  gruel::high_res_timer_type _zeroTime;
  double _secondsPerLine;
  
private:
  
};

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
    double secs = GetZeroTime()/double(gruel::high_res_timer_tps()) - (value * GetSecondsPerLine());
    return QwtText(make_time_label(secs));
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
  virtual QwtText trackerText( const QwtDoublePoint& p ) const 
  {
    double secs = GetZeroTime()/double(gruel::high_res_timer_tps()) - (p.y() * GetSecondsPerLine());
    QwtText t(QString("%1 %2, %3").
 	      arg(p.x(), 0, 'f', GetFrequencyPrecision()).
	      arg(_unitType.c_str()).arg(make_time_label(secs)));
    return t;
  }

private:
  std::string _unitType;
};

class ColorMap_MultiColor: public QwtLinearColorMap
{
public:
  ColorMap_MultiColor():
    QwtLinearColorMap(Qt::darkCyan, Qt::white)
  {
    addColorStop(0.25, Qt::cyan);
    addColorStop(0.5, Qt::yellow);
    addColorStop(0.75, Qt::red);
  }
};

class ColorMap_WhiteHot: public QwtLinearColorMap
{
public:
  ColorMap_WhiteHot():
    QwtLinearColorMap(Qt::black, Qt::white)
  {
  }
};

class ColorMap_BlackHot: public QwtLinearColorMap
{
public:
  ColorMap_BlackHot():
    QwtLinearColorMap(Qt::white, Qt::black)
  {
  }
};

class ColorMap_Incandescent: public QwtLinearColorMap
{
public:
  ColorMap_Incandescent():
    QwtLinearColorMap(Qt::black, Qt::white)
  {
    addColorStop(0.5, Qt::darkRed);
  }
};

class ColorMap_UserDefined: public QwtLinearColorMap
{
public:
  ColorMap_UserDefined(QColor low, QColor high):
    QwtLinearColorMap(low, high)
  {
  }
};

/*********************************************************************
MAIN WATERFALL PLOT WIDGET
*********************************************************************/

WaterfallDisplayPlot::WaterfallDisplayPlot(QWidget* parent)
  : QwtPlot(parent)
{
  _zoomer = NULL;
  _startFrequency = 0;
  _stopFrequency = 4000;
  
  resize(parent->width(), parent->height());
  _numPoints = 1024;

  QPalette palette;
  palette.setColor(canvas()->backgroundRole(), QColor("white"));
  canvas()->setPalette(palette);   

  setAxisTitle(QwtPlot::xBottom, "Frequency (Hz)");
  setAxisScaleDraw(QwtPlot::xBottom, new WaterfallFreqDisplayScaleDraw(0));

  setAxisTitle(QwtPlot::yLeft, "Time");
  setAxisScaleDraw(QwtPlot::yLeft, new QwtTimeScaleDraw());

  _lastReplot = 0;

  _intensityColorMapType = INTENSITY_COLOR_MAP_TYPE_MULTI_COLOR;

  d_data = new WaterfallData(_startFrequency, _stopFrequency,
			     _numPoints, 200);
  
#if QWT_VERSION < 0x060000
  d_spectrogram = new PlotWaterfall(d_data, "Waterfall Display");

  ColorMap_MultiColor colorMap;
  d_spectrogram->setColorMap(colorMap);

#else
  d_spectrogram = new QwtPlotSpectrogram("Spectrogram");
  d_spectrogram->setData(d_data);
  d_spectrogram->setDisplayMode(QwtPlotSpectrogram::ImageMode, true);
  d_spectrogram->setColorMap(new ColorMap_MultiColor());
#endif

  d_spectrogram->attach(this);
  
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

  // Avoid jumping when labels with more/less digits
  // appear/disappear when scrolling vertically
  
  const QFontMetrics fm(axisWidget(QwtPlot::yLeft)->font());
  QwtScaleDraw *sd = axisScaleDraw(QwtPlot::yLeft);
  sd->setMinimumExtent( fm.width("100.00") );
  
  const QColor c(Qt::black);
  _zoomer->setRubberBandPen(c);
  _zoomer->setTrackerPen(c);

  _UpdateIntensityRangeDisplay();

  _xAxisMultiplier = 1;
}

WaterfallDisplayPlot::~WaterfallDisplayPlot()
{
  delete d_data;
  delete d_spectrogram;
}

void 
WaterfallDisplayPlot::Reset()
{
  d_data->ResizeData(_startFrequency, _stopFrequency, _numPoints);
  d_data->Reset();

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

  if(_useCenterFrequencyFlag){
    startFreq = (startFreq + centerFreq);
    stopFreq = (stopFreq + centerFreq);
  }

  bool reset = false;
  if((startFreq != _startFrequency) || (stopFreq != _stopFrequency))
    reset = true;

  if(stopFreq > startFreq) {
    _startFrequency = startFreq;
    _stopFrequency = stopFreq;
 
    if((axisScaleDraw(QwtPlot::xBottom) != NULL) && (_zoomer != NULL)){
      double display_units = ceil(log10(units)/2.0);
      setAxisScaleDraw(QwtPlot::xBottom, new WaterfallFreqDisplayScaleDraw(display_units));
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
WaterfallDisplayPlot::PlotNewData(const double* dataPoints, 
				  const int64_t numDataPoints,
				  const double timePerFFT,
				  const gruel::high_res_timer_type timestamp,
				  const int droppedFrames)
{
  if(numDataPoints > 0){
    if(numDataPoints != _numPoints){
      _numPoints = numDataPoints;
      
      Reset();
      
      d_spectrogram->invalidateCache();
      d_spectrogram->itemChanged();
      
      if(isVisible()){
	replot();
      }
      
      _lastReplot = gruel::high_res_timer_now();
    }

    if(gruel::high_res_timer_now() - _lastReplot > timePerFFT*gruel::high_res_timer_tps()) {
      d_data->addFFTData(dataPoints, numDataPoints, droppedFrames);
      d_data->IncrementNumLinesToUpdate();
      
      QwtTimeScaleDraw* timeScale = (QwtTimeScaleDraw*)axisScaleDraw(QwtPlot::yLeft);
      timeScale->SetSecondsPerLine(timePerFFT);
      timeScale->SetZeroTime(timestamp);
      
      ((WaterfallZoomer*)_zoomer)->SetSecondsPerLine(timePerFFT);
      ((WaterfallZoomer*)_zoomer)->SetZeroTime(timestamp);
      
      d_spectrogram->invalidateCache();
      d_spectrogram->itemChanged();

      replot();

      _lastReplot = gruel::high_res_timer_now();
    }
  }
}

void
WaterfallDisplayPlot::SetIntensityRange(const double minIntensity, 
					const double maxIntensity)
{
#if QWT_VERSION < 0x060000
  d_data->setRange(QwtDoubleInterval(minIntensity, maxIntensity));
#else
  d_data->setInterval(Qt::ZAxis, QwtInterval(minIntensity, maxIntensity));
#endif

  emit UpdatedLowerIntensityLevel(minIntensity);
  emit UpdatedUpperIntensityLevel(maxIntensity);

  _UpdateIntensityRangeDisplay();
}

void
WaterfallDisplayPlot::replot()
{
  QwtTimeScaleDraw* timeScale = (QwtTimeScaleDraw*)axisScaleDraw(QwtPlot::yLeft);
  timeScale->initiateUpdate();

  WaterfallFreqDisplayScaleDraw* freqScale = \
    (WaterfallFreqDisplayScaleDraw*)axisScaleDraw(QwtPlot::xBottom);
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
WaterfallDisplayPlot::resizeSlot( QSize *s )
{
  resize(s->width(), s->height());
}

int
WaterfallDisplayPlot::GetIntensityColorMapType() const
{
  return _intensityColorMapType;
}

void
WaterfallDisplayPlot::SetIntensityColorMapType(const int newType, 
					       const QColor lowColor, 
					       const QColor highColor)
{
  if((_intensityColorMapType != newType) || 
     ((newType == INTENSITY_COLOR_MAP_TYPE_USER_DEFINED) &&
      (lowColor.isValid() && highColor.isValid()))){
    switch(newType){
    case INTENSITY_COLOR_MAP_TYPE_MULTI_COLOR:{
      _intensityColorMapType = newType;
#if QWT_VERSION < 0x060000
      ColorMap_MultiColor colorMap;
      d_spectrogram->setColorMap(colorMap);
#else
      d_spectrogram->setColorMap(new ColorMap_MultiColor());
#endif
      break;
    }
    case INTENSITY_COLOR_MAP_TYPE_WHITE_HOT:{
      _intensityColorMapType = newType;
#if QWT_VERSION < 0x060000
      ColorMap_WhiteHot colorMap;
      d_spectrogram->setColorMap(colorMap);
#else
      d_spectrogram->setColorMap(new ColorMap_WhiteHot());
#endif
      break;
    }
    case INTENSITY_COLOR_MAP_TYPE_BLACK_HOT:{
      _intensityColorMapType = newType;
#if QWT_VERSION < 0x060000
      ColorMap_BlackHot colorMap;
      d_spectrogram->setColorMap(colorMap);
#else
      d_spectrogram->setColorMap(new ColorMap_BlackHot());
#endif
      break;
    }
    case INTENSITY_COLOR_MAP_TYPE_INCANDESCENT:{
      _intensityColorMapType = newType;
#if QWT_VERSION < 0x060000
      ColorMap_Incandescent colorMap;
      d_spectrogram->setColorMap(colorMap);
#else
      d_spectrogram->setColorMap(new ColorMap_Incandescent());
#endif
      break;
    }
    case INTENSITY_COLOR_MAP_TYPE_USER_DEFINED:{
      _userDefinedLowIntensityColor = lowColor;
      _userDefinedHighIntensityColor = highColor;
      _intensityColorMapType = newType;
#if QWT_VERSION < 0x060000
      ColorMap_UserDefined colorMap(lowColor, highColor);
      d_spectrogram->setColorMap(colorMap);
#else
      d_spectrogram->setColorMap(new ColorMap_UserDefined(lowColor, highColor));
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

#if QWT_VERSION < 0x060000
  rightAxis->setColorMap(d_spectrogram->data()->range(),
			 d_spectrogram->colorMap());
  setAxisScale(QwtPlot::yRight, 
	       d_spectrogram->data()->range().minValue(),
	       d_spectrogram->data()->range().maxValue());
#else
  QwtInterval intv = d_spectrogram->interval(Qt::ZAxis);
  switch(_intensityColorMapType) {
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
  d_spectrogram->invalidateCache();
  d_spectrogram->itemChanged();

  // Draw again
  replot();

  // Update the last replot timer
  _lastReplot = gruel::high_res_timer_now();
}

void
WaterfallDisplayPlot::OnPickerPointSelected(const QwtDoublePoint & p)
{
  QPointF point = p;
  //fprintf(stderr,"OnPickerPointSelected %f %f\n", point.x(), point.y());
  point.setX(point.x() * _xAxisMultiplier);
  emit plotPointSelected(point);
}

void
WaterfallDisplayPlot::OnPickerPointSelected6(const QPointF & p)
{
  QPointF point = p;
  //fprintf(stderr,"OnPickerPointSelected %f %f\n", point.x(), point.y());
  point.setX(point.x() * _xAxisMultiplier);
  emit plotPointSelected(point);
}

#endif /* WATERFALL_DISPLAY_PLOT_C */

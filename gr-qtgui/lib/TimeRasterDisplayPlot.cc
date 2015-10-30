/* -*- c++ -*- */
/*
 * Copyright 2012,2013 Free Software Foundation, Inc.
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

#ifndef TIMERASTER_DISPLAY_PLOT_C
#define TIMERASTER_DISPLAY_PLOT_C

#include <gnuradio/qtgui/TimeRasterDisplayPlot.h>

#include <gnuradio/qtgui/qtgui_types.h>
#include <qwt_color_map.h>
#include <qwt_scale_draw.h>
#include <qwt_legend.h>
#include <qwt_plot_layout.h>
#include <QColor>
#include <iostream>

#if QWT_VERSION < 0x060100
#include <qwt_legend_item.h>
#else /* QWT_VERSION < 0x060100 */
#include <qwt_legend_data.h>
#include <qwt_legend_label.h>
#endif /* QWT_VERSION < 0x060100 */

#include <boost/date_time/posix_time/posix_time.hpp>
namespace pt = boost::posix_time;

#include <QDebug>

/***********************************************************************
 * Text scale widget to provide X (time) axis text
 **********************************************************************/
class QwtXScaleDraw: public QwtScaleDraw, public TimeScaleData
{
public:
  QwtXScaleDraw():QwtScaleDraw(),TimeScaleData() { }

  virtual ~QwtXScaleDraw() { }

  virtual QwtText label(double value) const
  {
    double secs = double(value * getSecondsPerLine());
    return QwtText(QString("").sprintf("%.2f", secs));
  }

  virtual void initiateUpdate()
  {
    // Do this in one call rather than when zeroTime and secondsPerLine
    // updates is to prevent the display from being updated too often...
    invalidateCache();
  }
};

/***********************************************************************
 * Text scale widget to provide Y axis text
 **********************************************************************/
class QwtYScaleDraw: public QwtScaleDraw
{
public:
  QwtYScaleDraw(): QwtScaleDraw(), d_rows(0) { }

  virtual ~QwtYScaleDraw() { }

  virtual QwtText label(double value) const
  {
    if(d_rows > 0)
      value = d_rows - value;
    return QwtText(QString("").sprintf("%.0f", value));
  }

  virtual void initiateUpdate()
  {
    // Do this in one call rather than when zeroTime and secondsPerLine
    // updates is to prevent the display from being updated too often...
    invalidateCache();
  }

  void setRows(double rows) { rows>0 ? d_rows = rows : d_rows = 0; }

private:
  double d_rows;
};

class TimePrecisionClass
{
public:
  TimePrecisionClass(const int timePrecision)
  {
    d_timePrecision = timePrecision;
  }

  virtual ~TimePrecisionClass()
  {
  }

  virtual unsigned int getTimePrecision() const
  {
    return d_timePrecision;
  }

  virtual void setTimePrecision(const unsigned int newPrecision)
  {
    d_timePrecision = newPrecision;
  }
protected:
  unsigned int d_timePrecision;
};

/***********************************************************************
 * Widget to provide mouse pointer coordinate text
 **********************************************************************/
class TimeRasterZoomer: public QwtPlotZoomer, public TimePrecisionClass,
			public TimeScaleData
{
public:
#if QWT_VERSION < 0x060100
  TimeRasterZoomer(QwtPlotCanvas* canvas, double rows, double cols,
		   const unsigned int timePrecision)
#else /* QWT_VERSION < 0x060100 */
  TimeRasterZoomer(QWidget* canvas, double rows, double cols,
		   const unsigned int timePrecision)
#endif /* QWT_VERSION < 0x060100 */
    : QwtPlotZoomer(canvas), TimePrecisionClass(timePrecision), TimeScaleData(),
      d_rows(static_cast<double>(rows)), d_cols(static_cast<double>(cols))
  {
    setTrackerMode(QwtPicker::AlwaysOn);
  }

  virtual ~TimeRasterZoomer()
  {
  }

  virtual void updateTrackerText()
  {
    updateDisplay();
  }

  void setUnitType(const std::string &type)
  {
    d_unitType = type;
  }

  void setColumns(const double cols)
  {
    d_cols = cols;
  }

  void setRows(const double rows)
  {
    d_rows = rows;
  }

protected:
  using QwtPlotZoomer::trackerText;
  virtual QwtText trackerText( QPoint const &p ) const
  {
    QwtDoublePoint dp = QwtPlotZoomer::invTransform(p);
    double x = dp.x() * getSecondsPerLine();
    //double y = dp.y() * getSecondsPerLine() * d_cols;
    double y = floor(d_rows - dp.y());
    QwtText t(QString("%1 %2, %3")
	      .arg(x, 0, 'f', getTimePrecision())
	      .arg(d_unitType.c_str())
              .arg(y, 0, 'f', 0));
    return t;
  }

private:
  std::string d_unitType;
  double d_rows, d_cols;
};

/*********************************************************************
* Main time raster plot widget
*********************************************************************/
TimeRasterDisplayPlot::TimeRasterDisplayPlot(int nplots,
					     double samp_rate,
					     double rows, double cols,
					     QWidget* parent)
  : DisplayPlot(nplots, parent)
{
  d_zoomer = NULL;  // need this for proper init

  resize(parent->width(), parent->height());

  d_samp_rate = samp_rate;
  d_cols = cols;
  d_rows = rows;
  d_numPoints = d_cols;
  d_color_bar_title_font_size = 18;

  setAxisScaleDraw(QwtPlot::xBottom, new QwtXScaleDraw());
  setAxisScaleDraw(QwtPlot::yLeft, new QwtYScaleDraw());

  for(int i = 0; i < d_nplots; i++) {
    d_data.push_back(new TimeRasterData(d_rows, d_cols));
    d_raster.push_back(new PlotTimeRaster("Raster"));
    d_raster[i]->setData(d_data[i]);

    // a hack around the fact that we aren't using plot curves for the
    // raster plots.
    d_plot_curve.push_back(new QwtPlotCurve(QString("Data")));

    d_raster[i]->attach(this);

    d_color_map_type.push_back(INTENSITY_COLOR_MAP_TYPE_BLACK_HOT);
    setAlpha(i, 255/d_nplots);
  }

  // Set bottom plot with no transparency as a base
  setAlpha(0, 255);

  // LeftButton for the zooming
  // MidButton for the panning
  // RightButton: zoom out by 1
  // Ctrl+RighButton: zoom out to full size
  d_zoomer = new TimeRasterZoomer(canvas(), d_rows, d_cols, 0);
#if QWT_VERSION < 0x060000
  d_zoomer->setSelectionFlags(QwtPicker::RectSelection | QwtPicker::DragSelection);
#endif
  d_zoomer->setMousePattern(QwtEventPattern::MouseSelect2,
                            Qt::RightButton, Qt::ControlModifier);
  d_zoomer->setMousePattern(QwtEventPattern::MouseSelect3,
                            Qt::RightButton);

  const QColor c(Qt::red);
  d_zoomer->setRubberBandPen(c);
  d_zoomer->setTrackerPen(c);

  // Set intensity color now (needed d_zoomer before we could do this).
  // We've made sure the old type is different than here so we'll
  // force and update.
  for(int i = 0; i < d_nplots; i++) {
    setIntensityColorMapType(i, INTENSITY_COLOR_MAP_TYPE_WHITE_HOT,
			     QColor("white"), QColor("white"));
  }

  _updateIntensityRangeDisplay();

  reset();
}

TimeRasterDisplayPlot::~TimeRasterDisplayPlot()
{
}

void
TimeRasterDisplayPlot::reset()
{
  for(int i = 0; i < d_nplots; i++) {
    d_data[i]->resizeData(d_rows, d_cols);
    d_data[i]->reset();
  }

  // Update zoomer/picker text units
  std::string strunits[4] = {"sec", "ms", "us", "ns"};
  double units10 = floor(log10(d_samp_rate));
  double units3  = std::max(floor(units10/3), 0.0);
  double units = pow(10, (units10-fmod(units10, 3.0)));
  int iunit = static_cast<int>(units3);

  double sec_per_samp = units/d_samp_rate;

  QwtYScaleDraw* yScale = (QwtYScaleDraw*)axisScaleDraw(QwtPlot::yLeft);
  yScale->setRows(d_rows);

  QwtXScaleDraw* xScale = (QwtXScaleDraw*)axisScaleDraw(QwtPlot::xBottom);
  xScale->setSecondsPerLine(sec_per_samp);
  setAxisTitle(QwtPlot::xBottom, QString("Time (%1)")
	       .arg(strunits[iunit].c_str()));
  xScale->initiateUpdate();

  // Load up the new base zoom settings
  if(d_zoomer) {
    double display_units = 4;
    ((TimeRasterZoomer*)d_zoomer)->setColumns(d_cols);
    ((TimeRasterZoomer*)d_zoomer)->setRows(d_rows);
    ((TimeRasterZoomer*)d_zoomer)->setSecondsPerLine(sec_per_samp);
    ((TimeRasterZoomer*)d_zoomer)->setTimePrecision(display_units);
    ((TimeRasterZoomer*)d_zoomer)->setUnitType(strunits[iunit]);

    QwtDoubleRect newSize = d_zoomer->zoomBase();
    newSize.setLeft(0);
    newSize.setWidth(d_cols);
    newSize.setBottom(0);
    newSize.setHeight(d_rows);

    d_zoomer->zoom(newSize);
    d_zoomer->setZoomBase(newSize);
    d_zoomer->zoom(0);
  }
}

void
TimeRasterDisplayPlot::setNumRows(double rows)
{
  d_rows = rows;
  reset();
}

void
TimeRasterDisplayPlot::setNumCols(double cols)
{
  d_cols = cols;
  reset();
}

void
TimeRasterDisplayPlot::setAlpha(int which, int alpha)
{
  d_raster[which]->setAlpha(alpha);
}

void
TimeRasterDisplayPlot::setSampleRate(double samprate)
{
  d_samp_rate = samprate;
  reset();
}

double
TimeRasterDisplayPlot::numRows() const
{
  return d_rows;
}

double
TimeRasterDisplayPlot::numCols() const
{
  return d_cols;
}

int
TimeRasterDisplayPlot::getAlpha(int which)
{
  return d_raster[which]->alpha();
}

void
TimeRasterDisplayPlot::setPlotDimensions(const double rows, const double cols,
					 const double units, const std::string &strunits)
{
  bool rst = false;
  if((rows != d_rows) || (cols != d_cols))
    rst = true;

  d_rows = rows;
  d_cols = cols;

  if((axisScaleDraw(QwtPlot::xBottom) != NULL) && (d_zoomer != NULL)) {
    if(rst) {
      reset();
    }
  }
}

void
TimeRasterDisplayPlot::plotNewData(const std::vector<double*> dataPoints,
				   const int64_t numDataPoints)
{
  if(!d_stop) {
    if(numDataPoints > 0) {
      for(int i = 0; i < d_nplots; i++) {
	d_data[i]->addData(dataPoints[i], numDataPoints);
	d_raster[i]->invalidateCache();
	d_raster[i]->itemChanged();
      }

      replot();
    }
  }
}

void
TimeRasterDisplayPlot::plotNewData(const double* dataPoints,
				   const int64_t numDataPoints)
{
  std::vector<double*> vecDataPoints;
  vecDataPoints.push_back((double*)dataPoints);
  plotNewData(vecDataPoints, numDataPoints);
}

void
TimeRasterDisplayPlot::setIntensityRange(const double minIntensity,
					 const double maxIntensity)
{
  for(int i = 0; i < d_nplots; i++) {
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
TimeRasterDisplayPlot::getMinIntensity(int which) const
{
#if QWT_VERSION < 0x060000
  QwtDoubleInterval r = d_data[which]->range();
#else
  QwtInterval r = d_data[which]->interval(Qt::ZAxis);
#endif

  return r.minValue();
}

double
TimeRasterDisplayPlot::getMaxIntensity(int which) const
{
#if QWT_VERSION < 0x060000
  QwtDoubleInterval r = d_data[which]->range();
#else
  QwtInterval r = d_data[which]->interval(Qt::ZAxis);
#endif

  return r.maxValue();
}

void
TimeRasterDisplayPlot::replot()
{
  // Update the x-axis display
  if(axisWidget(QwtPlot::yLeft) != NULL) {
    axisWidget(QwtPlot::yLeft)->update();
  }

  // Update the y-axis display
  if(axisWidget(QwtPlot::xBottom) != NULL) {
    axisWidget(QwtPlot::xBottom)->update();
  }

  if(d_zoomer != NULL) {
    ((TimeRasterZoomer*)d_zoomer)->updateTrackerText();
  }

  if(!d_stop) {
    QwtPlot::replot();
  }
}

int
TimeRasterDisplayPlot::getIntensityColorMapType(int which) const
{
  if(which >= (int)d_color_map_type.size())
    throw std::runtime_error("TimerasterDisplayPlot::GetIntesityColorMap: invalid which.\n");

  return d_color_map_type[which];
}

int
TimeRasterDisplayPlot::getIntensityColorMapType1() const
{
  return getIntensityColorMapType(0);
}

int
TimeRasterDisplayPlot::getColorMapTitleFontSize() const
{
  return d_color_bar_title_font_size;
}

void
TimeRasterDisplayPlot::setColorMapTitleFontSize(int tfs)
{
  d_color_bar_title_font_size = tfs;
}

void
TimeRasterDisplayPlot::setIntensityColorMapType(const int which,
						const int newType,
						const QColor lowColor,
						const QColor highColor)
{
  if(which >= (int)d_color_map_type.size())
    throw std::runtime_error("TimerasterDisplayPlot::setIntesityColorMap: invalid which.\n");

  if((d_color_map_type[which] != newType) ||
     ((newType == INTENSITY_COLOR_MAP_TYPE_USER_DEFINED) &&
      (lowColor.isValid() && highColor.isValid()))) {
    switch(newType) {
    case INTENSITY_COLOR_MAP_TYPE_MULTI_COLOR: {
      d_color_map_type[which] = newType;

      d_raster[which]->setColorMap(new ColorMap_MultiColor());
      if(d_zoomer)
	d_zoomer->setTrackerPen(QColor(Qt::black));
      break;
    }
    case INTENSITY_COLOR_MAP_TYPE_WHITE_HOT: {
      d_color_map_type[which] = newType;
      d_raster[which]->setColorMap(new ColorMap_WhiteHot());
      break;
    }
    case INTENSITY_COLOR_MAP_TYPE_BLACK_HOT: {
      d_color_map_type[which] = newType;
      d_raster[which]->setColorMap(new ColorMap_BlackHot());
      break;
    }
    case INTENSITY_COLOR_MAP_TYPE_INCANDESCENT: {
      d_color_map_type[which] = newType;
      d_raster[which]->setColorMap(new ColorMap_Incandescent());
      break;
    }
    case INTENSITY_COLOR_MAP_TYPE_SUNSET: {
      d_color_map_type[which] = newType;
      d_raster[which]->setColorMap(new ColorMap_Sunset());
      break;
    }
    case INTENSITY_COLOR_MAP_TYPE_COOL: {
      d_color_map_type[which] = newType;
      d_raster[which]->setColorMap(new ColorMap_Cool());
      break;
    }
    case INTENSITY_COLOR_MAP_TYPE_USER_DEFINED: {
      d_low_intensity = lowColor;
      d_high_intensity = highColor;
      d_color_map_type[which] = newType;
      d_raster[which]->setColorMap(new ColorMap_UserDefined(lowColor, highColor));
      break;
    }
    default: break;
    }

    _updateIntensityRangeDisplay();
  }
}

void
TimeRasterDisplayPlot::setIntensityColorMapType1(int newType)
{
  setIntensityColorMapType(0, newType, d_low_intensity, d_high_intensity);
}

const QColor
TimeRasterDisplayPlot::getUserDefinedLowIntensityColor() const
{
  return d_low_intensity;
}

const QColor
TimeRasterDisplayPlot::getUserDefinedHighIntensityColor() const
{
  return d_high_intensity;
}

void
TimeRasterDisplayPlot::_updateIntensityRangeDisplay()
{
  QwtScaleWidget *rightAxis = axisWidget(QwtPlot::yRight);
  QwtText colorBarTitle("Intensity");
  colorBarTitle.setFont(QFont("Arial",d_color_bar_title_font_size));
  rightAxis->setTitle(colorBarTitle);
  rightAxis->setColorBarEnabled(true);

  for(int i = 0; i < d_nplots; i++) {
#if QWT_VERSION < 0x060000
    rightAxis->setColorMap(d_raster[i]->data()->range(),
			   d_raster[i]->colorMap());
    setAxisScale(QwtPlot::yRight,
		 d_raster[i]->data()->range().minValue(),
		 d_raster[i]->data()->range().maxValue());
#else
    QwtInterval intv = d_raster[i]->interval(Qt::ZAxis);
    switch(d_color_map_type[i]) {
    case INTENSITY_COLOR_MAP_TYPE_MULTI_COLOR:
      rightAxis->setColorMap(intv, new ColorMap_MultiColor()); break;
    case INTENSITY_COLOR_MAP_TYPE_WHITE_HOT:
      rightAxis->setColorMap(intv, new ColorMap_WhiteHot()); break;
    case INTENSITY_COLOR_MAP_TYPE_BLACK_HOT:
      rightAxis->setColorMap(intv, new ColorMap_BlackHot()); break;
    case INTENSITY_COLOR_MAP_TYPE_INCANDESCENT:
      rightAxis->setColorMap(intv, new ColorMap_Incandescent()); break;
    case INTENSITY_COLOR_MAP_TYPE_SUNSET:
      rightAxis->setColorMap(intv, new ColorMap_Sunset()); break;
    case INTENSITY_COLOR_MAP_TYPE_COOL:
      rightAxis->setColorMap(intv, new ColorMap_Cool()); break;
    case INTENSITY_COLOR_MAP_TYPE_USER_DEFINED:
      rightAxis->setColorMap(intv, new ColorMap_UserDefined(d_low_intensity,
							    d_high_intensity));
      break;
    default:
      rightAxis->setColorMap(intv, new ColorMap_MultiColor()); break;
    }
    setAxisScale(QwtPlot::yRight, intv.minValue(), intv.maxValue());
#endif

    enableAxis(QwtPlot::yRight);

    plotLayout()->setAlignCanvasToScales(true);

    // Tell the display to redraw everything
    d_raster[i]->invalidateCache();
    d_raster[i]->itemChanged();
  }

  // Draw again
  replot();
}

#endif /* TIMERASTER_DISPLAY_PLOT_C */

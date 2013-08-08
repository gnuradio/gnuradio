/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#ifndef QTGUI_TYPES_H
#define QTGUI_TYPES_H

#include <qwt_color_map.h>
#include <qwt_scale_draw.h>
#include <gnuradio/high_res_timer.h>

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

  virtual unsigned int getFrequencyPrecision() const
  {
    return _frequencyPrecision;
  }

  virtual void setFrequencyPrecision(const unsigned int newPrecision)
  {
    _frequencyPrecision = newPrecision;
  }

  virtual double getCenterFrequency() const
  {
    return _centerFrequency;
  }

  virtual void setCenterFrequency(const double newFreq)
  {
    _centerFrequency = newFreq;
  }

protected:

private:
  unsigned int _frequencyPrecision;
  double _centerFrequency;
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

  virtual gr::high_res_timer_type getZeroTime() const
  {
    return _zeroTime;
  }
  
  virtual void setZeroTime(const gr::high_res_timer_type newTime)
  {
    _zeroTime = newTime - gr::high_res_timer_epoch();
  }

  virtual void setSecondsPerLine(const double newTime)
  {
    _secondsPerLine = newTime;
  }

  virtual double getSecondsPerLine() const
  {
    return _secondsPerLine;
  }

  
protected:
  
private:
  gr::high_res_timer_type _zeroTime;
  double _secondsPerLine;
  
};

/***********************************************************************
 * Text scale widget to provide X (freq) axis text
 **********************************************************************/
class FreqDisplayScaleDraw: public QwtScaleDraw, FreqOffsetAndPrecisionClass
{
public:
  FreqDisplayScaleDraw(const unsigned int precision)
    : QwtScaleDraw(), FreqOffsetAndPrecisionClass(precision)
  {
  }

  virtual QwtText label(double value) const
  {
    return QString("%1").arg(value, 0, 'f', getFrequencyPrecision());
  }

  virtual void initiateUpdate(void)
  {
      invalidateCache();
  }

protected:

private:

};

enum{
  INTENSITY_COLOR_MAP_TYPE_MULTI_COLOR = 0,
  INTENSITY_COLOR_MAP_TYPE_WHITE_HOT = 1,
  INTENSITY_COLOR_MAP_TYPE_BLACK_HOT = 2,
  INTENSITY_COLOR_MAP_TYPE_INCANDESCENT = 3,
  INTENSITY_COLOR_MAP_TYPE_USER_DEFINED = 4
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

#endif //QTGUI_TYPES_H

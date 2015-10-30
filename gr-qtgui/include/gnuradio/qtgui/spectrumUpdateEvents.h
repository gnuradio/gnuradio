/* -*- c++ -*- */
/*
 * Copyright 2008-2014 Free Software Foundation, Inc.
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

#ifndef SPECTRUM_UPDATE_EVENTS_H
#define SPECTRUM_UPDATE_EVENTS_H

#include <stdint.h>
#include <QEvent>
#include <QString>
#include <complex>
#include <vector>
#include <gnuradio/high_res_timer.h>
#include <gnuradio/qtgui/api.h>
#include <gnuradio/tags.h>

static const int SpectrumUpdateEventType = 10005;
static const int SpectrumWindowCaptionEventType = 10008;
static const int SpectrumWindowResetEventType = 10009;
static const int SpectrumFrequencyRangeEventType = 10010;

class SpectrumUpdateEvent:public QEvent{

public:
  SpectrumUpdateEvent(const float* fftPoints,
		      const uint64_t numFFTDataPoints,
		      const double* realTimeDomainPoints,
		      const double* imagTimeDomainPoints,
		      const uint64_t numTimeDomainDataPoints,
		      const gr::high_res_timer_type dataTimestamp,
		      const bool repeatDataFlag,
		      const bool lastOfMultipleUpdateFlag,
		      const gr::high_res_timer_type generatedTimestamp,
		      const int droppedFFTFrames);

  ~SpectrumUpdateEvent();

  const float* getFFTPoints() const;
  const double* getRealTimeDomainPoints() const;
  const double* getImagTimeDomainPoints() const;
  uint64_t getNumFFTDataPoints() const;
  uint64_t getNumTimeDomainDataPoints() const;
  gr::high_res_timer_type getDataTimestamp() const;
  bool getRepeatDataFlag() const;
  bool getLastOfMultipleUpdateFlag() const;
  gr::high_res_timer_type getEventGeneratedTimestamp() const;
  int getDroppedFFTFrames() const;

protected:

private:
  float* _fftPoints;
  double* _realDataTimeDomainPoints;
  double* _imagDataTimeDomainPoints;
  uint64_t _numFFTDataPoints;
  uint64_t _numTimeDomainDataPoints;
  gr::high_res_timer_type _dataTimestamp;
  bool _repeatDataFlag;
  bool _lastOfMultipleUpdateFlag;
  gr::high_res_timer_type _eventGeneratedTimestamp;
  int _droppedFFTFrames;
};

class SpectrumWindowCaptionEvent:public QEvent{
public:
  SpectrumWindowCaptionEvent(const QString&);
  ~SpectrumWindowCaptionEvent();
  QString getLabel();

protected:

private:
  QString _labelString;
};

class SpectrumWindowResetEvent:public QEvent{
public:
  SpectrumWindowResetEvent();
  ~SpectrumWindowResetEvent();

protected:

private:

};

class SpectrumFrequencyRangeEvent:public QEvent{
public:
  SpectrumFrequencyRangeEvent(const double, const double, const double);
  ~SpectrumFrequencyRangeEvent();
  double GetCenterFrequency()const;
  double GetStartFrequency()const;
  double GetStopFrequency()const;

protected:

private:
  double _centerFrequency;
  double _startFrequency;
  double _stopFrequency;
};


class TimeUpdateEvent: public QEvent
{
public:
  TimeUpdateEvent(const std::vector<double*> timeDomainPoints,
		  const uint64_t numTimeDomainDataPoints,
                  const std::vector< std::vector<gr::tag_t> > tags);

  ~TimeUpdateEvent();

  int which() const;
  const std::vector<double*> getTimeDomainPoints() const;
  uint64_t getNumTimeDomainDataPoints() const;
  bool getRepeatDataFlag() const;

  const std::vector< std::vector<gr::tag_t> > getTags() const;

  static QEvent::Type Type()
      { return QEvent::Type(SpectrumUpdateEventType); }

protected:

private:
  size_t _nplots;
  std::vector<double*> _dataTimeDomainPoints;
  uint64_t _numTimeDomainDataPoints;
  std::vector< std::vector<gr::tag_t> > _tags;
};


/********************************************************************/


class FreqUpdateEvent: public QEvent
{
public:
  FreqUpdateEvent(const std::vector<double*> dataPoints,
		  const uint64_t numDataPoints);

  ~FreqUpdateEvent();

  int which() const;
  const std::vector<double*> getPoints() const;
  uint64_t getNumDataPoints() const;
  bool getRepeatDataFlag() const;

  static QEvent::Type Type()
  { return QEvent::Type(SpectrumUpdateEventType); }

protected:

private:
  size_t _nplots;
  std::vector<double*> _dataPoints;
  uint64_t _numDataPoints;
};


class SetFreqEvent:public QEvent
{
public:
  SetFreqEvent(const double, const double);
  ~SetFreqEvent();
  double getCenterFrequency() const;
  double getBandwidth() const;

private:
  double _centerFrequency;
  double _bandwidth;
};


/********************************************************************/


class QTGUI_API ConstUpdateEvent: public QEvent
{
public:
  ConstUpdateEvent(const std::vector<double*> realDataPoints,
		   const std::vector<double*> imagDataPoints,
		   const uint64_t numDataPoints);

  ~ConstUpdateEvent();

  int which() const;
  const std::vector<double*> getRealPoints() const;
  const std::vector<double*> getImagPoints() const;
  uint64_t getNumDataPoints() const;
  bool getRepeatDataFlag() const;

  static QEvent::Type Type()
  { return QEvent::Type(SpectrumUpdateEventType); }

protected:

private:
  size_t _nplots;
  std::vector<double*> _realDataPoints;
  std::vector<double*> _imagDataPoints;
  uint64_t _numDataPoints;
};


/********************************************************************/


class WaterfallUpdateEvent: public QEvent
{
public:
  WaterfallUpdateEvent(const std::vector<double*> dataPoints,
		       const uint64_t numDataPoints,
		       const gr::high_res_timer_type dataTimestamp);

  ~WaterfallUpdateEvent();

  int which() const;
  const std::vector<double*> getPoints() const;
  uint64_t getNumDataPoints() const;
  bool getRepeatDataFlag() const;

  gr::high_res_timer_type getDataTimestamp() const;

  static QEvent::Type Type()
  { return QEvent::Type(SpectrumUpdateEventType); }

protected:

private:
  size_t _nplots;
  std::vector<double*> _dataPoints;
  uint64_t _numDataPoints;

  gr::high_res_timer_type _dataTimestamp;
};


/********************************************************************/


class TimeRasterUpdateEvent: public QEvent
{
public:
  TimeRasterUpdateEvent(const std::vector<double*> dataPoints,
			const uint64_t numDataPoints);
  ~TimeRasterUpdateEvent();

  int which() const;
  const std::vector<double*> getPoints() const;
  uint64_t getNumDataPoints() const;
  bool getRepeatDataFlag() const;

  static QEvent::Type Type()
  { return QEvent::Type(SpectrumUpdateEventType); }

protected:

private:
  size_t _nplots;
  std::vector<double*> _dataPoints;
  uint64_t _numDataPoints;
};


class TimeRasterSetSize: public QEvent
{
public:
  TimeRasterSetSize(const double nrows,
                    const double ncols);
  ~TimeRasterSetSize();

  double nRows() const;
  double nCols() const;

  static QEvent::Type Type()
  { return QEvent::Type(SpectrumUpdateEventType+1); }

private:
  double _nrows;
  double _ncols;
};


/********************************************************************/


class HistogramUpdateEvent: public QEvent
{
public:
  HistogramUpdateEvent(const std::vector<double*> points,
                       const uint64_t npoints);

  ~HistogramUpdateEvent();

  int which() const;
  const std::vector<double*> getDataPoints() const;
  uint64_t getNumDataPoints() const;
  bool getRepeatDataFlag() const;

  static QEvent::Type Type()
  { return QEvent::Type(SpectrumUpdateEventType); }

protected:

private:
  size_t _nplots;
  std::vector<double*> _points;
  uint64_t _npoints;
};


class HistogramSetAccumulator: public QEvent
{
public:
  HistogramSetAccumulator(const bool en);
  ~HistogramSetAccumulator();

  bool getAccumulator() const;

  static QEvent::Type Type()
  { return QEvent::Type(SpectrumUpdateEventType+1); }

private:
  bool _en;
};

class HistogramClearEvent: public QEvent
{
public:
  HistogramClearEvent()
    : QEvent(QEvent::Type(SpectrumUpdateEventType+2))
  {}

  ~HistogramClearEvent() {}

  static QEvent::Type Type()
  { return QEvent::Type(SpectrumUpdateEventType+2); }
};


/********************************************************************/


class NumberUpdateEvent: public QEvent
{
public:
  NumberUpdateEvent(const std::vector<float> samples);
  ~NumberUpdateEvent();

  int which() const;
  const std::vector<float> getSamples() const;

  static QEvent::Type Type()
      { return QEvent::Type(SpectrumUpdateEventType); }

protected:

private:
  size_t _nplots;
  std::vector<float> _samples;
};



#endif /* SPECTRUM_UPDATE_EVENTS_H */

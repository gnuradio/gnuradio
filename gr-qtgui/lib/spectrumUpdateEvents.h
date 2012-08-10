#ifndef SPECTRUM_UPDATE_EVENTS_H
#define SPECTRUM_UPDATE_EVENTS_H

#include <stdint.h>
#include <QEvent>
#include <QString>
#include <complex>
#include <vector>
#include <gruel/high_res_timer.h>

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
		      const gruel::high_res_timer_type dataTimestamp,
		      const bool repeatDataFlag,
		      const bool lastOfMultipleUpdateFlag,
		      const gruel::high_res_timer_type generatedTimestamp,
		      const int droppedFFTFrames);

  ~SpectrumUpdateEvent();

  const float* getFFTPoints() const;
  const double* getRealTimeDomainPoints() const;
  const double* getImagTimeDomainPoints() const;
  uint64_t getNumFFTDataPoints() const;
  uint64_t getNumTimeDomainDataPoints() const;
  gruel::high_res_timer_type getDataTimestamp() const;
  bool getRepeatDataFlag() const;
  bool getLastOfMultipleUpdateFlag() const;
  gruel::high_res_timer_type getEventGeneratedTimestamp() const;
  int getDroppedFFTFrames() const;

protected:

private:
  float* _fftPoints;
  double* _realDataTimeDomainPoints;
  double* _imagDataTimeDomainPoints;
  uint64_t _numFFTDataPoints;
  uint64_t _numTimeDomainDataPoints;
  gruel::high_res_timer_type _dataTimestamp;
  bool _repeatDataFlag;
  bool _lastOfMultipleUpdateFlag;
  gruel::high_res_timer_type _eventGeneratedTimestamp;
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
		  const uint64_t numTimeDomainDataPoints);

  ~TimeUpdateEvent();

  int which() const;
  const std::vector<double*> getTimeDomainPoints() const;
  uint64_t getNumTimeDomainDataPoints() const;
  bool getRepeatDataFlag() const;

  static QEvent::Type Type()
      { return QEvent::Type(SpectrumUpdateEventType); }

protected:

private:
  size_t _nplots;
  std::vector<double*> _dataTimeDomainPoints;
  uint64_t _numTimeDomainDataPoints;
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


/********************************************************************/


class ConstUpdateEvent: public QEvent
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
		       const gruel::high_res_timer_type dataTimestamp);

  ~WaterfallUpdateEvent();

  int which() const;
  const std::vector<double*> getPoints() const;
  uint64_t getNumDataPoints() const;
  bool getRepeatDataFlag() const;

  gruel::high_res_timer_type getDataTimestamp() const;

  static QEvent::Type Type()
  { return QEvent::Type(SpectrumUpdateEventType); }

protected:

private:
  size_t _nplots;
  std::vector<double*> _dataPoints;
  uint64_t _numDataPoints;

  gruel::high_res_timer_type _dataTimestamp;
};


#endif /* SPECTRUM_UPDATE_EVENTS_H */

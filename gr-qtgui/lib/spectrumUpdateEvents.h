#ifndef SPECTRUM_UPDATE_EVENTS_H
#define SPECTRUM_UPDATE_EVENTS_H

#include <stdint.h>
#include <QEvent>
#include <QString>
#include <complex>
#include <vector>
#include <gruel/high_res_timer.h>

class SpectrumUpdateEvent:public QEvent{

public:
  SpectrumUpdateEvent(const std::complex<float>* fftPoints,
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

  const std::complex<float>* getFFTPoints() const;
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
  std::complex<float>* _fftPoints;
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

protected:

private:
  size_t _nplots;
  std::vector<double*> _dataTimeDomainPoints;
  uint64_t _numTimeDomainDataPoints;
};


#endif /* SPECTRUM_UPDATE_EVENTS_H */

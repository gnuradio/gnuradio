#ifndef SPECTRUM_UPDATE_EVENTS_H
#define SPECTRUM_UPDATE_EVENTS_H

#include <stdint.h>
#include <QEvent>
#include <QString>
#include <complex>
#include <highResTimeFunctions.h>

class SpectrumUpdateEvent:public QEvent{

public:
  SpectrumUpdateEvent(const std::complex<float>* fftPoints,
		      const uint64_t numFFTDataPoints,
		      const double* realTimeDomainPoints,
		      const double* imagTimeDomainPoints,
		      const uint64_t numTimeDomainDataPoints,
		      const timespec dataTimestamp,
		      const bool repeatDataFlag,
		      const bool lastOfMultipleUpdateFlag,
		      const timespec generatedTimestamp,
		      const int droppedFFTFrames);

  ~SpectrumUpdateEvent();

  const std::complex<float>* getFFTPoints() const;
  const double* getRealTimeDomainPoints() const;
  const double* getImagTimeDomainPoints() const;
  uint64_t getNumFFTDataPoints() const;
  uint64_t getNumTimeDomainDataPoints() const;
  timespec getDataTimestamp() const;
  bool getRepeatDataFlag() const;
  bool getLastOfMultipleUpdateFlag() const;
  timespec getEventGeneratedTimestamp() const;
  int getDroppedFFTFrames() const;

protected:

private:
  std::complex<float>* _fftPoints;
  double* _realDataTimeDomainPoints;
  double* _imagDataTimeDomainPoints;
  uint64_t _numFFTDataPoints;
  uint64_t _numTimeDomainDataPoints;
  timespec _dataTimestamp;
  bool _repeatDataFlag;
  bool _lastOfMultipleUpdateFlag;
  timespec _eventGeneratedTimestamp;
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


#endif /* SPECTRUM_UPDATE_EVENTS_H */

#ifndef SPECTRUM_UPDATE_EVENTS_C
#define SPECTRUM_UPDATE_EVENTS_C

#include <spectrumUpdateEvents.h>

SpectrumUpdateEvent::SpectrumUpdateEvent(const std::complex<float>* fftPoints,
					 const uint64_t numFFTDataPoints,
					 const double* realTimeDomainPoints,
					 const double* imagTimeDomainPoints,
					 const uint64_t numTimeDomainDataPoints,
					 const timespec dataTimestamp,
					 const bool repeatDataFlag,
					 const bool lastOfMultipleUpdateFlag,
					 const timespec generatedTimestamp,
					 const int droppedFFTFrames)
  : QEvent(QEvent::Type(10005))
{
  if(numFFTDataPoints < 1) {
    _numFFTDataPoints = 1;
  }
  else {
    _numFFTDataPoints = numFFTDataPoints;
  }

  if(numTimeDomainDataPoints < 1) {
    _numTimeDomainDataPoints = 1;
  }
  else {
    _numTimeDomainDataPoints = numTimeDomainDataPoints;
  }

  _fftPoints = new std::complex<float>[_numFFTDataPoints];
  _fftPoints[0] = std::complex<float>(0,0);
  memcpy(_fftPoints, fftPoints, numFFTDataPoints*sizeof(std::complex<float>));

  _realDataTimeDomainPoints = new double[_numTimeDomainDataPoints];
  memset(_realDataTimeDomainPoints, 0x0, _numTimeDomainDataPoints*sizeof(double));
  if(numTimeDomainDataPoints > 0) {
    memcpy(_realDataTimeDomainPoints, realTimeDomainPoints,
	   numTimeDomainDataPoints*sizeof(double));
  }

  _imagDataTimeDomainPoints = new double[_numTimeDomainDataPoints];
  memset(_imagDataTimeDomainPoints, 0x0, _numTimeDomainDataPoints*sizeof(double));
  if(numTimeDomainDataPoints > 0) {
    memcpy(_imagDataTimeDomainPoints, imagTimeDomainPoints,
	   numTimeDomainDataPoints*sizeof(double));
  }
  _dataTimestamp = dataTimestamp;
  _repeatDataFlag = repeatDataFlag;
  _lastOfMultipleUpdateFlag = lastOfMultipleUpdateFlag;
  _eventGeneratedTimestamp = generatedTimestamp;
  _droppedFFTFrames = droppedFFTFrames;
}

SpectrumUpdateEvent::~SpectrumUpdateEvent()
{
  delete[] _fftPoints;
  delete[] _realDataTimeDomainPoints;
  delete[] _imagDataTimeDomainPoints;
}

const std::complex<float>*
SpectrumUpdateEvent::getFFTPoints() const
{
  return _fftPoints;
}

const double*
SpectrumUpdateEvent::getRealTimeDomainPoints() const
{
  return _realDataTimeDomainPoints;
}

const double*
SpectrumUpdateEvent::getImagTimeDomainPoints() const
{
  return _imagDataTimeDomainPoints;
}

uint64_t
SpectrumUpdateEvent::getNumFFTDataPoints() const
{
  return _numFFTDataPoints;
}

uint64_t
SpectrumUpdateEvent::getNumTimeDomainDataPoints() const
{
  return _numTimeDomainDataPoints;
}

timespec
SpectrumUpdateEvent::getDataTimestamp() const
{
  return _dataTimestamp;
}

bool
SpectrumUpdateEvent::getRepeatDataFlag() const
{
  return _repeatDataFlag;
}

bool
SpectrumUpdateEvent::getLastOfMultipleUpdateFlag() const
{
  return _lastOfMultipleUpdateFlag;
}

timespec
SpectrumUpdateEvent::getEventGeneratedTimestamp() const
{
  return _eventGeneratedTimestamp;
}

int
SpectrumUpdateEvent::getDroppedFFTFrames() const
{
  return _droppedFFTFrames;
}

SpectrumWindowCaptionEvent::SpectrumWindowCaptionEvent(const QString& newLbl)
  : QEvent(QEvent::Type(10008))
{
  _labelString = newLbl;
}

SpectrumWindowCaptionEvent::~SpectrumWindowCaptionEvent()
{
}

QString
SpectrumWindowCaptionEvent::getLabel()
{
  return _labelString;
}

SpectrumWindowResetEvent::SpectrumWindowResetEvent()
  : QEvent(QEvent::Type(10009))
{
}

SpectrumWindowResetEvent::~SpectrumWindowResetEvent()
{
}

SpectrumFrequencyRangeEvent::SpectrumFrequencyRangeEvent(const double centerFreq, 
							 const double startFreq,
							 const double stopFreq)
  : QEvent(QEvent::Type(10010))
{
  _centerFrequency = centerFreq;
  _startFrequency = startFreq;
  _stopFrequency = stopFreq;
}

SpectrumFrequencyRangeEvent::~SpectrumFrequencyRangeEvent()
{
}

double
SpectrumFrequencyRangeEvent::GetCenterFrequency() const
{
  return _centerFrequency;
}

double
SpectrumFrequencyRangeEvent::GetStartFrequency() const
{
  return _startFrequency;
}

double
SpectrumFrequencyRangeEvent::GetStopFrequency() const
{
  return _stopFrequency;
}


/***************************************************************************/

TimeUpdateEvent::TimeUpdateEvent(const std::vector<gr_complex*> &timeDomainPoints,
				 const uint64_t numTimeDomainDataPoints,
				 const timespec dataTimestamp,
				 const bool repeatDataFlag)
  : QEvent(QEvent::Type(10005))
{
  if(numTimeDomainDataPoints < 1) {
    _numTimeDomainDataPoints = 1;
  }
  else {
    _numTimeDomainDataPoints = numTimeDomainDataPoints;
  }

  _dataTimeDomainPoints = new gr_complex[_numTimeDomainDataPoints];
  memset(_dataTimeDomainPoints, 0x0, _numTimeDomainDataPoints*sizeof(gr_complex));
  if(numTimeDomainDataPoints > 0) {
    memcpy(_dataTimeDomainPoints, timeDomainPoints[0],
	   numTimeDomainDataPoints*sizeof(gr_complex));
  }

  _dataTimestamp = dataTimestamp;
  _repeatDataFlag = repeatDataFlag;
}

TimeUpdateEvent::~TimeUpdateEvent()
{
  delete[] _dataTimeDomainPoints;
}

const gr_complex*
TimeUpdateEvent::getTimeDomainPoints() const
{
  return _dataTimeDomainPoints;
}

uint64_t
TimeUpdateEvent::getNumTimeDomainDataPoints() const
{
  return _numTimeDomainDataPoints;
}

timespec
TimeUpdateEvent::getDataTimestamp() const
{
  return _dataTimestamp;
}

bool
TimeUpdateEvent::getRepeatDataFlag() const
{
  return _repeatDataFlag;
}

#endif /* SPECTRUM_UPDATE_EVENTS_C */

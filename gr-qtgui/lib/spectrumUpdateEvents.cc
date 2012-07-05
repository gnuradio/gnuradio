#ifndef SPECTRUM_UPDATE_EVENTS_C
#define SPECTRUM_UPDATE_EVENTS_C

#include <spectrumUpdateEvents.h>

SpectrumUpdateEvent::SpectrumUpdateEvent(const float* fftPoints,
					 const uint64_t numFFTDataPoints,
					 const double* realTimeDomainPoints,
					 const double* imagTimeDomainPoints,
					 const uint64_t numTimeDomainDataPoints,
					 const gruel::high_res_timer_type dataTimestamp,
					 const bool repeatDataFlag,
					 const bool lastOfMultipleUpdateFlag,
					 const gruel::high_res_timer_type generatedTimestamp,
					 const int droppedFFTFrames)
  : QEvent(QEvent::Type(SpectrumUpdateEventType))
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

  _fftPoints = new float[_numFFTDataPoints];
  _fftPoints[0] = 0;
  memcpy(_fftPoints, fftPoints, numFFTDataPoints*sizeof(float));

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

const float*
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

gruel::high_res_timer_type
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

gruel::high_res_timer_type
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
  : QEvent(QEvent::Type(SpectrumWindowCaptionEventType))
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
  : QEvent(QEvent::Type(SpectrumWindowResetEventType))
{
}

SpectrumWindowResetEvent::~SpectrumWindowResetEvent()
{
}

SpectrumFrequencyRangeEvent::SpectrumFrequencyRangeEvent(const double centerFreq,
							 const double startFreq,
							 const double stopFreq)
  : QEvent(QEvent::Type(SpectrumFrequencyRangeEventType))
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


TimeUpdateEvent::TimeUpdateEvent(const std::vector<double*> timeDomainPoints,
				 const uint64_t numTimeDomainDataPoints)
  : QEvent(QEvent::Type(SpectrumUpdateEventType))
{
  if(numTimeDomainDataPoints < 1) {
    _numTimeDomainDataPoints = 1;
  }
  else {
    _numTimeDomainDataPoints = numTimeDomainDataPoints;
  }

  _nplots = timeDomainPoints.size();
  for(size_t i = 0; i < _nplots; i++) {
    _dataTimeDomainPoints.push_back(new double[_numTimeDomainDataPoints]);
    if(numTimeDomainDataPoints > 0) {
      memcpy(_dataTimeDomainPoints[i], timeDomainPoints[i],
	     _numTimeDomainDataPoints*sizeof(double));
    }
  }
}

TimeUpdateEvent::~TimeUpdateEvent()
{
  for(size_t i = 0; i < _nplots; i++) {
    delete[] _dataTimeDomainPoints[i];
  }
}

const std::vector<double*>
TimeUpdateEvent::getTimeDomainPoints() const
{
  return _dataTimeDomainPoints;
}

uint64_t
TimeUpdateEvent::getNumTimeDomainDataPoints() const
{
  return _numTimeDomainDataPoints;
}


/***************************************************************************/


FreqUpdateEvent::FreqUpdateEvent(const std::vector<double*> dataPoints,
				 const uint64_t numDataPoints)
  : QEvent(QEvent::Type(SpectrumUpdateEventType))
{
  if(numDataPoints < 1) {
    _numDataPoints = 1;
  }
  else {
    _numDataPoints = numDataPoints;
  }

  _nplots = dataPoints.size();
  for(size_t i = 0; i < _nplots; i++) {
    _dataPoints.push_back(new double[_numDataPoints]);
    if(numDataPoints > 0) {
      memcpy(_dataPoints[i], dataPoints[i],
	     _numDataPoints*sizeof(double));
    }
  }
}

FreqUpdateEvent::~FreqUpdateEvent()
{
  for(size_t i = 0; i < _nplots; i++) {
    delete[] _dataPoints[i];
  }
}

const std::vector<double*>
FreqUpdateEvent::getPoints() const
{
  return _dataPoints;
}

uint64_t
FreqUpdateEvent::getNumDataPoints() const
{
  return _numDataPoints;
}


/***************************************************************************/


ConstUpdateEvent::ConstUpdateEvent(const std::vector<double*> realDataPoints,
				   const std::vector<double*> imagDataPoints,
				   const uint64_t numDataPoints)
  : QEvent(QEvent::Type(SpectrumUpdateEventType))
{
  if(numDataPoints < 1) {
    _numDataPoints = 1;
  }
  else {
    _numDataPoints = numDataPoints;
  }

  _nplots = realDataPoints.size();
  for(size_t i = 0; i < _nplots; i++) {
    _realDataPoints.push_back(new double[_numDataPoints]);
    _imagDataPoints.push_back(new double[_numDataPoints]);
    if(numDataPoints > 0) {
      memcpy(_realDataPoints[i], realDataPoints[i],
	     _numDataPoints*sizeof(double));
      memcpy(_imagDataPoints[i], imagDataPoints[i],
	     _numDataPoints*sizeof(double));
    }
  }
}

ConstUpdateEvent::~ConstUpdateEvent()
{
  for(size_t i = 0; i < _nplots; i++) {
    delete[] _realDataPoints[i];
    delete[] _imagDataPoints[i];
  }
}

const std::vector<double*>
ConstUpdateEvent::getRealPoints() const
{
  return _realDataPoints;
}

const std::vector<double*>
ConstUpdateEvent::getImagPoints() const
{
  return _imagDataPoints;
}

uint64_t
ConstUpdateEvent::getNumDataPoints() const
{
  return _numDataPoints;
}


/***************************************************************************/


WaterfallUpdateEvent::WaterfallUpdateEvent(const std::vector<double*> dataPoints,
					   const uint64_t numDataPoints,
					   const gruel::high_res_timer_type dataTimestamp)
  : QEvent(QEvent::Type(SpectrumUpdateEventType))
{
  if(numDataPoints < 1) {
    _numDataPoints = 1;
  }
  else {
    _numDataPoints = numDataPoints;
  }

  _nplots = dataPoints.size();
  for(size_t i = 0; i < _nplots; i++) {
    _dataPoints.push_back(new double[_numDataPoints]);
    if(numDataPoints > 0) {
      memcpy(_dataPoints[i], dataPoints[i],
	     _numDataPoints*sizeof(double));
    }
  }

  _dataTimestamp = dataTimestamp;
}

WaterfallUpdateEvent::~WaterfallUpdateEvent()
{
  for(size_t i = 0; i < _nplots; i++) {
    delete[] _dataPoints[i];
  }
}

const std::vector<double*>
WaterfallUpdateEvent::getPoints() const
{
  return _dataPoints;
}

uint64_t
WaterfallUpdateEvent::getNumDataPoints() const
{
  return _numDataPoints;
}

gruel::high_res_timer_type
WaterfallUpdateEvent::getDataTimestamp() const
{
  return _dataTimestamp;
}

#endif /* SPECTRUM_UPDATE_EVENTS_C */

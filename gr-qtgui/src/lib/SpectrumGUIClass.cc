#ifndef SPECTRUM_GUI_CLASS_CPP
#define SPECTRUM_GUI_CLASS_CPP

#include <SpectrumGUIClass.h>
//Added by qt3to4:
#include <QEvent>
#include <QCustomEvent>

const long SpectrumGUIClass::MAX_FFT_SIZE;
const long SpectrumGUIClass::MIN_FFT_SIZE;

SpectrumGUIClass::SpectrumGUIClass(const uint64_t maxDataSize,
				   const uint64_t fftSize,
				   const double newCenterFrequency,
				   const double newStartFrequency,
				   const double newStopFrequency)
{
  _dataPoints = maxDataSize;
  if(_dataPoints < 2){
    _dataPoints = 2;
  }
  _lastDataPointCount = _dataPoints;

  _fftSize = fftSize;

  _pendingGUIUpdateEventsCount = 0;
  _droppedEntriesCount = 0;

  _centerFrequency = newCenterFrequency;
  _startFrequency = newStartFrequency;
  _stopFrequency = newStopFrequency;

  _windowType = 5;

  timespec_reset(&_lastGUIUpdateTime);

  _windowOpennedFlag = false;
  _fftBuffersCreatedFlag = false;

  // Create Mutex Lock
  //_windowStateLock = new MutexClass("_windowStateLock");

  _powerValue = 1;
}

SpectrumGUIClass::~SpectrumGUIClass()
{
  if(GetWindowOpenFlag()){
    delete _spectrumDisplayForm;
  }

  if(_fftBuffersCreatedFlag){
    delete[] _fftPoints;
    delete[] _realTimeDomainPoints;
    delete[] _imagTimeDomainPoints;
  }

  //delete _windowStateLock;
}

void
SpectrumGUIClass::OpenSpectrumWindow(QWidget* parent,
				     const bool frequency, const bool waterfall,
				     const bool time, const bool constellation,
				     const bool use_openGL)
{
  //_windowStateLock->Lock();

  if(!_windowOpennedFlag){

    if(!_fftBuffersCreatedFlag){
      _fftPoints = new std::complex<float>[_dataPoints];
      _realTimeDomainPoints = new double[_dataPoints];
      _imagTimeDomainPoints = new double[_dataPoints];
      _fftBuffersCreatedFlag = true;
      
      
      memset(_fftPoints, 0x0, _dataPoints*sizeof(std::complex<float>));
      memset(_realTimeDomainPoints, 0x0, _dataPoints*sizeof(double));
      memset(_imagTimeDomainPoints, 0x0, _dataPoints*sizeof(double));
    }
    
    // Called from the Event Thread
    _spectrumDisplayForm = new SpectrumDisplayForm(use_openGL, parent);
    
    // Toggle Windows on/off
    _spectrumDisplayForm->ToggleTabFrequency(frequency);
    _spectrumDisplayForm->ToggleTabWaterfall(waterfall);
    _spectrumDisplayForm->ToggleTabTime(time);
    _spectrumDisplayForm->ToggleTabConstellation(constellation);

    _windowOpennedFlag = true;

    _spectrumDisplayForm->setSystem(this, _dataPoints, _fftSize);

    qApp->processEvents();
  }

  //_windowStateLock->Unlock();

  SetDisplayTitle(_title);
  Reset();

  qApp->postEvent(_spectrumDisplayForm,
		  new QEvent(QEvent::Type(QEvent::User+3)));

  qApp->processEvents();

  timespec_reset(&_lastGUIUpdateTime);

  // Draw Blank Display
  UpdateWindow(false, NULL, 0, NULL, 0, NULL, 0, get_highres_clock(), true);

  // Set up the initial frequency axis settings
  SetFrequencyRange(_centerFrequency, _startFrequency, _stopFrequency);

  // GUI Thread only
  qApp->processEvents();
}

void
SpectrumGUIClass::Reset()
{
  if(GetWindowOpenFlag()) {
    qApp->postEvent(_spectrumDisplayForm,
		    new SpectrumFrequencyRangeEvent(_centerFrequency, 
						    _startFrequency, 
						    _stopFrequency));
    qApp->postEvent(_spectrumDisplayForm, new SpectrumWindowResetEvent());
  }
  _droppedEntriesCount = 0;
  // Call the following function the the Spectrum Window Reset Event window
  // ResetPendingGUIUpdateEvents();
}

void
SpectrumGUIClass::SetDisplayTitle(const std::string newString)
{
  _title.assign(newString);

  if(GetWindowOpenFlag()){
    qApp->postEvent(_spectrumDisplayForm,
		    new SpectrumWindowCaptionEvent(_title.c_str()));
  }
}

bool
SpectrumGUIClass::GetWindowOpenFlag()
{
  bool returnFlag = false;
  //_windowStateLock->Lock();
  returnFlag =  _windowOpennedFlag;
  //_windowStateLock->Unlock();
  return returnFlag;
}


void
SpectrumGUIClass::SetWindowOpenFlag(const bool newFlag)
{
  //_windowStateLock->Lock();
  _windowOpennedFlag = newFlag;
  //_windowStateLock->Unlock();
}

void
SpectrumGUIClass::SetFrequencyRange(const double centerFreq,
				    const double startFreq,
				    const double stopFreq)
{
  //_windowStateLock->Lock();
  _centerFrequency = centerFreq;
  _startFrequency = startFreq;
  _stopFrequency = stopFreq;

  _spectrumDisplayForm->SetFrequencyRange(_centerFrequency,
					  _startFrequency,
					  _stopFrequency);
  //_windowStateLock->Unlock();
}

double
SpectrumGUIClass::GetStartFrequency() const 
{
  double returnValue = 0.0;
  //_windowStateLock->Lock();
  returnValue =  _startFrequency;
  //_windowStateLock->Unlock();
  return returnValue;
}

double
SpectrumGUIClass::GetStopFrequency() const
{
  double returnValue = 0.0;
  //_windowStateLock->Lock();
  returnValue =  _stopFrequency;
  //_windowStateLock->Unlock();
  return returnValue;
}

double
SpectrumGUIClass::GetCenterFrequency() const
{
  double returnValue = 0.0;
  //_windowStateLock->Lock();
  returnValue =  _centerFrequency;
  //_windowStateLock->Unlock();
  return returnValue;
}


void
SpectrumGUIClass::UpdateWindow(const bool updateDisplayFlag,
			       const std::complex<float>* fftBuffer,
			       const uint64_t inputBufferSize,
			       const float* realTimeDomainData,
			       const uint64_t realTimeDomainDataSize,
			       const float* complexTimeDomainData,
			       const uint64_t complexTimeDomainDataSize,
			       const timespec timestamp,
			       const bool lastOfMultipleFFTUpdateFlag)
{
  int64_t bufferSize = inputBufferSize;
  bool repeatDataFlag = false;
  if(bufferSize > _dataPoints){
    bufferSize = _dataPoints;
  }
  int64_t timeDomainBufferSize = 0;

  if(updateDisplayFlag){
    if((fftBuffer != NULL) && (bufferSize > 0)){
      memcpy(_fftPoints, fftBuffer, bufferSize * sizeof(std::complex<float>));
    }

    // Can't do a memcpy since ths is going from float to double data type
    if((realTimeDomainData != NULL) && (realTimeDomainDataSize > 0)){
      const float* realTimeDomainDataPtr = realTimeDomainData;

      double* realTimeDomainPointsPtr = _realTimeDomainPoints;
      timeDomainBufferSize = realTimeDomainDataSize;

      memset( _imagTimeDomainPoints, 0x0, realTimeDomainDataSize*sizeof(double));
      for( uint64_t number = 0; number < realTimeDomainDataSize; number++){
	*realTimeDomainPointsPtr++ = *realTimeDomainDataPtr++;
      }
    }

    // Can't do a memcpy since ths is going from float to double data type
    if((complexTimeDomainData != NULL) && (complexTimeDomainDataSize > 0)){
      const float* complexTimeDomainDataPtr = complexTimeDomainData;

      double* realTimeDomainPointsPtr = _realTimeDomainPoints;
      double* imagTimeDomainPointsPtr = _imagTimeDomainPoints;

      timeDomainBufferSize = complexTimeDomainDataSize;
      for( uint64_t number = 0; number < complexTimeDomainDataSize; number++){
	*realTimeDomainPointsPtr++ = *complexTimeDomainDataPtr++;
	*imagTimeDomainPointsPtr++ = *complexTimeDomainDataPtr++;
      }
    }
  }

  // If bufferSize is zero, then just update the display by sending over the old data
  if(bufferSize < 1){
    bufferSize = _lastDataPointCount;
    repeatDataFlag = true;
  }
  else{
    // Since there is data this time, update the count
    _lastDataPointCount = bufferSize;
  }

  const timespec currentTime = get_highres_clock();
  const timespec lastUpdateGUITime = GetLastGUIUpdateTime();

  if((diff_timespec(currentTime, lastUpdateGUITime) > (4*_updateTime)) &&
     (GetPendingGUIUpdateEvents() > 0) && !timespec_empty(&lastUpdateGUITime)) {
    // Do not update the display if too much data is pending to be displayed
    _droppedEntriesCount++;
  }
  else{
    // Draw the Data
    IncrementPendingGUIUpdateEvents();
    qApp->postEvent(_spectrumDisplayForm,
		    new SpectrumUpdateEvent(_fftPoints, bufferSize,
					    _realTimeDomainPoints,
					    _imagTimeDomainPoints,
					    timeDomainBufferSize,
					    timestamp,
					    repeatDataFlag,
					    lastOfMultipleFFTUpdateFlag,
					    currentTime,
					    _droppedEntriesCount));
    
    // Only reset the dropped entries counter if this is not
    // repeat data since repeat data is dropped by the display systems
    if(!repeatDataFlag){
      _droppedEntriesCount = 0;
    }
  }
}

float
SpectrumGUIClass::GetPowerValue() const
{
  float returnValue = 0;
  //_windowStateLock->Lock();
  returnValue = _powerValue;
  //_windowStateLock->Unlock();
  return returnValue;
}

void
SpectrumGUIClass::SetPowerValue(const float value)
{
  //_windowStateLock->Lock();
  _powerValue = value;
  //_windowStateLock->Unlock();
}

int
SpectrumGUIClass::GetWindowType() const
{
  int returnValue = 0;
  //_windowStateLock->Lock();
  returnValue = _windowType;
  //_windowStateLock->Unlock();
  return returnValue;
}

void
SpectrumGUIClass::SetWindowType(const int newType)
{
  //_windowStateLock->Lock();
  _windowType = newType;
  //_windowStateLock->Unlock();
}

int
SpectrumGUIClass::GetFFTSize() const
{
  int returnValue = 0;
  //_windowStateLock->Lock();
  returnValue = _fftSize;
  //_windowStateLock->Unlock();
  return returnValue;
}

int
SpectrumGUIClass::GetFFTSizeIndex() const
{
  int fftsize = GetFFTSize();
  switch(fftsize) {
  case(1024): return 0; break;
  case(2048): return 1; break;
  case(4096): return 2; break;
  case(8192): return 3; break;
  case(16384): return 3; break;
  case(32768): return 3; break;
  default: return 0;
  }
}

void
SpectrumGUIClass::SetFFTSize(const int newSize)
{
  //_windowStateLock->Lock();
  _fftSize = newSize;
  //_windowStateLock->Unlock();
}

timespec
SpectrumGUIClass::GetLastGUIUpdateTime() const
{
  timespec returnValue;
  //_windowStateLock->Lock();
  returnValue = _lastGUIUpdateTime;
  //_windowStateLock->Unlock();
  return returnValue;
}

void
SpectrumGUIClass::SetLastGUIUpdateTime(const timespec newTime)
{
  //_windowStateLock->Lock();
  _lastGUIUpdateTime = newTime;
  //_windowStateLock->Unlock();
}

unsigned int
SpectrumGUIClass::GetPendingGUIUpdateEvents() const
{
  unsigned int returnValue = 0;
  //_windowStateLock->Lock();
  returnValue = _pendingGUIUpdateEventsCount;
  //_windowStateLock->Unlock();
  return returnValue;
}

void
SpectrumGUIClass::IncrementPendingGUIUpdateEvents()
{
  //_windowStateLock->Lock();
  _pendingGUIUpdateEventsCount++;
  //_windowStateLock->Unlock();
}

void
SpectrumGUIClass::DecrementPendingGUIUpdateEvents()
{
  //_windowStateLock->Lock();
  if(_pendingGUIUpdateEventsCount > 0){
    _pendingGUIUpdateEventsCount--;
  }
  //_windowStateLock->Unlock();
}

void
SpectrumGUIClass::ResetPendingGUIUpdateEvents()
{
  //_windowStateLock->Lock();
  _pendingGUIUpdateEventsCount = 0;
  //_windowStateLock->Unlock();
}


QWidget*
SpectrumGUIClass::qwidget()
{
  return (QWidget*)_spectrumDisplayForm;
}

void
SpectrumGUIClass::SetTimeDomainAxis(double min, double max)
{
  _spectrumDisplayForm->SetTimeDomainAxis(min, max);
}

void
SpectrumGUIClass::SetConstellationAxis(double xmin, double xmax,
				       double ymin, double ymax)
{
  _spectrumDisplayForm->SetConstellationAxis(xmin, xmax, ymin, ymax);

}

void
SpectrumGUIClass::SetConstellationPenSize(int size){
  _spectrumDisplayForm->SetConstellationPenSize(size);
}


void
SpectrumGUIClass::SetFrequencyAxis(double min, double max)
{
  _spectrumDisplayForm->SetFrequencyAxis(min, max);
}

void
SpectrumGUIClass::SetUpdateTime(double t)
{
  _updateTime = t;
  _spectrumDisplayForm->SetUpdateTime(_updateTime);
}


#endif /* SPECTRUM_GUI_CLASS_CPP */

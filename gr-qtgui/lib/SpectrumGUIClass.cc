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

#ifndef SPECTRUM_GUI_CLASS_CPP
#define SPECTRUM_GUI_CLASS_CPP

#include <SpectrumGUIClass.h>
//Added by qt3to4:
#include <QEvent>
#include <QCustomEvent>

const long SpectrumGUIClass::MAX_FFT_SIZE = 32768;
const long SpectrumGUIClass::MIN_FFT_SIZE = 256;

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

  _lastGUIUpdateTime = 0;  

  _windowOpennedFlag = false;
  _fftBuffersCreatedFlag = false;

  _powerValue = 1;
}

SpectrumGUIClass::~SpectrumGUIClass()
{
  // We don't need to delete this since as a QWidget, it is supposed to be destroyed
  // with it's parent. Deleting it causes a segmentation fault, and not deleting it
  // does not leave any extra memory.
  //if(GetWindowOpenFlag()){
    //delete _spectrumDisplayForm;
  //}

  if(_fftBuffersCreatedFlag){
    delete[] _fftPoints;
    delete[] _realTimeDomainPoints;
    delete[] _imagTimeDomainPoints;
  }
}

void
SpectrumGUIClass::OpenSpectrumWindow(QWidget* parent,
				     const bool frequency, const bool waterfall,
				     const bool time, const bool constellation)
{
  d_mutex.lock();

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
    _spectrumDisplayForm = new SpectrumDisplayForm(parent);
    
    // Toggle Windows on/off
    _spectrumDisplayForm->ToggleTabFrequency(frequency);
    _spectrumDisplayForm->ToggleTabWaterfall(waterfall);
    _spectrumDisplayForm->ToggleTabTime(time);
    _spectrumDisplayForm->ToggleTabConstellation(constellation);

    _windowOpennedFlag = true;

    _spectrumDisplayForm->setSystem(this, _dataPoints, _fftSize);

    qApp->processEvents();
  }
  d_mutex.unlock();


  SetDisplayTitle(_title);
  Reset();

  qApp->postEvent(_spectrumDisplayForm,
		  new QEvent(QEvent::Type(QEvent::User+3)));

  _lastGUIUpdateTime = 0;

  // Draw Blank Display
  UpdateWindow(false, NULL, 0, NULL, 0, NULL, 0, gruel::high_res_timer_now(), true);

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
  gruel::scoped_lock lock(d_mutex);
  bool returnFlag = false;
  returnFlag =  _windowOpennedFlag;
  return returnFlag;
}


void
SpectrumGUIClass::SetWindowOpenFlag(const bool newFlag)
{
  gruel::scoped_lock lock(d_mutex);
  _windowOpennedFlag = newFlag;
}

void
SpectrumGUIClass::SetFrequencyRange(const double centerFreq,
				    const double startFreq,
				    const double stopFreq)
{
  gruel::scoped_lock lock(d_mutex);
  _centerFrequency = centerFreq;
  _startFrequency = startFreq;
  _stopFrequency = stopFreq;

  _spectrumDisplayForm->SetFrequencyRange(_centerFrequency,
					  _startFrequency,
					  _stopFrequency);
}

double
SpectrumGUIClass::GetStartFrequency()
{
  gruel::scoped_lock lock(d_mutex);
  double returnValue = 0.0;
  returnValue =  _startFrequency;
  return returnValue;
}

double
SpectrumGUIClass::GetStopFrequency()
{
  gruel::scoped_lock lock(d_mutex);
  double returnValue = 0.0;
  returnValue =  _stopFrequency;
  return returnValue;
}

double
SpectrumGUIClass::GetCenterFrequency()
{
  gruel::scoped_lock lock(d_mutex);
  double returnValue = 0.0;
  returnValue =  _centerFrequency;
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
			       const gruel::high_res_timer_type timestamp,
			       const bool lastOfMultipleFFTUpdateFlag)
{
  //gruel::scoped_lock lock(d_mutex);
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

  const gruel::high_res_timer_type currentTime = gruel::high_res_timer_now();
  const gruel::high_res_timer_type lastUpdateGUITime = GetLastGUIUpdateTime();

  if((currentTime - lastUpdateGUITime > (4*_updateTime)*gruel::high_res_timer_tps()) &&
     (GetPendingGUIUpdateEvents() > 0) && lastUpdateGUITime != 0) {
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
SpectrumGUIClass::GetPowerValue()
{
  gruel::scoped_lock lock(d_mutex);
  float returnValue = 0;
  returnValue = _powerValue;
  return returnValue;
}

void
SpectrumGUIClass::SetPowerValue(const float value)
{
  gruel::scoped_lock lock(d_mutex);
  _powerValue = value;
}

int
SpectrumGUIClass::GetWindowType()
{
  gruel::scoped_lock lock(d_mutex);
  int returnValue = 0;
  returnValue = _windowType;
  return returnValue;
}

void
SpectrumGUIClass::SetWindowType(const int newType)
{
  gruel::scoped_lock lock(d_mutex);
  _windowType = newType;
}

int
SpectrumGUIClass::GetFFTSize()
{
  int returnValue = 0;
  returnValue = _fftSize;
  return returnValue;
}

int
SpectrumGUIClass::GetFFTSizeIndex()
{
  gruel::scoped_lock lock(d_mutex);
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
  gruel::scoped_lock lock(d_mutex);
  _fftSize = newSize;
}

gruel::high_res_timer_type
SpectrumGUIClass::GetLastGUIUpdateTime()
{
  gruel::scoped_lock lock(d_mutex);
  gruel::high_res_timer_type returnValue;
  returnValue = _lastGUIUpdateTime;
  return returnValue;
}

void
SpectrumGUIClass::SetLastGUIUpdateTime(const gruel::high_res_timer_type newTime)
{
  gruel::scoped_lock lock(d_mutex);
  _lastGUIUpdateTime = newTime;
}

unsigned int
SpectrumGUIClass::GetPendingGUIUpdateEvents()
{
  gruel::scoped_lock lock(d_mutex);
  unsigned int returnValue = 0;
  returnValue = _pendingGUIUpdateEventsCount;
  return returnValue;
}

void
SpectrumGUIClass::IncrementPendingGUIUpdateEvents()
{
  gruel::scoped_lock lock(d_mutex);
  _pendingGUIUpdateEventsCount++;
}

void
SpectrumGUIClass::DecrementPendingGUIUpdateEvents()
{
  gruel::scoped_lock lock(d_mutex);
  if(_pendingGUIUpdateEventsCount > 0){
    _pendingGUIUpdateEventsCount--;
  }
}

void
SpectrumGUIClass::ResetPendingGUIUpdateEvents()
{
  gruel::scoped_lock lock(d_mutex);
  _pendingGUIUpdateEventsCount = 0;
}


QWidget*
SpectrumGUIClass::qwidget()
{
  gruel::scoped_lock lock(d_mutex);
  return (QWidget*)_spectrumDisplayForm;
}

void
SpectrumGUIClass::SetTimeDomainAxis(double min, double max)
{
  gruel::scoped_lock lock(d_mutex);
  _spectrumDisplayForm->SetTimeDomainAxis(min, max);
}

void
SpectrumGUIClass::SetConstellationAxis(double xmin, double xmax,
				       double ymin, double ymax)
{
  gruel::scoped_lock lock(d_mutex);
  _spectrumDisplayForm->SetConstellationAxis(xmin, xmax, ymin, ymax);
}

void
SpectrumGUIClass::SetConstellationPenSize(int size)
{
  gruel::scoped_lock lock(d_mutex);
  _spectrumDisplayForm->SetConstellationPenSize(size);
}


void
SpectrumGUIClass::SetFrequencyAxis(double min, double max)
{
  gruel::scoped_lock lock(d_mutex);
  _spectrumDisplayForm->SetFrequencyAxis(min, max);
}

void
SpectrumGUIClass::SetUpdateTime(double t)
{
  gruel::scoped_lock lock(d_mutex);
  _updateTime = t;
  _spectrumDisplayForm->SetUpdateTime(_updateTime);
}


#endif /* SPECTRUM_GUI_CLASS_CPP */

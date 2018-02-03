/* -*- c++ -*- */
/*
 * Copyright 2008-2011 Free Software Foundation, Inc.
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

#include <gnuradio/qtgui/SpectrumGUIClass.h>
//Added by qt3to4:
#include <QEvent>
#include <QCustomEvent>
#include <volk/volk.h>

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
  //if(getWindowOpenFlag()){
    //delete _spectrumDisplayForm;
  //}

  if(_fftBuffersCreatedFlag){
    delete[] _fftPoints;
    delete[] _realTimeDomainPoints;
    delete[] _imagTimeDomainPoints;
  }
}

void
SpectrumGUIClass::openSpectrumWindow(QWidget* parent,
				     const bool frequency, const bool waterfall,
				     const bool time, const bool constellation)
{
  d_mutex.lock();

  if(!_windowOpennedFlag) {

    if(!_fftBuffersCreatedFlag) {
      _fftPoints = new float[_dataPoints];
      _realTimeDomainPoints = new double[_dataPoints];
      _imagTimeDomainPoints = new double[_dataPoints];
      _fftBuffersCreatedFlag = true;

      memset(_fftPoints, 0x0, _dataPoints*sizeof(float));
      memset(_realTimeDomainPoints, 0x0, _dataPoints*sizeof(double));
      memset(_imagTimeDomainPoints, 0x0, _dataPoints*sizeof(double));
    }

    // Called from the Event Thread
    _spectrumDisplayForm = new SpectrumDisplayForm(parent);

    // Toggle Windows on/off
    _spectrumDisplayForm->toggleTabFrequency(frequency);
    _spectrumDisplayForm->toggleTabWaterfall(waterfall);
    _spectrumDisplayForm->toggleTabTime(time);
    _spectrumDisplayForm->toggleTabConstellation(constellation);

    _windowOpennedFlag = true;

    _spectrumDisplayForm->setSystem(this, _dataPoints, _fftSize);

    qApp->processEvents();
  }
  d_mutex.unlock();


  setDisplayTitle(_title);
  reset();

  qApp->postEvent(_spectrumDisplayForm,
		  new QEvent(QEvent::Type(QEvent::User+3)));

  _lastGUIUpdateTime = 0;

  // Draw Blank Display
  updateWindow(false, NULL, 0, NULL, 0, NULL, 0, gr::high_res_timer_now(), true);

  // Set up the initial frequency axis settings
  setFrequencyRange(_centerFrequency, _startFrequency, _stopFrequency);

  // GUI Thread only
  qApp->processEvents();

  // Set the FFT Size combo box to display the right number
  int idx = _spectrumDisplayForm->FFTSizeComboBox->findText(QString("%1").arg(_fftSize));
  _spectrumDisplayForm->FFTSizeComboBox->setCurrentIndex(idx);
}

void
SpectrumGUIClass::reset()
{
  if(getWindowOpenFlag()) {
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
SpectrumGUIClass::setDisplayTitle(const std::string newString)
{
  _title.assign(newString);

  if(getWindowOpenFlag()){
    qApp->postEvent(_spectrumDisplayForm,
		    new SpectrumWindowCaptionEvent(_title.c_str()));
  }
}

bool
SpectrumGUIClass::getWindowOpenFlag()
{
  gr::thread::scoped_lock lock(d_mutex);
  bool returnFlag = false;
  returnFlag =  _windowOpennedFlag;
  return returnFlag;
}


void
SpectrumGUIClass::setWindowOpenFlag(const bool newFlag)
{
  gr::thread::scoped_lock lock(d_mutex);
  _windowOpennedFlag = newFlag;
}

void
SpectrumGUIClass::setFrequencyRange(const double centerFreq,
				    const double startFreq,
				    const double stopFreq)
{
  gr::thread::scoped_lock lock(d_mutex);
  _centerFrequency = centerFreq;
  _startFrequency = startFreq;
  _stopFrequency = stopFreq;

  qApp->postEvent(_spectrumDisplayForm,
                  new SpectrumFrequencyRangeEvent(_centerFrequency,
                                                  _startFrequency,
                                                  _stopFrequency));
}

double
SpectrumGUIClass::getStartFrequency()
{
  gr::thread::scoped_lock lock(d_mutex);
  double returnValue = 0.0;
  returnValue =  _startFrequency;
  return returnValue;
}

double
SpectrumGUIClass::getStopFrequency()
{
  gr::thread::scoped_lock lock(d_mutex);
  double returnValue = 0.0;
  returnValue =  _stopFrequency;
  return returnValue;
}

double
SpectrumGUIClass::getCenterFrequency()
{
  gr::thread::scoped_lock lock(d_mutex);
  double returnValue = 0.0;
  returnValue =  _centerFrequency;
  return returnValue;
}


void
SpectrumGUIClass::updateWindow(const bool updateDisplayFlag,
			       const float* fftBuffer,
			       const uint64_t inputBufferSize,
			       const float* realTimeDomainData,
			       const uint64_t realTimeDomainDataSize,
			       const float* complexTimeDomainData,
			       const uint64_t complexTimeDomainDataSize,
			       const gr::high_res_timer_type timestamp,
			       const bool lastOfMultipleFFTUpdateFlag)
{
  //gr::thread::scoped_lock lock(d_mutex);
  int64_t bufferSize = inputBufferSize;
  bool repeatDataFlag = false;
  if(bufferSize > _dataPoints){
    bufferSize = _dataPoints;
  }
  int64_t timeDomainBufferSize = 0;

  if(updateDisplayFlag) {
    if((fftBuffer != NULL) && (bufferSize > 0)) {
      memcpy(_fftPoints, fftBuffer, bufferSize * sizeof(float));
    }

    //ALL OF THIS SHIT SHOULD BE COMBINED WITH THE FFTSHIFT
    //USE VOLK_32FC_DEINTERLEAVE_64F_X2_A TO GET REAL/IMAG FROM COMPLEX32
    // Can't do a memcpy since this is going from float to double data type
    if((realTimeDomainData != NULL) && (realTimeDomainDataSize > 0)){
      const float* realTimeDomainDataPtr = realTimeDomainData;

      double* realTimeDomainPointsPtr = _realTimeDomainPoints;
      timeDomainBufferSize = realTimeDomainDataSize;

      memset(_imagTimeDomainPoints, 0x0, realTimeDomainDataSize*sizeof(double));
      for(uint64_t number = 0; number < realTimeDomainDataSize; number++){
	*realTimeDomainPointsPtr++ = *realTimeDomainDataPtr++;
      }
    }

    if((complexTimeDomainData != NULL) && (complexTimeDomainDataSize > 0)) {
      volk_32fc_deinterleave_64f_x2(_realTimeDomainPoints,
                                    _imagTimeDomainPoints,
                                    (const lv_32fc_t *)complexTimeDomainData,
                                    complexTimeDomainDataSize);
      timeDomainBufferSize = complexTimeDomainDataSize;
    }
  }

  // If bufferSize is zero, then just update the display by sending over the old data
  if(bufferSize < 1) {
    bufferSize = _lastDataPointCount;
    repeatDataFlag = true;
  }
  else {
    // Since there is data this time, update the count
    _lastDataPointCount = bufferSize;
  }

  const gr::high_res_timer_type currentTime = gr::high_res_timer_now();
  const gr::high_res_timer_type lastUpdateGUITime = getLastGUIUpdateTime();

  if((currentTime - lastUpdateGUITime > (4*_updateTime)*gr::high_res_timer_tps()) &&
     (getPendingGUIUpdateEvents() > 0) && lastUpdateGUITime != 0) {
    // Do not update the display if too much data is pending to be displayed
    _droppedEntriesCount++;
  }
  else {
    // Draw the Data
    incrementPendingGUIUpdateEvents();
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
    if(!repeatDataFlag) {
      _droppedEntriesCount = 0;
    }
  }
}

float
SpectrumGUIClass::getPowerValue()
{
  gr::thread::scoped_lock lock(d_mutex);
  float returnValue = 0;
  returnValue = _powerValue;
  return returnValue;
}

void
SpectrumGUIClass::setPowerValue(const float value)
{
  gr::thread::scoped_lock lock(d_mutex);
  _powerValue = value;
}

int
SpectrumGUIClass::getWindowType()
{
  gr::thread::scoped_lock lock(d_mutex);
  int returnValue = 0;
  returnValue = _windowType;
  return returnValue;
}

void
SpectrumGUIClass::setWindowType(const int newType)
{
  gr::thread::scoped_lock lock(d_mutex);
  _windowType = newType;
}

int
SpectrumGUIClass::getFFTSize()
{
  int returnValue = 0;
  returnValue = _fftSize;
  return returnValue;
}

int
SpectrumGUIClass::getFFTSizeIndex()
{
  gr::thread::scoped_lock lock(d_mutex);
  int fftsize = getFFTSize();
  int rv = 0;
  switch(fftsize) {
  case(1024): rv = 0; break;
  case(2048): rv = 1; break;
  case(4096): rv = 2; break;
  case(8192): rv = 3; break;
  case(16384): rv = 3; break;
  case(32768): rv = 3; break;
  default: rv = 0; break;
  }
  return rv;
}

void
SpectrumGUIClass::setFFTSize(const int newSize)
{
  gr::thread::scoped_lock lock(d_mutex);
  _fftSize = newSize;
}

gr::high_res_timer_type
SpectrumGUIClass::getLastGUIUpdateTime()
{
  gr::thread::scoped_lock lock(d_mutex);
  gr::high_res_timer_type returnValue;
  returnValue = _lastGUIUpdateTime;
  return returnValue;
}

void
SpectrumGUIClass::setLastGUIUpdateTime(const gr::high_res_timer_type newTime)
{
  gr::thread::scoped_lock lock(d_mutex);
  _lastGUIUpdateTime = newTime;
}

unsigned int
SpectrumGUIClass::getPendingGUIUpdateEvents()
{
  gr::thread::scoped_lock lock(d_mutex);
  unsigned int returnValue = 0;
  returnValue = _pendingGUIUpdateEventsCount;
  return returnValue;
}

void
SpectrumGUIClass::incrementPendingGUIUpdateEvents()
{
  gr::thread::scoped_lock lock(d_mutex);
  _pendingGUIUpdateEventsCount++;
}

void
SpectrumGUIClass::decrementPendingGUIUpdateEvents()
{
  gr::thread::scoped_lock lock(d_mutex);
  if(_pendingGUIUpdateEventsCount > 0){
    _pendingGUIUpdateEventsCount--;
  }
}

void
SpectrumGUIClass::resetPendingGUIUpdateEvents()
{
  gr::thread::scoped_lock lock(d_mutex);
  _pendingGUIUpdateEventsCount = 0;
}


QWidget*
SpectrumGUIClass::qwidget()
{
  gr::thread::scoped_lock lock(d_mutex);
  return (QWidget*)_spectrumDisplayForm;
}

void
SpectrumGUIClass::setTimeDomainAxis(double min, double max)
{
  gr::thread::scoped_lock lock(d_mutex);
  _spectrumDisplayForm->setTimeDomainAxis(min, max);
}

void
SpectrumGUIClass::setConstellationAxis(double xmin, double xmax,
				       double ymin, double ymax)
{
  gr::thread::scoped_lock lock(d_mutex);
  _spectrumDisplayForm->setConstellationAxis(xmin, xmax, ymin, ymax);
}

void
SpectrumGUIClass::setConstellationPenSize(int size)
{
  gr::thread::scoped_lock lock(d_mutex);
  _spectrumDisplayForm->setConstellationPenSize(size);
}


void
SpectrumGUIClass::setFrequencyAxis(double min, double max)
{
  gr::thread::scoped_lock lock(d_mutex);
  _spectrumDisplayForm->setFrequencyAxis(min, max);
}

void
SpectrumGUIClass::setUpdateTime(double t)
{
  gr::thread::scoped_lock lock(d_mutex);
  _updateTime = t;
  _spectrumDisplayForm->setUpdateTime(_updateTime);
}

void
SpectrumGUIClass::enableRFFreq(bool en)
{
  gr::thread::scoped_lock lock(d_mutex);
  _spectrumDisplayForm->toggleRFFrequencies(en);
}

bool
SpectrumGUIClass::checkClicked()
{
  gr::thread::scoped_lock lock(d_mutex);
  return _spectrumDisplayForm->checkClicked();
}

float
SpectrumGUIClass::getClickedFreq()
{
  gr::thread::scoped_lock lock(d_mutex);
  return _spectrumDisplayForm->getClickedFreq();
}

#endif /* SPECTRUM_GUI_CLASS_CPP */

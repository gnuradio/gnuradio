/* -*- c++ -*- */
/*
 * Copyright 2008-2011 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef SPECTRUM_GUI_CLASS_CPP
#define SPECTRUM_GUI_CLASS_CPP

#include <gnuradio/qtgui/SpectrumGUIClass.h>

// Added by qt3to4:
#include <volk/volk.h>
#include <QEvent>

const long SpectrumGUIClass::MAX_FFT_SIZE = 32768;
const long SpectrumGUIClass::MIN_FFT_SIZE = 256;

SpectrumGUIClass::SpectrumGUIClass(const uint64_t maxDataSize,
                                   const uint64_t fftSize,
                                   const double newCenterFrequency,
                                   const double newStartFrequency,
                                   const double newStopFrequency)
    : _dataPoints(std::max(static_cast<int64_t>(maxDataSize), static_cast<int64_t>(2))),
      _centerFrequency(newCenterFrequency),
      _startFrequency(newStartFrequency),
      _stopFrequency(newStopFrequency),
      _lastDataPointCount(_dataPoints),
      _fftSize(fftSize)
{
}

SpectrumGUIClass::~SpectrumGUIClass()
{
    // We don't need to delete this since as a QWidget, it is supposed to be destroyed
    // with it's parent. Deleting it causes a segmentation fault, and not deleting it
    // does not leave any extra memory.
    // if(getWindowOpenFlag()){
    // delete _spectrumDisplayForm;
    //}
}

void SpectrumGUIClass::openSpectrumWindow(QWidget* parent,
                                          const bool frequency,
                                          const bool waterfall,
                                          const bool time,
                                          const bool constellation)
{
    d_mutex.lock();

    if (!_windowOpennedFlag) {

        if (_fftPoints.empty()) {
            _fftPoints.resize(_dataPoints);
            _realTimeDomainPoints.resize(_dataPoints);
            _imagTimeDomainPoints.resize(_dataPoints);
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

    qApp->postEvent(_spectrumDisplayForm, new QEvent(QEvent::Type(QEvent::User + 3)));

    _lastGUIUpdateTime = 0;

    // Draw Blank Display
    updateWindow(false, NULL, 0, NULL, 0, NULL, 0, gr::high_res_timer_now(), true);

    // Set up the initial frequency axis settings
    setFrequencyRange(_centerFrequency, _startFrequency, _stopFrequency);

    // GUI Thread only
    qApp->processEvents();

    // Set the FFT Size combo box to display the right number
    int idx =
        _spectrumDisplayForm->FFTSizeComboBox->findText(QString("%1").arg(_fftSize));
    _spectrumDisplayForm->FFTSizeComboBox->setCurrentIndex(idx);
}

void SpectrumGUIClass::reset()
{
    if (getWindowOpenFlag()) {
        qApp->postEvent(_spectrumDisplayForm,
                        new SpectrumFrequencyRangeEvent(
                            _centerFrequency, _startFrequency, _stopFrequency));
        qApp->postEvent(_spectrumDisplayForm, new SpectrumWindowResetEvent());
    }
    _droppedEntriesCount = 0;
    // Call the following function from the Spectrum Window Reset Event window
    // ResetPendingGUIUpdateEvents();
}

void SpectrumGUIClass::setDisplayTitle(const std::string newString)
{
    _title.assign(newString);

    if (getWindowOpenFlag()) {
        qApp->postEvent(_spectrumDisplayForm,
                        new SpectrumWindowCaptionEvent(_title.c_str()));
    }
}

bool SpectrumGUIClass::getWindowOpenFlag()
{
    gr::thread::scoped_lock lock(d_mutex);
    return _windowOpennedFlag;
}


void SpectrumGUIClass::setWindowOpenFlag(const bool newFlag)
{
    gr::thread::scoped_lock lock(d_mutex);
    _windowOpennedFlag = newFlag;
}

void SpectrumGUIClass::setFrequencyRange(const double centerFreq,
                                         const double startFreq,
                                         const double stopFreq)
{
    gr::thread::scoped_lock lock(d_mutex);
    _centerFrequency = centerFreq;
    _startFrequency = startFreq;
    _stopFrequency = stopFreq;

    qApp->postEvent(_spectrumDisplayForm,
                    new SpectrumFrequencyRangeEvent(
                        _centerFrequency, _startFrequency, _stopFrequency));
}

double SpectrumGUIClass::getStartFrequency()
{
    gr::thread::scoped_lock lock(d_mutex);
    double returnValue = 0.0;
    returnValue = _startFrequency;
    return returnValue;
}

double SpectrumGUIClass::getStopFrequency()
{
    gr::thread::scoped_lock lock(d_mutex);
    double returnValue = 0.0;
    returnValue = _stopFrequency;
    return returnValue;
}

double SpectrumGUIClass::getCenterFrequency()
{
    gr::thread::scoped_lock lock(d_mutex);
    double returnValue = 0.0;
    returnValue = _centerFrequency;
    return returnValue;
}


void SpectrumGUIClass::updateWindow(const bool updateDisplayFlag,
                                    const float* fftBuffer,
                                    const uint64_t inputBufferSize,
                                    const float* realTimeDomainData,
                                    const uint64_t realTimeDomainDataSize,
                                    const float* complexTimeDomainData,
                                    const uint64_t complexTimeDomainDataSize,
                                    const gr::high_res_timer_type timestamp,
                                    const bool lastOfMultipleFFTUpdateFlag)
{
    // gr::thread::scoped_lock lock(d_mutex);
    int64_t bufferSize = inputBufferSize;
    bool repeatDataFlag = false;
    if (bufferSize > _dataPoints) {
        bufferSize = _dataPoints;
    }
    int64_t timeDomainBufferSize = 0;

    if (updateDisplayFlag) {
        if ((fftBuffer != NULL) && (bufferSize > 0)) {
            std::copy(fftBuffer, fftBuffer + bufferSize, std::begin(_fftPoints));
        }

        // ALL OF THIS SHIT SHOULD BE COMBINED WITH THE FFTSHIFT
        // USE VOLK_32FC_DEINTERLEAVE_64F_X2_A TO GET REAL/IMAG FROM COMPLEX32
        // Can't do a memcpy since this is going from float to double data type
        if ((realTimeDomainData != NULL) && (realTimeDomainDataSize > 0)) {
            const float* realTimeDomainDataPtr = realTimeDomainData;

            double* realTimeDomainPointsPtr = _realTimeDomainPoints.data();
            timeDomainBufferSize = realTimeDomainDataSize;

            std::fill(
                std::begin(_imagTimeDomainPoints), std::end(_imagTimeDomainPoints), 0.0);
            for (uint64_t number = 0; number < realTimeDomainDataSize; number++) {
                *realTimeDomainPointsPtr++ = *realTimeDomainDataPtr++;
            }
        }

        if ((complexTimeDomainData != NULL) && (complexTimeDomainDataSize > 0)) {
            volk_32fc_deinterleave_64f_x2(_realTimeDomainPoints.data(),
                                          _imagTimeDomainPoints.data(),
                                          (const lv_32fc_t*)complexTimeDomainData,
                                          complexTimeDomainDataSize);
            timeDomainBufferSize = complexTimeDomainDataSize;
        }
    }

    // If bufferSize is zero, then just update the display by sending over the old data
    if (bufferSize < 1) {
        bufferSize = _lastDataPointCount;
        repeatDataFlag = true;
    } else {
        // Since there is data this time, update the count
        _lastDataPointCount = bufferSize;
    }

    const gr::high_res_timer_type currentTime = gr::high_res_timer_now();
    const gr::high_res_timer_type lastUpdateGUITime = getLastGUIUpdateTime();

    if ((currentTime - lastUpdateGUITime >
         (4 * _updateTime) * gr::high_res_timer_tps()) &&
        (getPendingGUIUpdateEvents() > 0) && lastUpdateGUITime != 0) {
        // Do not update the display if too much data is pending to be displayed
        _droppedEntriesCount++;
    } else {
        // Draw the Data
        incrementPendingGUIUpdateEvents();
        qApp->postEvent(_spectrumDisplayForm,
                        new SpectrumUpdateEvent(_fftPoints.data(),
                                                bufferSize,
                                                _realTimeDomainPoints.data(),
                                                _imagTimeDomainPoints.data(),
                                                timeDomainBufferSize,
                                                timestamp,
                                                repeatDataFlag,
                                                lastOfMultipleFFTUpdateFlag,
                                                currentTime,
                                                _droppedEntriesCount));

        // Only reset the dropped entries counter if this is not
        // repeat data since repeat data is dropped by the display systems
        if (!repeatDataFlag) {
            _droppedEntriesCount = 0;
        }
    }
}

float SpectrumGUIClass::getPowerValue()
{
    gr::thread::scoped_lock lock(d_mutex);
    float returnValue = 0;
    returnValue = _powerValue;
    return returnValue;
}

void SpectrumGUIClass::setPowerValue(const float value)
{
    gr::thread::scoped_lock lock(d_mutex);
    _powerValue = value;
}

int SpectrumGUIClass::getWindowType()
{
    gr::thread::scoped_lock lock(d_mutex);
    int returnValue = 0;
    returnValue = _windowType;
    return returnValue;
}

void SpectrumGUIClass::setWindowType(const int newType)
{
    gr::thread::scoped_lock lock(d_mutex);
    _windowType = newType;
}

int SpectrumGUIClass::getFFTSize()
{
    int returnValue = 0;
    returnValue = _fftSize;
    return returnValue;
}

int SpectrumGUIClass::getFFTSizeIndex()
{
    gr::thread::scoped_lock lock(d_mutex);
    int fftsize = getFFTSize();
    int rv = 0;
    switch (fftsize) {
    case (1024):
        rv = 0;
        break;
    case (2048):
        rv = 1;
        break;
    case (4096):
        rv = 2;
        break;
    case (8192):
        rv = 3;
        break;
    case (16384):
        rv = 3;
        break;
    case (32768):
        rv = 3;
        break;
    default:
        rv = 0;
        break;
    }
    return rv;
}

void SpectrumGUIClass::setFFTSize(const int newSize)
{
    gr::thread::scoped_lock lock(d_mutex);
    _fftSize = newSize;
}

gr::high_res_timer_type SpectrumGUIClass::getLastGUIUpdateTime()
{
    gr::thread::scoped_lock lock(d_mutex);
    gr::high_res_timer_type returnValue;
    returnValue = _lastGUIUpdateTime;
    return returnValue;
}

void SpectrumGUIClass::setLastGUIUpdateTime(const gr::high_res_timer_type newTime)
{
    gr::thread::scoped_lock lock(d_mutex);
    _lastGUIUpdateTime = newTime;
}

unsigned int SpectrumGUIClass::getPendingGUIUpdateEvents()
{
    gr::thread::scoped_lock lock(d_mutex);
    unsigned int returnValue = 0;
    returnValue = _pendingGUIUpdateEventsCount;
    return returnValue;
}

void SpectrumGUIClass::incrementPendingGUIUpdateEvents()
{
    gr::thread::scoped_lock lock(d_mutex);
    _pendingGUIUpdateEventsCount++;
}

void SpectrumGUIClass::decrementPendingGUIUpdateEvents()
{
    gr::thread::scoped_lock lock(d_mutex);
    if (_pendingGUIUpdateEventsCount > 0) {
        _pendingGUIUpdateEventsCount--;
    }
}

void SpectrumGUIClass::resetPendingGUIUpdateEvents()
{
    gr::thread::scoped_lock lock(d_mutex);
    _pendingGUIUpdateEventsCount = 0;
}


QWidget* SpectrumGUIClass::qwidget()
{
    gr::thread::scoped_lock lock(d_mutex);
    return (QWidget*)_spectrumDisplayForm;
}

void SpectrumGUIClass::setTimeDomainAxis(double min, double max)
{
    gr::thread::scoped_lock lock(d_mutex);
    _spectrumDisplayForm->setTimeDomainAxis(min, max);
}

void SpectrumGUIClass::setConstellationAxis(double xmin,
                                            double xmax,
                                            double ymin,
                                            double ymax)
{
    gr::thread::scoped_lock lock(d_mutex);
    _spectrumDisplayForm->setConstellationAxis(xmin, xmax, ymin, ymax);
}

void SpectrumGUIClass::setConstellationPenSize(int size)
{
    gr::thread::scoped_lock lock(d_mutex);
    _spectrumDisplayForm->setConstellationPenSize(size);
}


void SpectrumGUIClass::setFrequencyAxis(double min, double max)
{
    gr::thread::scoped_lock lock(d_mutex);
    _spectrumDisplayForm->setFrequencyAxis(min, max);
}

void SpectrumGUIClass::setUpdateTime(double t)
{
    gr::thread::scoped_lock lock(d_mutex);
    _updateTime = t;
    _spectrumDisplayForm->setUpdateTime(_updateTime);
}

void SpectrumGUIClass::enableRFFreq(bool en)
{
    gr::thread::scoped_lock lock(d_mutex);
    _spectrumDisplayForm->toggleRFFrequencies(en);
}

bool SpectrumGUIClass::checkClicked()
{
    gr::thread::scoped_lock lock(d_mutex);
    return _spectrumDisplayForm->checkClicked();
}

float SpectrumGUIClass::getClickedFreq()
{
    gr::thread::scoped_lock lock(d_mutex);
    return _spectrumDisplayForm->getClickedFreq();
}

#endif /* SPECTRUM_GUI_CLASS_CPP */

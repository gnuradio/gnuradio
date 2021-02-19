/* -*- c++ -*- */
/*
 * Copyright 2008-2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef SPECTRUM_UPDATE_EVENTS_C
#define SPECTRUM_UPDATE_EVENTS_C

#include <gnuradio/qtgui/spectrumUpdateEvents.h>

SpectrumUpdateEvent::SpectrumUpdateEvent(const float* fft_points,
                                         const uint64_t numFFTDataPoints,
                                         const double* real_time_domain_points,
                                         const double* imag_time_domain_points,
                                         const uint64_t num_time_domain_points,
                                         const gr::high_res_timer_type dataTimestamp,
                                         const bool repeatDataFlag,
                                         const bool lastOfMultipleUpdateFlag,
                                         const gr::high_res_timer_type generatedTimestamp,
                                         const int droppedFFTFrames)
    : QEvent(QEvent::Type(SpectrumUpdateEventType)),
      d_fft_points(fft_points, fft_points + numFFTDataPoints),
      d_real_data_time_domain_points(real_time_domain_points,
                                     real_time_domain_points + num_time_domain_points),
      d_imag_data_time_domain_points(imag_time_domain_points,
                                     imag_time_domain_points + num_time_domain_points)
{
    if (d_fft_points.empty()) {
        d_fft_points.resize(1);
    }

    if (num_time_domain_points == 0) {
        d_real_data_time_domain_points.resize(1);
        d_imag_data_time_domain_points.resize(1);
    }

    _dataTimestamp = dataTimestamp;
    _repeatDataFlag = repeatDataFlag;
    _lastOfMultipleUpdateFlag = lastOfMultipleUpdateFlag;
    _eventGeneratedTimestamp = generatedTimestamp;
    _droppedFFTFrames = droppedFFTFrames;
}

SpectrumUpdateEvent::~SpectrumUpdateEvent() {}

const float* SpectrumUpdateEvent::getFFTPoints() const { return d_fft_points.data(); }

const double* SpectrumUpdateEvent::getRealTimeDomainPoints() const
{
    return d_real_data_time_domain_points.data();
}

const double* SpectrumUpdateEvent::getImagTimeDomainPoints() const
{
    return d_imag_data_time_domain_points.data();
}

uint64_t SpectrumUpdateEvent::getNumFFTDataPoints() const { return d_fft_points.size(); }

uint64_t SpectrumUpdateEvent::getNumTimeDomainDataPoints() const
{
    return d_real_data_time_domain_points.size();
}

gr::high_res_timer_type SpectrumUpdateEvent::getDataTimestamp() const
{
    return _dataTimestamp;
}

bool SpectrumUpdateEvent::getRepeatDataFlag() const { return _repeatDataFlag; }

bool SpectrumUpdateEvent::getLastOfMultipleUpdateFlag() const
{
    return _lastOfMultipleUpdateFlag;
}

gr::high_res_timer_type SpectrumUpdateEvent::getEventGeneratedTimestamp() const
{
    return _eventGeneratedTimestamp;
}

int SpectrumUpdateEvent::getDroppedFFTFrames() const { return _droppedFFTFrames; }

SpectrumWindowCaptionEvent::SpectrumWindowCaptionEvent(const QString& newLbl)
    : QEvent(QEvent::Type(SpectrumWindowCaptionEventType))
{
    _labelString = newLbl;
}

SpectrumWindowCaptionEvent::~SpectrumWindowCaptionEvent() {}

QString SpectrumWindowCaptionEvent::getLabel() { return _labelString; }

SpectrumWindowResetEvent::SpectrumWindowResetEvent()
    : QEvent(QEvent::Type(SpectrumWindowResetEventType))
{
}

SpectrumWindowResetEvent::~SpectrumWindowResetEvent() {}

SpectrumFrequencyRangeEvent::SpectrumFrequencyRangeEvent(const double centerFreq,
                                                         const double startFreq,
                                                         const double stopFreq)
    : QEvent(QEvent::Type(SpectrumFrequencyRangeEventType))
{
    _centerFrequency = centerFreq;
    _startFrequency = startFreq;
    _stopFrequency = stopFreq;
}

SpectrumFrequencyRangeEvent::~SpectrumFrequencyRangeEvent() {}

double SpectrumFrequencyRangeEvent::GetCenterFrequency() const
{
    return _centerFrequency;
}

double SpectrumFrequencyRangeEvent::GetStartFrequency() const { return _startFrequency; }

double SpectrumFrequencyRangeEvent::GetStopFrequency() const { return _stopFrequency; }


/***************************************************************************/


TimeUpdateEvent::TimeUpdateEvent(const std::vector<volk::vector<double>> timeDomainPoints,
                                 const uint64_t numTimeDomainDataPoints,
                                 const std::vector<std::vector<gr::tag_t>> tags)
    : QEvent(QEvent::Type(SpectrumUpdateEventType))
{
    if (numTimeDomainDataPoints < 1) {
        _numTimeDomainDataPoints = 1;
    } else {
        _numTimeDomainDataPoints = numTimeDomainDataPoints;
    }

    // TODO: make this whole thing a vector copy.
    _nplots = timeDomainPoints.size();
    for (size_t i = 0; i < _nplots; i++) {
        _dataTimeDomainPoints.push_back(new double[_numTimeDomainDataPoints]);
        if (numTimeDomainDataPoints > 0) {
            memcpy(_dataTimeDomainPoints[i],
                   timeDomainPoints[i].data(),
                   _numTimeDomainDataPoints * sizeof(double));
        }
    }

    _tags = tags;
}

TimeUpdateEvent::~TimeUpdateEvent()
{
    for (size_t i = 0; i < _nplots; i++) {
        delete[] _dataTimeDomainPoints[i];
    }
}

const std::vector<double*> TimeUpdateEvent::getTimeDomainPoints() const
{
    return _dataTimeDomainPoints;
}

uint64_t TimeUpdateEvent::getNumTimeDomainDataPoints() const
{
    return _numTimeDomainDataPoints;
}

const std::vector<std::vector<gr::tag_t>> TimeUpdateEvent::getTags() const
{
    return _tags;
}

/***************************************************************************/


FreqUpdateEvent::FreqUpdateEvent(const std::vector<volk::vector<double>> dataPoints,
                                 const uint64_t numDataPoints)
    : QEvent(QEvent::Type(SpectrumUpdateEventType))
{
    if (numDataPoints < 1) {
        _numDataPoints = 1;
    } else {
        _numDataPoints = numDataPoints;
    }

    _nplots = dataPoints.size();
    // TODO: do a vector copy.
    for (size_t i = 0; i < _nplots; i++) {
        _dataPoints.push_back(new double[_numDataPoints]);
        if (numDataPoints > 0) {
            memcpy(_dataPoints[i], dataPoints[i].data(), _numDataPoints * sizeof(double));
        }
    }
}

FreqUpdateEvent::~FreqUpdateEvent()
{
    for (size_t i = 0; i < _nplots; i++) {
        delete[] _dataPoints[i];
    }
}

const std::vector<double*> FreqUpdateEvent::getPoints() const { return _dataPoints; }

uint64_t FreqUpdateEvent::getNumDataPoints() const { return _numDataPoints; }


SetFreqEvent::SetFreqEvent(const double centerFreq, const double bandwidth)
    : QEvent(QEvent::Type(SpectrumFrequencyRangeEventType))
{
    _centerFrequency = centerFreq;
    _bandwidth = bandwidth;
}

SetFreqEvent::~SetFreqEvent() {}

double SetFreqEvent::getCenterFrequency() const { return _centerFrequency; }

double SetFreqEvent::getBandwidth() const { return _bandwidth; }


/***************************************************************************/


ConstUpdateEvent::ConstUpdateEvent(const std::vector<volk::vector<double>> realDataPoints,
                                   const std::vector<volk::vector<double>> imagDataPoints,
                                   const uint64_t numDataPoints)
    : QEvent(QEvent::Type(SpectrumUpdateEventType))
{
    if (numDataPoints < 1) {
        _numDataPoints = 1;
    } else {
        _numDataPoints = numDataPoints;
    }

    // Todo: do vector copies.
    _nplots = realDataPoints.size();
    for (size_t i = 0; i < _nplots; i++) {
        _realDataPoints.push_back(new double[_numDataPoints]);
        _imagDataPoints.push_back(new double[_numDataPoints]);
        if (numDataPoints > 0) {
            memcpy(_realDataPoints[i],
                   realDataPoints[i].data(),
                   _numDataPoints * sizeof(double));
            memcpy(_imagDataPoints[i],
                   imagDataPoints[i].data(),
                   _numDataPoints * sizeof(double));
        }
    }
}

ConstUpdateEvent::~ConstUpdateEvent()
{
    for (size_t i = 0; i < _nplots; i++) {
        delete[] _realDataPoints[i];
        delete[] _imagDataPoints[i];
    }
}

const std::vector<double*> ConstUpdateEvent::getRealPoints() const
{
    return _realDataPoints;
}

const std::vector<double*> ConstUpdateEvent::getImagPoints() const
{
    return _imagDataPoints;
}

uint64_t ConstUpdateEvent::getNumDataPoints() const { return _numDataPoints; }


/***************************************************************************/


WaterfallUpdateEvent::WaterfallUpdateEvent(
    const std::vector<volk::vector<double>> dataPoints,
    const uint64_t numDataPoints,
    const gr::high_res_timer_type dataTimestamp)
    : QEvent(QEvent::Type(SpectrumUpdateEventType))
{
    if (numDataPoints < 1) {
        _numDataPoints = 1;
    } else {
        _numDataPoints = numDataPoints;
    }

    _nplots = dataPoints.size();
    for (size_t i = 0; i < _nplots; i++) {
        _dataPoints.push_back(new double[_numDataPoints]);
        if (numDataPoints > 0) {
            memcpy(_dataPoints[i], dataPoints[i].data(), _numDataPoints * sizeof(double));
        }
    }

    _dataTimestamp = dataTimestamp;
}

WaterfallUpdateEvent::~WaterfallUpdateEvent()
{
    for (size_t i = 0; i < _nplots; i++) {
        delete[] _dataPoints[i];
    }
}

const std::vector<double*> WaterfallUpdateEvent::getPoints() const { return _dataPoints; }

uint64_t WaterfallUpdateEvent::getNumDataPoints() const { return _numDataPoints; }

gr::high_res_timer_type WaterfallUpdateEvent::getDataTimestamp() const
{
    return _dataTimestamp;
}


/***************************************************************************/


TimeRasterUpdateEvent::TimeRasterUpdateEvent(
    const std::vector<volk::vector<double>> dataPoints, const uint64_t numDataPoints)
    : QEvent(QEvent::Type(SpectrumUpdateEventType))
{
    if (numDataPoints < 1) {
        _numDataPoints = 1;
    } else {
        _numDataPoints = numDataPoints;
    }

    _nplots = dataPoints.size();
    for (size_t i = 0; i < _nplots; i++) {
        _dataPoints.push_back(new double[_numDataPoints]);
        if (numDataPoints > 0) {
            memcpy(_dataPoints[i], dataPoints[i].data(), _numDataPoints * sizeof(double));
        }
    }
}

TimeRasterUpdateEvent::~TimeRasterUpdateEvent()
{
    for (size_t i = 0; i < _nplots; i++) {
        delete[] _dataPoints[i];
    }
}

const std::vector<double*> TimeRasterUpdateEvent::getPoints() const
{
    return _dataPoints;
}

uint64_t TimeRasterUpdateEvent::getNumDataPoints() const { return _numDataPoints; }


TimeRasterSetSize::TimeRasterSetSize(const double nrows, const double ncols)
    : QEvent(QEvent::Type(SpectrumUpdateEventType + 1)), _nrows(nrows), _ncols(ncols)
{
}

TimeRasterSetSize::~TimeRasterSetSize() {}

double TimeRasterSetSize::nRows() const { return _nrows; }

double TimeRasterSetSize::nCols() const { return _ncols; }

/***************************************************************************/


HistogramUpdateEvent::HistogramUpdateEvent(const std::vector<volk::vector<double>> points,
                                           const uint64_t npoints)
    : QEvent(QEvent::Type(SpectrumUpdateEventType))
{
    if (npoints < 1) {
        _npoints = 1;
    } else {
        _npoints = npoints;
    }

    _nplots = points.size();
    for (size_t i = 0; i < _nplots; i++) {
        _points.push_back(new double[_npoints]);
        if (npoints > 0) {
            memcpy(_points[i], points[i].data(), _npoints * sizeof(double));
        }
    }
}

HistogramUpdateEvent::~HistogramUpdateEvent()
{
    for (size_t i = 0; i < _nplots; i++) {
        delete[] _points[i];
    }
}

const std::vector<double*> HistogramUpdateEvent::getDataPoints() const { return _points; }

uint64_t HistogramUpdateEvent::getNumDataPoints() const { return _npoints; }


HistogramSetAccumulator::HistogramSetAccumulator(const bool en)
    : QEvent(QEvent::Type(SpectrumUpdateEventType + 1)), _en(en)
{
}

HistogramSetAccumulator::~HistogramSetAccumulator() {}

bool HistogramSetAccumulator::getAccumulator() const { return _en; }


/***************************************************************************/


NumberUpdateEvent::NumberUpdateEvent(const std::vector<float> samples)
    : QEvent(QEvent::Type(SpectrumUpdateEventType))
{
    _samples = samples;
    _nplots = samples.size();
}

NumberUpdateEvent::~NumberUpdateEvent() {}

const std::vector<float> NumberUpdateEvent::getSamples() const { return _samples; }


#endif /* SPECTRUM_UPDATE_EVENTS_C */

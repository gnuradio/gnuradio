/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef WATERFALL_GLOBAL_DATA_CPP
#define WATERFALL_GLOBAL_DATA_CPP

#include <gnuradio/qtgui/waterfallGlobalData.h>

#include <cstdio>

WaterfallData::WaterfallData(const double minimumFrequency,
                             const double maximumFrequency,
                             const uint64_t fftPoints,
                             const unsigned int historyExtent)
#if QWT_VERSION < 0x060000
    : QwtRasterData(QwtDoubleRect(minimumFrequency /* X START */,
                                  0 /* Y START */,
                                  maximumFrequency - minimumFrequency /* WIDTH */,
                                  static_cast<double>(historyExtent) /* HEIGHT */)),
#else
    : QwtRasterData(),
#endif
      _spectrumData(fftPoints * historyExtent),
      _fftPoints(fftPoints),
      _historyLength(historyExtent),
      _intensityRange(QwtDoubleInterval(-200.0, 0.0))
{
#if QWT_VERSION >= 0x060000
    setInterval(Qt::XAxis, QwtInterval(minimumFrequency, maximumFrequency));
    setInterval(Qt::YAxis, QwtInterval(0, historyExtent));
    setInterval(Qt::ZAxis, QwtInterval(-200, 0.0));
#endif

    reset();
}

WaterfallData::~WaterfallData() {}

void WaterfallData::reset()
{
    std::fill(std::begin(_spectrumData), std::end(_spectrumData), 0.0);

    _numLinesToUpdate = -1;
}

void WaterfallData::copy(const WaterfallData* rhs)
{
#if QWT_VERSION < 0x060000
    if ((_fftPoints != rhs->getNumFFTPoints()) ||
        (boundingRect() != rhs->boundingRect())) {
        _fftPoints = rhs->getNumFFTPoints();
        setBoundingRect(rhs->boundingRect());
        _spectrumData.resize(_fftPoints * _historyLength);
    }
#else
    if (_fftPoints != rhs->getNumFFTPoints()) {
        _fftPoints = rhs->getNumFFTPoints();
        _spectrumData.resize(_fftPoints * _historyLength);
    }
#endif

    reset();
    setSpectrumDataBuffer(rhs->getSpectrumDataBuffer());
    setNumLinesToUpdate(rhs->getNumLinesToUpdate());

#if QWT_VERSION < 0x060000
    setRange(rhs->range());
#else
    setInterval(Qt::XAxis, rhs->interval(Qt::XAxis));
    setInterval(Qt::YAxis, rhs->interval(Qt::YAxis));
    setInterval(Qt::ZAxis, rhs->interval(Qt::ZAxis));
#endif
}

void WaterfallData::resizeData(const double startFreq,
                               const double stopFreq,
                               const uint64_t fftPoints,
                               const int history)
{
    if (history > 0) {
        _historyLength = history;
    }

#if QWT_VERSION < 0x060000
    if ((fftPoints != getNumFFTPoints()) ||
        (boundingRect().width() != (stopFreq - startFreq)) ||
        (boundingRect().left() != startFreq)) {

        setBoundingRect(QwtDoubleRect(
            startFreq, 0, stopFreq - startFreq, static_cast<double>(_historyLength)));
        _fftPoints = fftPoints;
        _spectrumData.resize(_fftPoints * _historyLength);
    }

#else
    if ((fftPoints != getNumFFTPoints()) ||
        (interval(Qt::XAxis).width() != (stopFreq - startFreq)) ||
        (interval(Qt::XAxis).minValue() != startFreq)) {

        setInterval(Qt::XAxis, QwtInterval(startFreq, stopFreq));
        setInterval(Qt::YAxis, QwtInterval(0, _historyLength));

        _fftPoints = fftPoints;
        _spectrumData.resize(_fftPoints * _historyLength);
    }
#endif

    reset();
}

QwtRasterData* WaterfallData::copy() const
{
#if QWT_VERSION < 0x060000
    WaterfallData* returnData = new WaterfallData(
        boundingRect().left(), boundingRect().right(), _fftPoints, _historyLength);
#else
    WaterfallData* returnData = new WaterfallData(interval(Qt::XAxis).minValue(),
                                                  interval(Qt::XAxis).maxValue(),
                                                  _fftPoints,
                                                  _historyLength);
#endif

    returnData->copy(this);
    return returnData;
}


#if QWT_VERSION < 0x060000
QwtDoubleInterval WaterfallData::range() const { return _intensityRange; }

void WaterfallData::setRange(const QwtDoubleInterval& newRange)
{
    _intensityRange = newRange;
}

#endif


double WaterfallData::value(double x, double y) const
{
    double returnValue = 0.0;

#if QWT_VERSION < 0x060000
    const unsigned int intY = static_cast<unsigned int>(
        (1.0 - (y / boundingRect().height())) * static_cast<double>(_historyLength - 1));
    const unsigned int intX = static_cast<unsigned int>(
        (((x - boundingRect().left()) / boundingRect().width()) *
         static_cast<double>(_fftPoints - 1)) +
        0.5);
#else
    double height = interval(Qt::YAxis).maxValue();
    double left = interval(Qt::XAxis).minValue();
    double right = interval(Qt::XAxis).maxValue();
    double ylen = static_cast<double>(_historyLength - 1);
    double xlen = static_cast<double>(_fftPoints - 1);
    const unsigned int intY = static_cast<unsigned int>((1.0 - y / height) * ylen);
    const unsigned int intX =
        static_cast<unsigned int>((((x - left) / (right - left)) * xlen) + 0.5);
#endif

    const int location = (intY * _fftPoints) + intX;
    if ((location > -1) &&
        (location < static_cast<int64_t>(_fftPoints * _historyLength))) {
        returnValue = _spectrumData[location];
    }

    return returnValue;
}

uint64_t WaterfallData::getNumFFTPoints() const { return _fftPoints; }

void WaterfallData::addFFTData(const double* fftData,
                               const uint64_t fftDataSize,
                               const int droppedFrames)
{
    if (fftDataSize == _fftPoints) {
        int64_t heightOffset = _historyLength - 1 - droppedFrames;
        uint64_t drawingDroppedFrames = droppedFrames;

        // Any valid data rolled off the display so just fill in zeros and write new data
        if (heightOffset < 0) {
            heightOffset = 0;
            drawingDroppedFrames = static_cast<uint64_t>(_historyLength - 1);
        }

        // Copy the old data over if any available
        if (heightOffset > 0) {
            memmove(_spectrumData.data(),
                    &_spectrumData[(drawingDroppedFrames + 1) * _fftPoints],
                    heightOffset * _fftPoints * sizeof(double));
        }

        if (drawingDroppedFrames > 0) {
            // Fill in zeros data for dropped data
            memset(&_spectrumData[heightOffset * _fftPoints],
                   0x00,
                   static_cast<int64_t>(drawingDroppedFrames) * _fftPoints *
                       sizeof(double));
        }

        // add the new buffer
        memcpy(&_spectrumData[(_historyLength - 1) * _fftPoints],
               fftData,
               _fftPoints * sizeof(double));
    }
}

const double* WaterfallData::getSpectrumDataBuffer() const
{
    return _spectrumData.data();
}

void WaterfallData::setSpectrumDataBuffer(const double* newData)
{
    memcpy(_spectrumData.data(), newData, _fftPoints * _historyLength * sizeof(double));
}

int WaterfallData::getNumLinesToUpdate() const { return _numLinesToUpdate; }

void WaterfallData::setNumLinesToUpdate(const int newNum) { _numLinesToUpdate = newNum; }

void WaterfallData::incrementNumLinesToUpdate() { _numLinesToUpdate++; }

#endif /* WATERFALL_GLOBAL_DATA_CPP */

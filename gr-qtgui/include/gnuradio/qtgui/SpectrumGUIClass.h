/* -*- c++ -*- */
/*
 * Copyright 2008-2011 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef SPECTRUM_GUI_CLASS_HPP
#define SPECTRUM_GUI_CLASS_HPP

#include <gnuradio/qtgui/spectrumUpdateEvents.h>
#include <gnuradio/thread/thread.h>
#include <qapplication.h>
#include <qlabel.h>
#include <qslider.h>
#include <qwidget.h>

class SpectrumDisplayForm;
#include <gnuradio/qtgui/spectrumdisplayform.h>

#include <cmath>

#include <complex>
#include <string>
#include <vector>

/*!
 * \brief QWidget class for controlling plotting.
 * \ingroup qtgui_blk
 */
class SpectrumGUIClass
{
public:
    SpectrumGUIClass(const uint64_t maxDataSize,
                     const uint64_t fftSize,
                     const double newCenterFrequency,
                     const double newStartFrequency,
                     const double newStopFrequency);
    ~SpectrumGUIClass();

    // Disable copy/move because of raw QT pointers.
    SpectrumGUIClass(const SpectrumGUIClass&) = delete;
    SpectrumGUIClass(SpectrumGUIClass&&) = delete;
    SpectrumGUIClass& operator=(const SpectrumGUIClass&) = delete;
    SpectrumGUIClass& operator=(SpectrumGUIClass&&) = delete;

    void reset();

    void openSpectrumWindow(QWidget*,
                            const bool frequency = true,
                            const bool waterfall = true,
                            const bool time = true,
                            const bool constellation = true);
    void setDisplayTitle(const std::string);

    bool getWindowOpenFlag();
    void setWindowOpenFlag(const bool);

    void setFrequencyRange(const double, const double, const double);
    double getStartFrequency();
    double getStopFrequency();
    double getCenterFrequency();

    void updateWindow(const bool,
                      const float*,
                      const uint64_t,
                      const float*,
                      const uint64_t,
                      const float*,
                      const uint64_t,
                      const gr::high_res_timer_type,
                      const bool);

    float getPowerValue();
    void setPowerValue(const float);

    int getWindowType();
    void setWindowType(const int);

    int getFFTSize();
    int getFFTSizeIndex();
    void setFFTSize(const int);

    gr::high_res_timer_type getLastGUIUpdateTime();
    void setLastGUIUpdateTime(const gr::high_res_timer_type);

    unsigned int getPendingGUIUpdateEvents();
    void incrementPendingGUIUpdateEvents();
    void decrementPendingGUIUpdateEvents();
    void resetPendingGUIUpdateEvents();

    static const long MAX_FFT_SIZE;
    static const long MIN_FFT_SIZE;

    QWidget* qwidget();

    void setTimeDomainAxis(double min, double max);
    void setConstellationAxis(double xmin, double xmax, double ymin, double ymax);
    void setConstellationPenSize(int size);
    void setFrequencyAxis(double min, double max);

    void setUpdateTime(double t);

    void enableRFFreq(bool en);

    bool checkClicked();
    float getClickedFreq();

protected:
private:
    gr::thread::mutex d_mutex;
    const int64_t _dataPoints;
    std::string _title;
    double _centerFrequency;
    double _startFrequency;
    double _stopFrequency;
    float _powerValue = 1;
    bool _windowOpennedFlag = false;
    int _windowType = 5;
    int64_t _lastDataPointCount;
    int _fftSize;
    gr::high_res_timer_type _lastGUIUpdateTime = 0;
    unsigned int _pendingGUIUpdateEventsCount = 0;
    int _droppedEntriesCount = 0;
    double _updateTime;

    SpectrumDisplayForm* _spectrumDisplayForm = nullptr; // Deleted by QT.

    std::vector<float> _fftPoints;
    std::vector<double> _realTimeDomainPoints;
    std::vector<double> _imagTimeDomainPoints;
};

#endif /* SPECTRUM_GUI_CLASS_HPP */

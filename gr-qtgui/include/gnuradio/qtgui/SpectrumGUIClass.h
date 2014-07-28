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

#ifndef SPECTRUM_GUI_CLASS_HPP
#define SPECTRUM_GUI_CLASS_HPP

#include <gnuradio/thread/thread.h>
#include <qwidget.h>
#include <qapplication.h>
#include <qlabel.h>
#include <qslider.h>
#include <gnuradio/qtgui/spectrumUpdateEvents.h>

class SpectrumDisplayForm;
#include <gnuradio/qtgui/spectrumdisplayform.h>

#include <cmath>

#include <complex>
#include <vector>
#include <string>

/*!
 * \brief QWidget class for controlling plotting.
 * \ingroup qtgui_blk
 */
class SpectrumGUIClass
{
public:
  SpectrumGUIClass(const uint64_t maxDataSize, const uint64_t fftSize,
		   const double newCenterFrequency,
		   const double newStartFrequency,
		   const double newStopFrequency);
  ~SpectrumGUIClass();
  void reset();

  void openSpectrumWindow(QWidget*,
			  const bool frequency=true, const bool waterfall=true,
			  const bool time=true, const bool constellation=true);
  void setDisplayTitle(const std::string);

  bool getWindowOpenFlag();
  void setWindowOpenFlag(const bool);

  void setFrequencyRange(const double, const double, const double);
  double getStartFrequency();
  double getStopFrequency();
  double getCenterFrequency();

  void updateWindow(const bool, const float*,
		    const uint64_t, const float*,
		    const uint64_t, const float*,
		    const uint64_t,
		    const gr::high_res_timer_type, const bool);

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
  void setConstellationAxis(double xmin, double xmax,
			    double ymin, double ymax);
  void setConstellationPenSize(int size);
  void setFrequencyAxis(double min, double max);

  void setUpdateTime(double t);

  void enableRFFreq(bool en);

  bool checkClicked();
  float getClickedFreq();

protected:

private:

  gr::thread::mutex d_mutex;
  int64_t _dataPoints;
  std::string _title;
  double _centerFrequency;
  double _startFrequency;
  double _stopFrequency;
  float _powerValue;
  bool _windowOpennedFlag;
  int _windowType;
  int64_t _lastDataPointCount;
  int _fftSize;
  gr::high_res_timer_type _lastGUIUpdateTime;
  unsigned int _pendingGUIUpdateEventsCount;
  int _droppedEntriesCount;
  bool _fftBuffersCreatedFlag;
  double _updateTime;

  SpectrumDisplayForm* _spectrumDisplayForm;

  float* _fftPoints;
  double* _realTimeDomainPoints;
  double* _imagTimeDomainPoints;
};

#endif /* SPECTRUM_GUI_CLASS_HPP */

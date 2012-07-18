/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#ifndef FREQ_DISPLAY_FORM_H
#define FREQ_DISPLAY_FORM_H

#include <spectrumUpdateEvents.h>
#include <FrequencyDisplayPlot.h>
#include <QtGui/QtGui>
#include <vector>
#include <filter/firdes.h>

#include "displayform.h"

class FreqDisplayForm : public DisplayForm
{
  Q_OBJECT

  public:
  FreqDisplayForm(int nplots=1, QWidget* parent = 0);
  ~FreqDisplayForm();

  FrequencyDisplayPlot* getPlot();

  int GetFFTSize() const;
  float GetFFTAverage() const;
  gr::filter::firdes::win_type GetFFTWindowType() const;

public slots:
  void customEvent(QEvent *e);

  void SetFFTSize(const int);
  void SetFFTAverage(const float);
  void SetFFTWindowType(const gr::filter::firdes::win_type);

  void SetFrequencyRange(const double newCenterFrequency,
			 const double newStartFrequency,
			 const double newStopFrequency);
  void SetFrequencyAxis(double min, double max);

private slots:
  void newData(const QEvent *updateEvent);

private:
  uint64_t _numRealDataPoints;
  QIntValidator* _intValidator;

  double _startFrequency;
  double _stopFrequency;

  int _fftsize;
  float _fftavg;
  gr::filter::firdes::win_type _fftwintype;
};

#endif /* FREQ_DISPLAY_FORM_H */

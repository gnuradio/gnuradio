/* -*- c++ -*- */
/*
 * Copyright 2011,2012 Free Software Foundation, Inc.
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

#ifndef TIME_DISPLAY_FORM_H
#define TIME_DISPLAY_FORM_H

#include <spectrumUpdateEvents.h>
#include <TimeDomainDisplayPlot.h>
#include <QtGui/QtGui>
#include <vector>

#include "displayform.h"

class TimeDisplayForm : public DisplayForm
{
  Q_OBJECT

  public:
  TimeDisplayForm(int nplots=1, QWidget* parent = 0);
  ~TimeDisplayForm();

  TimeDomainDisplayPlot* getPlot();

  int GetNPoints() const;

public slots:
  void customEvent(QEvent * e);

  void setFrequencyRange(const double newCenterFrequency,
			 const double newStartFrequency,
			 const double newStopFrequency);
  void setTimeDomainAxis(double min, double max);
  void SetNPoints(const int);

private slots:
  void newData(const QEvent*);

private:
  QIntValidator* _intValidator;

  double _startFrequency;
  double _stopFrequency;

  int d_npoints;
};

#endif /* TIME_DISPLAY_FORM_H */

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

#include <cmath>
#include <QMessageBox>
#include <freqdisplayform.h>
#include <iostream>

FreqDisplayForm::FreqDisplayForm(int nplots, QWidget* parent)
  : DisplayForm(nplots, parent)
{
  _intValidator = new QIntValidator(this);
  _intValidator->setBottom(0);

  _layout = new QGridLayout(this);
  _displayPlot = new FrequencyDisplayPlot(nplots, this);
  _layout->addWidget(_displayPlot, 0, 0);
  setLayout(_layout);

  _numRealDataPoints = 1024;
  _fftsize = 1024;
  _fftavg = 1.0;
  
  FFTSizeMenu *sizemenu = new FFTSizeMenu(this);
  FFTAverageMenu *avgmenu = new FFTAverageMenu(this);
  FFTWindowMenu *winmenu = new FFTWindowMenu(this);
  _menu->addMenu(sizemenu);
  _menu->addMenu(avgmenu);
  _menu->addMenu(winmenu);
  connect(sizemenu, SIGNAL(whichTrigger(int)),
	  this, SLOT(SetFFTSize(const int)));
  connect(avgmenu, SIGNAL(whichTrigger(float)),
	  this, SLOT(SetFFTAverage(const float)));
  connect(winmenu, SIGNAL(whichTrigger(gr::filter::firdes::win_type)),
	  this, SLOT(SetFFTWindowType(const gr::filter::firdes::win_type)));

  Reset();

  connect(_displayPlot, SIGNAL(plotPointSelected(const QPointF)),
	  this, SLOT(onPlotPointSelected(const QPointF)));
}

FreqDisplayForm::~FreqDisplayForm()
{
  // Qt deletes children when parent is deleted

  // Don't worry about deleting Display Plots - they are deleted when parents are deleted
  delete _intValidator;
}

FrequencyDisplayPlot*
FreqDisplayForm::getPlot()
{
  return ((FrequencyDisplayPlot*)_displayPlot);
}

void
FreqDisplayForm::newData(const QEvent *updateEvent)
{
  FreqUpdateEvent *fevent = (FreqUpdateEvent*)updateEvent;
  const std::vector<double*> dataPoints = fevent->getPoints();
  const uint64_t numDataPoints = fevent->getNumDataPoints();

  getPlot()->PlotNewData(dataPoints, numDataPoints,
			 0, 0, 0, d_update_time);
}

void
FreqDisplayForm::customEvent( QEvent * e)
{
  if(e->type() == FreqUpdateEvent::Type()) {
    newData(e);
  }
}

int
FreqDisplayForm::GetFFTSize() const
{
  return _fftsize;
}

float
FreqDisplayForm::GetFFTAverage() const
{
  return _fftavg;
}

gr::filter::firdes::win_type
FreqDisplayForm::GetFFTWindowType() const
{
  return _fftwintype;
}

void
FreqDisplayForm::SetFFTSize(const int newsize)
{
  _fftsize = newsize;
}

void
FreqDisplayForm::SetFFTAverage(const float newavg)
{
  _fftavg = newavg;
  getPlot()->replot();
}

void
FreqDisplayForm::SetFFTWindowType(const gr::filter::firdes::win_type newwin)
{
  _fftwintype = newwin;
}

void
FreqDisplayForm::SetFrequencyRange(const double newCenterFrequency,
				   const double newStartFrequency,
				   const double newStopFrequency)
{
  double fdiff = std::max(fabs(newStartFrequency), fabs(newStopFrequency));

  if(fdiff > 0) {
    std::string strunits[4] = {"Hz", "kHz", "MHz", "GHz"};
    double units10 = floor(log10(fdiff));
    double units3  = std::max(floor(units10 / 3.0), 0.0);
    double units = pow(10, (units10-fmod(units10, 3.0)));
    int iunit = static_cast<int>(units3);

    _startFrequency = newStartFrequency;
    _stopFrequency = newStopFrequency;
    double centerFrequency = newCenterFrequency;

    getPlot()->SetFrequencyRange(_startFrequency,
				 _stopFrequency,
				 centerFrequency,
				 true,
				 units, strunits[iunit]);
  }
}

void
FreqDisplayForm::SetFrequencyAxis(double min, double max)
{
  getPlot()->set_yaxis(min, max);
}

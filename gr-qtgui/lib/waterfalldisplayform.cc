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
#include <QColorDialog>
#include <QMessageBox>
#include <waterfalldisplayform.h>
#include <iostream>

WaterfallDisplayForm::WaterfallDisplayForm(int nplots, QWidget* parent)
  : DisplayForm(nplots, parent)
{
  _intValidator = new QIntValidator(this);
  _intValidator->setBottom(0);

  _layout = new QGridLayout(this);
  _displayPlot = new WaterfallDisplayPlot(nplots, this);
  _layout->addWidget(_displayPlot, 0, 0);
  setLayout(_layout);

  _numRealDataPoints = 1024;
  _fftsize = 1024;
  _fftavg = 1.0;

  FFTSizeMenu *sizemenu = new FFTSizeMenu(this);
  FFTAverageMenu *avgmenu = new FFTAverageMenu(this);
  _menu->addMenu(sizemenu);
  _menu->addMenu(avgmenu);
  connect(sizemenu, SIGNAL(whichTrigger(int)),
	  this, SLOT(SetFFTSize(const int)));
  connect(avgmenu, SIGNAL(whichTrigger(float)),
	  this, SLOT(SetFFTAverage(const float)));

  Reset();

  connect(_displayPlot, SIGNAL(plotPointSelected(const QPointF)),
	  this, SLOT(onPlotPointSelected(const QPointF)));
}

WaterfallDisplayForm::~WaterfallDisplayForm()
{
  // Qt deletes children when parent is deleted

  // Don't worry about deleting Display Plots - they are deleted when parents are deleted
  delete _intValidator;
}

WaterfallDisplayPlot*
WaterfallDisplayForm::getPlot()
{
  return ((WaterfallDisplayPlot*)_displayPlot);
}

void
WaterfallDisplayForm::newData(const QEvent *updateEvent)
{
  WaterfallUpdateEvent *event = (WaterfallUpdateEvent*)updateEvent;
  const std::vector<double*> dataPoints = event->getPoints();
  const uint64_t numDataPoints = event->getNumDataPoints();
  const gruel::high_res_timer_type dataTimestamp = event->getDataTimestamp();
  getPlot()->PlotNewData(dataPoints, numDataPoints,
			 d_update_time, dataTimestamp, 0);
}

void
WaterfallDisplayForm::customEvent( QEvent * e)
{
  if(e->type() == WaterfallUpdateEvent::Type()) {
    newData(e);
  }
}

int
WaterfallDisplayForm::GetFFTSize() const
{
  return _fftsize;
}

float
WaterfallDisplayForm::GetFFTAverage() const
{
  return _fftavg;
}

gr::filter::firdes::win_type
WaterfallDisplayForm::GetFFTWindowType() const
{
  return _fftwintype;
}

void
WaterfallDisplayForm::SetFFTSize(const int newsize)
{
  _fftsize = newsize;
}

void
WaterfallDisplayForm::SetFFTAverage(const float newavg)
{
  _fftavg = newavg;
}

void
WaterfallDisplayForm::SetFFTWindowType(const gr::filter::firdes::win_type newwin)
{
  _fftwintype = newwin;
}

void
WaterfallDisplayForm::SetFrequencyRange(const double newCenterFrequency,
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
WaterfallDisplayForm::SetIntensityRange(const double minIntensity,
					const double maxIntensity)
{
  getPlot()->SetIntensityRange(minIntensity, maxIntensity);
}

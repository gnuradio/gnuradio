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

#include <cmath>
#include <QMessageBox>
#include <timedisplayform.h>
#include <iostream>

TimeDisplayForm::TimeDisplayForm(int nplots, QWidget* parent)
  : DisplayForm(nplots, parent)
{
  _intValidator = new QIntValidator(this);
  _intValidator->setBottom(0);

  _layout = new QGridLayout(this);
  _displayPlot = new TimeDomainDisplayPlot(nplots, this);
  _layout->addWidget(_displayPlot, 0, 0);
  setLayout(_layout);

  NPointsMenu *nptsmenu = new NPointsMenu(this);
  _menu->addAction(nptsmenu);
  connect(nptsmenu, SIGNAL(whichTrigger(int)),
	  this, SLOT(SetNPoints(const int)));

  Reset();

  connect(_displayPlot, SIGNAL(plotPointSelected(const QPointF)),
	  this, SLOT(onPlotPointSelected(const QPointF)));
}

TimeDisplayForm::~TimeDisplayForm()
{
  // Qt deletes children when parent is deleted

  // Don't worry about deleting Display Plots - they are deleted when parents are deleted
  delete _intValidator;
}

TimeDomainDisplayPlot*
TimeDisplayForm::getPlot()
{
  return ((TimeDomainDisplayPlot*)_displayPlot);
}

void
TimeDisplayForm::newData(const QEvent* updateEvent)
{
  TimeUpdateEvent *tevent = (TimeUpdateEvent*)updateEvent;
  const std::vector<double*> dataPoints = tevent->getTimeDomainPoints();
  const uint64_t numDataPoints = tevent->getNumTimeDomainDataPoints();

  getPlot()->PlotNewData(dataPoints,
			 numDataPoints,
			 d_update_time);
}

void
TimeDisplayForm::customEvent(QEvent * e)
{
  if(e->type() == TimeUpdateEvent::Type()) {
    newData(e);
  }
}

void
TimeDisplayForm::setFrequencyRange(const double newCenterFrequency,
				   const double newStartFrequency,
				   const double newStopFrequency)
{
  double fdiff = std::max(fabs(newStartFrequency), fabs(newStopFrequency));

  if(fdiff > 0) {
    std::string strtime[4] = {"sec", "ms", "us", "ns"};
    double units10 = floor(log10(fdiff));
    double units3  = std::max(floor(units10 / 3.0), 0.0);
    double units = pow(10, (units10-fmod(units10, 3.0)));
    int iunit = static_cast<int>(units3);

    _startFrequency = newStartFrequency;
    _stopFrequency = newStopFrequency;
    
    getPlot()->SetSampleRate(_stopFrequency - _startFrequency,
			     units, strtime[iunit]);
  }
}

void
TimeDisplayForm::setTimeDomainAxis(double min, double max)
{
  getPlot()->setYaxis(min, max);
}

int
TimeDisplayForm::GetNPoints() const
{
  return d_npoints;
}

void
TimeDisplayForm::SetNPoints(const int npoints)
{
  d_npoints = npoints;
}

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
#include <constellationdisplayform.h>
#include <iostream>

ConstellationDisplayForm::ConstellationDisplayForm(int nplots, QWidget* parent)
  : DisplayForm(nplots, parent)
{
  _intValidator = new QIntValidator(this);
  _intValidator->setBottom(0);

  _layout = new QGridLayout(this);
  _displayPlot = new ConstellationDisplayPlot(nplots, this);
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

ConstellationDisplayForm::~ConstellationDisplayForm()
{
  // Qt deletes children when parent is deleted

  // Don't worry about deleting Display Plots - they are deleted when parents are deleted
  delete _intValidator;
}

ConstellationDisplayPlot*
ConstellationDisplayForm::getPlot()
{
  return ((ConstellationDisplayPlot*)_displayPlot);
}

void
ConstellationDisplayForm::newData(const QEvent* updateEvent)
{
  ConstUpdateEvent *tevent = (ConstUpdateEvent*)updateEvent;
  const std::vector<double*> realDataPoints = tevent->getRealPoints();
  const std::vector<double*> imagDataPoints = tevent->getImagPoints();
  const uint64_t numDataPoints = tevent->getNumDataPoints();

  getPlot()->PlotNewData(realDataPoints,
			 imagDataPoints,
			 numDataPoints,
			 d_update_time);
}

void
ConstellationDisplayForm::customEvent(QEvent * e)
{
  if(e->type() == ConstUpdateEvent::Type()) {
    newData(e);
  }
}

int
ConstellationDisplayForm::GetNPoints() const
{
  return d_npoints;
}

void
ConstellationDisplayForm::SetNPoints(const int npoints)
{
  d_npoints = npoints;
}

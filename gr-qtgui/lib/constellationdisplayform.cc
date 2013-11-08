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
#include <gnuradio/qtgui/constellationdisplayform.h>
#include <iostream>

ConstellationDisplayForm::ConstellationDisplayForm(int nplots, QWidget* parent)
  : DisplayForm(nplots, parent)
{
  d_int_validator = new QIntValidator(this);
  d_int_validator->setBottom(0);

  d_layout = new QGridLayout(this);
  d_display_plot = new ConstellationDisplayPlot(nplots, this);
  d_layout->addWidget(d_display_plot, 0, 0);
  setLayout(d_layout);

  NPointsMenu *nptsmenu = new NPointsMenu(this);
  d_menu->addAction(nptsmenu);
  connect(nptsmenu, SIGNAL(whichTrigger(int)),
	  this, SLOT(setNPoints(const int)));

  Reset();

  connect(d_display_plot, SIGNAL(plotPointSelected(const QPointF)),
	  this, SLOT(onPlotPointSelected(const QPointF)));
}

ConstellationDisplayForm::~ConstellationDisplayForm()
{
  // Qt deletes children when parent is deleted

  // Don't worry about deleting Display Plots - they are deleted when parents are deleted
  delete d_int_validator;
}

ConstellationDisplayPlot*
ConstellationDisplayForm::getPlot()
{
  return ((ConstellationDisplayPlot*)d_display_plot);
}

void
ConstellationDisplayForm::newData(const QEvent* updateEvent)
{
  ConstUpdateEvent *tevent = (ConstUpdateEvent*)updateEvent;
  const std::vector<double*> realDataPoints = tevent->getRealPoints();
  const std::vector<double*> imagDataPoints = tevent->getImagPoints();
  const uint64_t numDataPoints = tevent->getNumDataPoints();

  getPlot()->plotNewData(realDataPoints,
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
ConstellationDisplayForm::getNPoints() const
{
  return d_npoints;
}

void
ConstellationDisplayForm::setNPoints(const int npoints)
{
  d_npoints = npoints;
}

void
ConstellationDisplayForm::setYaxis(double min, double max)
{
  getPlot()->set_yaxis(min, max);
}

void
ConstellationDisplayForm::setXaxis(double min, double max)
{
  getPlot()->set_xaxis(min, max);
}

void
ConstellationDisplayForm::autoScale(bool en)
{
  d_autoscale_state = en;
  d_autoscale_act->setChecked(en);
  getPlot()->setAutoScale(d_autoscale_state);
  getPlot()->replot();
}

void
ConstellationDisplayForm::setSampleRate(const QString &samprate)
{
}

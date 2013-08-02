/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
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
#include <gnuradio/qtgui/histogramdisplayform.h>
#include <iostream>

HistogramDisplayForm::HistogramDisplayForm(int nplots, QWidget* parent)
  : DisplayForm(nplots, parent)
{
  d_semilogx = false;
  d_semilogy = false;

  _intValidator = new QIntValidator(this);
  _intValidator->setBottom(0);

  _layout = new QGridLayout(this);
  _displayPlot = new HistogramDisplayPlot(nplots, this);
  _layout->addWidget(_displayPlot, 0, 0);
  setLayout(_layout);

  NPointsMenu *nptsmenu = new NPointsMenu(this);
  _menu->addAction(nptsmenu);
  connect(nptsmenu, SIGNAL(whichTrigger(int)),
	  this, SLOT(setNPoints(const int)));

  NPointsMenu *nbinsmenu = new NPointsMenu(this);
  nbinsmenu->setText("Number of Bins");
  _menu->addAction(nbinsmenu);
  connect(nbinsmenu, SIGNAL(whichTrigger(int)),
	  this, SLOT(setNumBins(const int)));

  QAction *accummenu = new QAction("Accumulate", this);
  accummenu->setCheckable(true);
  _menu->addAction(accummenu);
  connect(accummenu, SIGNAL(triggered(bool)),
	  this, SLOT(setAccumulate(bool)));

//  d_semilogxmenu = new QAction("Semilog X", this);
//  d_semilogxmenu->setCheckable(true);
//  _menu->addAction(d_semilogxmenu);
//  connect(d_semilogxmenu, SIGNAL(triggered(bool)),
//	  this, SLOT(setSemilogx(bool)));
//
//  d_semilogymenu = new QAction("Semilog Y", this);
//  d_semilogymenu->setCheckable(true);
//  _menu->addAction(d_semilogymenu);
//  connect(d_semilogymenu, SIGNAL(triggered(bool)),
//	  this, SLOT(setSemilogy(bool)));

  Reset();

  connect(_displayPlot, SIGNAL(plotPointSelected(const QPointF)),
	  this, SLOT(onPlotPointSelected(const QPointF)));
}

HistogramDisplayForm::~HistogramDisplayForm()
{
  // Qt deletes children when parent is deleted

  // Don't worry about deleting Display Plots - they are deleted when parents are deleted
  delete _intValidator;
}

HistogramDisplayPlot*
HistogramDisplayForm::getPlot()
{
  return ((HistogramDisplayPlot*)_displayPlot);
}

void
HistogramDisplayForm::newData(const QEvent* updateEvent)
{
  HistogramUpdateEvent *hevent = (HistogramUpdateEvent*)updateEvent;
  const std::vector<double*> dataPoints = hevent->getDataPoints();
  const uint64_t numDataPoints = hevent->getNumDataPoints();

  getPlot()->plotNewData(dataPoints,
			 numDataPoints,
			 d_update_time);
}

void
HistogramDisplayForm::customEvent(QEvent * e)
{
  if(e->type() == HistogramUpdateEvent::Type()) {
    newData(e);
  }
}

void
HistogramDisplayForm::setYaxis(double min, double max)
{
  getPlot()->setYaxis(min, max);
}

void
HistogramDisplayForm::setXaxis(double min, double max)
{
  getPlot()->setXaxis(min, max);
}

int
HistogramDisplayForm::getNPoints() const
{
  return d_npoints;
}

void
HistogramDisplayForm::setNPoints(const int npoints)
{
  d_npoints = npoints;
}

void
HistogramDisplayForm::autoScale(bool en)
{
  _autoscale_state = en;
  _autoscale_act->setChecked(en);
  getPlot()->setAutoScale(_autoscale_state);
  getPlot()->replot();
}

void
HistogramDisplayForm::setSemilogx(bool en)
{
  d_semilogx = en;
  d_semilogxmenu->setChecked(en);
  getPlot()->setSemilogx(d_semilogx);
  getPlot()->replot();
}

void
HistogramDisplayForm::setSemilogy(bool en)
{
  d_semilogy = en;
  d_semilogymenu->setChecked(en);
  getPlot()->setSemilogy(d_semilogy);
  getPlot()->replot();
}

void
HistogramDisplayForm::setNumBins(const int bins)
{
  getPlot()->setNumBins(bins);
  getPlot()->replot();
}

void
HistogramDisplayForm::setAccumulate(bool en)
{
  getPlot()->setAccumulate(en);
  getPlot()->replot();
}

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

  d_int_validator = new QIntValidator(this);
  d_int_validator->setBottom(0);

  d_layout = new QGridLayout(this);
  d_display_plot = new HistogramDisplayPlot(nplots, this);
  d_layout->addWidget(d_display_plot, 0, 0);
  setLayout(d_layout);

  d_nptsmenu = new NPointsMenu(this);
  d_menu->addAction(d_nptsmenu);
  connect(d_nptsmenu, SIGNAL(whichTrigger(int)),
	  this, SLOT(setNPoints(const int)));

  d_nbinsmenu = new NPointsMenu(this);
  d_nbinsmenu->setText("Number of Bins");
  d_menu->addAction(d_nbinsmenu);
  connect(d_nbinsmenu, SIGNAL(whichTrigger(int)),
	  this, SLOT(setNumBins(const int)));

  d_accum_act = new QAction("Accumulate", this);
  d_accum_act->setCheckable(true);
  d_menu->addAction(d_accum_act);
  connect(d_accum_act, SIGNAL(triggered(bool)),
	  this, SLOT(setAccumulate(bool)));

  d_menu->removeAction(d_autoscale_act);
  d_autoscale_act->setText(tr("Auto Scale Y"));
  d_autoscale_act->setStatusTip(tr("Autoscale Y-axis"));
  d_autoscale_act->setCheckable(true);
  d_autoscale_act->setChecked(true);
  d_autoscale_state = true;
  d_menu->addAction(d_autoscale_act);

  d_autoscalex_act = new QAction("Auto Scale X", this);
  d_autoscalex_act->setStatusTip(tr("Update X-axis scale"));
  d_autoscalex_act->setCheckable(false);
  connect(d_autoscalex_act, SIGNAL(changed()),
	  this, SLOT(autoScaleX()));
  d_autoscalex_state = false;
  d_menu->addAction(d_autoscalex_act);


//  d_semilogxmenu = new QAction("Semilog X", this);
//  d_semilogxmenu->setCheckable(true);
//  d_menu->addAction(d_semilogxmenu);
//  connect(d_semilogxmenu, SIGNAL(triggered(bool)),
//	  this, SLOT(setSemilogx(bool)));
//
//  d_semilogymenu = new QAction("Semilog Y", this);
//  d_semilogymenu->setCheckable(true);
//  d_menu->addAction(d_semilogymenu);
//  connect(d_semilogymenu, SIGNAL(triggered(bool)),
//	  this, SLOT(setSemilogy(bool)));

  Reset();

  connect(d_display_plot, SIGNAL(plotPointSelected(const QPointF)),
	  this, SLOT(onPlotPointSelected(const QPointF)));
}

HistogramDisplayForm::~HistogramDisplayForm()
{
  // Qt deletes children when parent is deleted

  // Don't worry about deleting Display Plots - they are deleted when parents are deleted
  delete d_int_validator;
}

HistogramDisplayPlot*
HistogramDisplayForm::getPlot()
{
  return ((HistogramDisplayPlot*)d_display_plot);
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
  else if(e->type() == HistogramSetAccumulator::Type()) {
    bool en = ((HistogramSetAccumulator*)e)->getAccumulator();
    setAccumulate(en);
  }
  else if(e->type() == HistogramClearEvent::Type()) {
    getPlot()->clear();
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
  d_nptsmenu->setDiagText(npoints);
}

void
HistogramDisplayForm::autoScale(bool en)
{
  d_autoscale_state = en;
  d_autoscale_act->setChecked(en);
  getPlot()->setAutoScale(d_autoscale_state);
  getPlot()->replot();
}

void
HistogramDisplayForm::autoScaleX()
{
  getPlot()->setAutoScaleX();
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
  d_nbinsmenu->setDiagText(bins);
}

void
HistogramDisplayForm::setAccumulate(bool en)
{
  // Turn on y-axis autoscaling when turning accumulate on.
  if(en) {
    autoScale(true);
  }
  d_accum_act->setChecked(en);
  getPlot()->setAccumulate(en);
  getPlot()->replot();
}

bool
HistogramDisplayForm::getAccumulate()
{
  return getPlot()->getAccumulate();
}

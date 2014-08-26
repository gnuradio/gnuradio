/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
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
#include <gnuradio/qtgui/vectordisplayform.h>
#include <iostream>

VectorDisplayForm::VectorDisplayForm(int nplots, QWidget* parent)
  : DisplayForm(nplots, parent)
{
  d_int_validator = new QIntValidator(this);
  d_int_validator->setBottom(0);

  d_layout = new QGridLayout(this);
  d_display_plot = new VectorDisplayPlot(nplots, this);
  d_layout->addWidget(d_display_plot, 0, 0);
  setLayout(d_layout);

  d_num_real_data_points = 1024;
  d_vecsize = 1024;
  d_vecavg = 1.0;
  d_ref_level = 0.0;
  d_clicked = false;
  d_clicked_x_level = 0;

  d_avgmenu = new AverageMenu("Average", this);
  d_menu->addMenu(d_avgmenu);
  connect(d_avgmenu, SIGNAL(whichTrigger(float)),
	  this, SLOT(setVecAverage(const float)));

  PopupMenu *maxymenu = new PopupMenu("Y Max", this);
  d_menu->addAction(maxymenu);
  connect(maxymenu, SIGNAL(whichTrigger(QString)),
	  this, SLOT(setYMax(QString)));

  PopupMenu *minymenu = new PopupMenu("Y Min", this);
  d_menu->addAction(minymenu);
  connect(minymenu, SIGNAL(whichTrigger(QString)),
	  this, SLOT(setYMin(QString)));

  d_clearmax_act = new QAction("Clear Max", this);
  d_menu->addAction(d_clearmax_act);
  connect(d_clearmax_act, SIGNAL(triggered()),
	  this, SLOT(clearMaxHold()));
  d_clearmin_act = new QAction("Clear Min", this);
  d_menu->addAction(d_clearmin_act);
  connect(d_clearmin_act, SIGNAL(triggered()),
	  this, SLOT(clearMinHold()));

  Reset();

  connect(d_display_plot, SIGNAL(plotPointSelected(const QPointF)),
	  this, SLOT(onPlotPointSelected(const QPointF)));
}

VectorDisplayForm::~VectorDisplayForm()
{
  // Qt deletes children when parent is deleted

  // Don't worry about deleting Display Plots - they are deleted when parents are deleted
  delete d_int_validator;
}

VectorDisplayPlot*
VectorDisplayForm::getPlot()
{
  return ((VectorDisplayPlot*)d_display_plot);
}

void
VectorDisplayForm::newData(const QEvent *updateEvent)
{
  FreqUpdateEvent *fevent = (FreqUpdateEvent*)updateEvent;
  const std::vector<double*> dataPoints = fevent->getPoints();
  const uint64_t numDataPoints = fevent->getNumDataPoints();

  getPlot()->plotNewData(
      dataPoints,
      numDataPoints,
      d_ref_level,
      d_update_time
  );
}

void
VectorDisplayForm::customEvent( QEvent * e)
{
  // We just re-use FreqUpdateEvent as long as that works
  if(e->type() == FreqUpdateEvent::Type()) {
    newData(e);
  }
}

int
VectorDisplayForm::getVecSize() const
{
  return d_vecsize;
}

float
VectorDisplayForm::getVecAverage() const
{
  return d_vecavg;
}

void VectorDisplayForm::setXAxisLabel(const QString &label)
{
  getPlot()->setXAxisLabel(label);
}

void VectorDisplayForm::setYAxisLabel(const QString &label)
{
  getPlot()->setYAxisLabel(label);
}

void VectorDisplayForm::setRefLevel(double refLevel)
{
  d_ref_level = refLevel;
}

void
VectorDisplayForm::setVecSize(const int newsize)
{
  d_vecsize = newsize;
  getPlot()->replot();
}

void
VectorDisplayForm::setVecAverage(const float newavg)
{
  d_vecavg = newavg;
  d_avgmenu->getActionFromAvg(newavg)->setChecked(true);
  getPlot()->replot();
}

void VectorDisplayForm::setXaxis(double start, double step)
{
  getPlot()->setXAxisValues(start, step);
}

void
VectorDisplayForm::setYaxis(double min, double max)
{
  getPlot()->setYaxis(min, max);
}

void
VectorDisplayForm::setYMax(const QString &m)
{
  double new_max = m.toDouble();
  double cur_ymin = getPlot()->getYMin();
  if(new_max > cur_ymin)
    setYaxis(cur_ymin, new_max);
}

void
VectorDisplayForm::setYMin(const QString &m)
{
  double new_min = m.toDouble();
  double cur_ymax = getPlot()->getYMax();
  if(new_min < cur_ymax)
    setYaxis(new_min, cur_ymax);
}

void
VectorDisplayForm::autoScale(bool en)
{
  if(en) {
    d_autoscale_state = true;
  }
  else {
    d_autoscale_state = false;
  }

  d_autoscale_act->setChecked(en);
  getPlot()->setAutoScale(d_autoscale_state);
  getPlot()->replot();
}

void
VectorDisplayForm::clearMaxHold()
{
  getPlot()->clearMaxData();
}

void
VectorDisplayForm::clearMinHold()
{
  getPlot()->clearMinData();
}

void
VectorDisplayForm::onPlotPointSelected(const QPointF p)
{
  d_clicked = true;
  d_clicked_x_level = p.x();
}

bool
VectorDisplayForm::checkClicked()
{
  if(d_clicked) {
    d_clicked = false;
    return true;
  }
  else {
    return false;
  }
}

float
VectorDisplayForm::getClickedXVal() const
{
  return d_clicked_x_level;
}

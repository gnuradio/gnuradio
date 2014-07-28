/* -*- c++ -*- */
/*
 * Copyright 2012,2014 Free Software Foundation, Inc.
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

  d_nptsmenu = new NPointsMenu(this);
  d_menu->addAction(d_nptsmenu);
  connect(d_nptsmenu, SIGNAL(whichTrigger(int)),
	  this, SLOT(setNPoints(const int)));

  // Set up the trigger menu
  d_triggermenu = new QMenu("Trigger", this);
  d_tr_mode_menu = new TriggerModeMenu(this);
  d_tr_slope_menu = new TriggerSlopeMenu(this);
  d_tr_level_act = new PopupMenu("Level", this);
  d_tr_channel_menu = new TriggerChannelMenu(nplots, this);
  d_tr_tag_key_act = new PopupMenu("Tag Key", this);
  d_triggermenu->addMenu(d_tr_mode_menu);
  d_triggermenu->addMenu(d_tr_slope_menu);
  d_triggermenu->addAction(d_tr_level_act);
  d_triggermenu->addMenu(d_tr_channel_menu);
  d_triggermenu->addAction(d_tr_tag_key_act);
  d_menu->addMenu(d_triggermenu);

  setTriggerMode(gr::qtgui::TRIG_MODE_FREE);
  connect(d_tr_mode_menu, SIGNAL(whichTrigger(gr::qtgui::trigger_mode)),
	  this, SLOT(setTriggerMode(gr::qtgui::trigger_mode)));
  // updates trigger state by calling set level or set tag key.
  connect(d_tr_mode_menu, SIGNAL(whichTrigger(gr::qtgui::trigger_mode)),
	  this, SLOT(updateTrigger(gr::qtgui::trigger_mode)));

  setTriggerSlope(gr::qtgui::TRIG_SLOPE_POS);
  connect(d_tr_slope_menu, SIGNAL(whichTrigger(gr::qtgui::trigger_slope)),
	  this, SLOT(setTriggerSlope(gr::qtgui::trigger_slope)));

  setTriggerLevel(0);
  connect(d_tr_level_act, SIGNAL(whichTrigger(QString)),
	  this, SLOT(setTriggerLevel(QString)));

  setTriggerChannel(0);
  connect(d_tr_channel_menu, SIGNAL(whichTrigger(int)),
	  this, SLOT(setTriggerChannel(int)));

  setTriggerTagKey(std::string(""));
  connect(d_tr_tag_key_act, SIGNAL(whichTrigger(QString)),
	  this, SLOT(setTriggerTagKey(QString)));

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

/********************************************************************
 * TRIGGER METHODS
 *******************************************************************/

void
ConstellationDisplayForm::setTriggerMode(gr::qtgui::trigger_mode mode)
{
  d_trig_mode = mode;
  d_tr_mode_menu->getAction(mode)->setChecked(true);
}

void
ConstellationDisplayForm::updateTrigger(gr::qtgui::trigger_mode mode)
{
  // If auto or normal mode, popup trigger level box to set
  if((d_trig_mode == gr::qtgui::TRIG_MODE_AUTO) || (d_trig_mode == gr::qtgui::TRIG_MODE_NORM))
    d_tr_level_act->activate(QAction::Trigger);

  // if tag mode, popup tag key box to set
  if(d_trig_mode == gr::qtgui::TRIG_MODE_TAG)
    d_tr_tag_key_act->activate(QAction::Trigger);
}

gr::qtgui::trigger_mode
ConstellationDisplayForm::getTriggerMode() const
{
  return d_trig_mode;
}

void
ConstellationDisplayForm::setTriggerSlope(gr::qtgui::trigger_slope slope)
{
  d_trig_slope = slope;
  d_tr_slope_menu->getAction(slope)->setChecked(true);
}

gr::qtgui::trigger_slope
ConstellationDisplayForm::getTriggerSlope() const
{
  return d_trig_slope;
}

void
ConstellationDisplayForm::setTriggerLevel(QString s)
{
  d_trig_level = s.toFloat();
}

void
ConstellationDisplayForm::setTriggerLevel(float level)
{
  d_trig_level = level;
  d_tr_level_act->setText(QString().setNum(d_trig_level));
}

float
ConstellationDisplayForm::getTriggerLevel() const
{
  return d_trig_level;
}

void
ConstellationDisplayForm::setTriggerChannel(int channel)
{
  d_trig_channel = channel;
  d_tr_channel_menu->getAction(d_trig_channel)->setChecked(true);
}

int
ConstellationDisplayForm::getTriggerChannel() const
{
  return d_trig_channel;
}

void
ConstellationDisplayForm::setTriggerTagKey(QString s)
{
  d_trig_tag_key = s.toStdString();
}

void
ConstellationDisplayForm::setTriggerTagKey(const std::string &key)
{
  d_trig_tag_key = key;
  d_tr_tag_key_act->setText(QString().fromStdString(d_trig_tag_key));
}

std::string
ConstellationDisplayForm::getTriggerTagKey() const
{
  return d_trig_tag_key;
}

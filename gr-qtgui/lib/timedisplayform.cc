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
#include <gnuradio/qtgui/timedisplayform.h>
#include <iostream>

TimeDisplayForm::TimeDisplayForm(int nplots, QWidget* parent)
  : DisplayForm(nplots, parent)
{
  d_stem = false;
  d_semilogx = false;
  d_semilogy = false;

  d_int_validator = new QIntValidator(this);
  d_int_validator->setBottom(0);

  d_layout = new QGridLayout(this);
  d_display_plot = new TimeDomainDisplayPlot(nplots, this);
  d_layout->addWidget(d_display_plot, 0, 0);
  setLayout(d_layout);

  d_nptsmenu = new NPointsMenu(this);
  d_menu->addAction(d_nptsmenu);
  connect(d_nptsmenu, SIGNAL(whichTrigger(int)),
	  this, SLOT(setNPoints(const int)));

  d_stemmenu = new QAction("Stem Plot", this);
  d_stemmenu->setCheckable(true);
  d_menu->addAction(d_stemmenu);
  connect(d_stemmenu, SIGNAL(triggered(bool)),
	  this, SLOT(setStem(bool)));

  d_semilogxmenu = new QAction("Semilog X", this);
  d_semilogxmenu->setCheckable(true);
  d_menu->addAction(d_semilogxmenu);
  connect(d_semilogxmenu, SIGNAL(triggered(bool)),
	  this, SLOT(setSemilogx(bool)));

  d_semilogymenu = new QAction("Semilog Y", this);
  d_semilogymenu->setCheckable(true);
  d_menu->addAction(d_semilogymenu);
  connect(d_semilogymenu, SIGNAL(triggered(bool)),
	  this, SLOT(setSemilogy(bool)));

  for(int i = 0; i < d_nplots; i++) {
    d_tagsmenu.push_back(new QAction("Show Tag Makers", this));
    d_tagsmenu[i]->setCheckable(true);
    d_tagsmenu[i]->setChecked(true);
    connect(d_tagsmenu[i], SIGNAL(triggered(bool)),
	    this, SLOT(tagMenuSlot(bool)));
    d_lines_menu[i]->addAction(d_tagsmenu[i]);
  }

  // Set up the trigger menu
  d_triggermenu = new QMenu("Trigger", this);
  d_tr_mode_menu = new TriggerModeMenu(this);
  d_tr_slope_menu = new TriggerSlopeMenu(this);
  d_tr_level_act = new PopupMenu("Level", this);
  d_tr_delay_act = new PopupMenu("Delay", this);
  d_tr_channel_menu = new TriggerChannelMenu(nplots, this);
  d_tr_tag_key_act = new PopupMenu("Tag Key", this);
  d_triggermenu->addMenu(d_tr_mode_menu);
  d_triggermenu->addMenu(d_tr_slope_menu);
  d_triggermenu->addAction(d_tr_level_act);
  d_triggermenu->addAction(d_tr_delay_act);
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

  setTriggerDelay(0);
  connect(d_tr_delay_act, SIGNAL(whichTrigger(QString)),
	  this, SLOT(setTriggerDelay(QString)));

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

TimeDisplayForm::~TimeDisplayForm()
{
  // Qt deletes children when parent is deleted

  // Don't worry about deleting Display Plots - they are deleted when parents are deleted
  delete d_int_validator;
}

TimeDomainDisplayPlot*
TimeDisplayForm::getPlot()
{
  return ((TimeDomainDisplayPlot*)d_display_plot);
}

void
TimeDisplayForm::newData(const QEvent* updateEvent)
{
  TimeUpdateEvent *tevent = (TimeUpdateEvent*)updateEvent;
  const std::vector<double*> dataPoints = tevent->getTimeDomainPoints();
  const uint64_t numDataPoints = tevent->getNumTimeDomainDataPoints();
  const std::vector< std::vector<gr::tag_t> > tags = tevent->getTags();

  getPlot()->plotNewData(dataPoints,
			 numDataPoints,
			 d_update_time,
                         tags);
}

void
TimeDisplayForm::customEvent(QEvent * e)
{
  if(e->type() == TimeUpdateEvent::Type()) {
    newData(e);
  }
}

void
TimeDisplayForm::setSampleRate(const QString &samprate)
{
  setSampleRate(samprate.toDouble());
}

void
TimeDisplayForm::setSampleRate(const double samprate)
{
  if(samprate > 0) {
    std::string strtime[4] = {"sec", "ms", "us", "ns"};
    double units10 = floor(log10(samprate));
    double units3  = std::max(floor(units10 / 3.0), 0.0);
    double units = pow(10, (units10-fmod(units10, 3.0)));
    int iunit = static_cast<int>(units3);

    getPlot()->setSampleRate(samprate, units, strtime[iunit]);
  }
  else {
    throw std::runtime_error("TimeDisplayForm: samprate must be > 0.\n");
  }
}

void
TimeDisplayForm::setYaxis(double min, double max)
{
  getPlot()->setYaxis(min, max);
}

int
TimeDisplayForm::getNPoints() const
{
  return d_npoints;
}

void
TimeDisplayForm::setNPoints(const int npoints)
{
  d_npoints = npoints;
  d_nptsmenu->setDiagText(d_npoints);
}

void
TimeDisplayForm::setStem(bool en)
{
  d_stem = en;
  d_stemmenu->setChecked(en);
  getPlot()->stemPlot(d_stem);
  getPlot()->replot();
}

void
TimeDisplayForm::autoScale(bool en)
{
  d_autoscale_state = en;
  d_autoscale_act->setChecked(en);
  getPlot()->setAutoScale(d_autoscale_state);
  getPlot()->replot();
}

void
TimeDisplayForm::setSemilogx(bool en)
{
  d_semilogx = en;
  d_semilogxmenu->setChecked(en);
  getPlot()->setSemilogx(d_semilogx);
  getPlot()->replot();
}

void
TimeDisplayForm::setSemilogy(bool en)
{
  d_semilogy = en;
  d_semilogymenu->setChecked(en);
  getPlot()->setSemilogy(d_semilogy);
  getPlot()->replot();
}

void
TimeDisplayForm::setTagMenu(int which, bool en)
{
  getPlot()->enableTagMarker(which, en);
  d_tagsmenu[which]->setChecked(en);
}

void
TimeDisplayForm::tagMenuSlot(bool en)
{
  for(size_t i = 0; i < d_tagsmenu.size(); i++) {
    getPlot()->enableTagMarker(i, d_tagsmenu[i]->isChecked());
  }
}


/********************************************************************
 * TRIGGER METHODS
 *******************************************************************/

void
TimeDisplayForm::setTriggerMode(gr::qtgui::trigger_mode mode)
{
  d_trig_mode = mode;
  d_tr_mode_menu->getAction(mode)->setChecked(true);
}

void
TimeDisplayForm::updateTrigger(gr::qtgui::trigger_mode mode)
{
  // If auto or normal mode, popup trigger level box to set
  if((d_trig_mode == gr::qtgui::TRIG_MODE_AUTO) || (d_trig_mode == gr::qtgui::TRIG_MODE_NORM))
    d_tr_level_act->activate(QAction::Trigger);

  // if tag mode, popup tag key box to set
  if(d_trig_mode == gr::qtgui::TRIG_MODE_TAG)
    d_tr_tag_key_act->activate(QAction::Trigger);
}

gr::qtgui::trigger_mode
TimeDisplayForm::getTriggerMode() const
{
  return d_trig_mode;
}

void
TimeDisplayForm::setTriggerSlope(gr::qtgui::trigger_slope slope)
{
  d_trig_slope = slope;
  d_tr_slope_menu->getAction(slope)->setChecked(true);
}

gr::qtgui::trigger_slope
TimeDisplayForm::getTriggerSlope() const
{
  return d_trig_slope;
}

void
TimeDisplayForm::setTriggerLevel(QString s)
{
  d_trig_level = s.toFloat();
}

void
TimeDisplayForm::setTriggerLevel(float level)
{
  d_trig_level = level;
  d_tr_level_act->setText(QString().setNum(d_trig_level));
}

float
TimeDisplayForm::getTriggerLevel() const
{
  return d_trig_level;
}

void
TimeDisplayForm::setTriggerDelay(QString s)
{
  d_trig_delay = s.toFloat();
}

void
TimeDisplayForm::setTriggerDelay(float delay)
{
  d_trig_delay = delay;
  d_tr_delay_act->setText(QString().setNum(d_trig_delay));
}

float
TimeDisplayForm::getTriggerDelay() const
{
  return d_trig_delay;
}

void
TimeDisplayForm::setTriggerChannel(int channel)
{
  d_trig_channel = channel;
  d_tr_channel_menu->getAction(d_trig_channel)->setChecked(true);
}

int
TimeDisplayForm::getTriggerChannel() const
{
  return d_trig_channel;
}

void
TimeDisplayForm::setTriggerTagKey(QString s)
{
  d_trig_tag_key = s.toStdString();
}

void
TimeDisplayForm::setTriggerTagKey(const std::string &key)
{
  d_trig_tag_key = key;
  d_tr_tag_key_act->setText(QString().fromStdString(d_trig_tag_key));
}

std::string
TimeDisplayForm::getTriggerTagKey() const
{
  return d_trig_tag_key;
}


/* -*- c++ -*- */
/*
 * Copyright 2011,2012,2015 Free Software Foundation, Inc.
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
#include <QSpacerItem>
#include <QGroupBox>
#include <gnuradio/qtgui/timedisplayform.h>
#include <gnuradio/qtgui/timecontrolpanel.h>
#include <iostream>


TimeDisplayForm::TimeDisplayForm(int nplots, QWidget* parent)
  : DisplayForm(nplots, parent)
{
  d_stem = false;
  d_semilogx = false;
  d_semilogy = false;
  d_current_units = 1;

  d_trig_mode = gr::qtgui::TRIG_MODE_FREE;
  d_trig_slope = gr::qtgui::TRIG_SLOPE_POS;
  d_trig_level = 0;
  d_trig_delay = 0;
  d_trig_channel = 0;
  d_trig_tag_key = "";

  d_int_validator = new QIntValidator(this);
  d_int_validator->setBottom(0);

  d_layout = new QGridLayout(this);
  d_display_plot = new TimeDomainDisplayPlot(nplots, this);

  d_controlpanel = NULL;

  // Setup the layout of the display
  d_layout->addWidget(d_display_plot, 0, 0);

  d_layout->setColumnStretch(0, 1);
  setLayout(d_layout);

  d_nptsmenu = new NPointsMenu(this);
  d_menu->addAction(d_nptsmenu);
  connect(d_nptsmenu, SIGNAL(whichTrigger(int)),
	  this, SLOT(setNPoints(const int)));
  connect(this, SIGNAL(signalNPoints(const int)),
	  d_nptsmenu, SLOT(setDiagText(const int)));

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

  d_controlpanelmenu = new QAction("Control Panel", this);
  d_controlpanelmenu->setCheckable(true);
  d_menu->addAction(d_controlpanelmenu);
  connect(d_controlpanelmenu, SIGNAL(triggered(bool)),
	  this, SLOT(setupControlPanel(bool)));

  setTriggerMode(gr::qtgui::TRIG_MODE_FREE);
  setTriggerSlope(gr::qtgui::TRIG_SLOPE_POS);

  connect(d_tr_mode_menu, SIGNAL(whichTrigger(gr::qtgui::trigger_mode)),
	  this, SLOT(setTriggerMode(gr::qtgui::trigger_mode)));
  // updates trigger state by calling set level or set tag key.
  connect(d_tr_mode_menu, SIGNAL(whichTrigger(gr::qtgui::trigger_mode)),
	  this, SLOT(updateTrigger(gr::qtgui::trigger_mode)));

  connect(d_tr_slope_menu, SIGNAL(whichTrigger(gr::qtgui::trigger_slope)),
	  this, SLOT(setTriggerSlope(gr::qtgui::trigger_slope)));

  setTriggerLevel(0);
  connect(d_tr_level_act, SIGNAL(whichTrigger(QString)),
	  this, SLOT(setTriggerLevel(QString)));
  connect(this, SIGNAL(signalTriggerLevel(float)),
	  this, SLOT(setTriggerLevel(float)));

  setTriggerDelay(0);
  connect(d_tr_delay_act, SIGNAL(whichTrigger(QString)),
	  this, SLOT(setTriggerDelay(QString)));
  connect(this, SIGNAL(signalTriggerDelay(float)),
	  this, SLOT(setTriggerDelay(float)));

  setTriggerChannel(0);
  connect(d_tr_channel_menu, SIGNAL(whichTrigger(int)),
	  this, SLOT(setTriggerChannel(int)));

  setTriggerTagKey(std::string(""));
  connect(d_tr_tag_key_act, SIGNAL(whichTrigger(QString)),
	  this, SLOT(setTriggerTagKey(QString)));

  Reset();

  connect(d_display_plot, SIGNAL(plotPointSelected(const QPointF)),
	  this, SLOT(onPlotPointSelected(const QPointF)));

  connect(this, SIGNAL(signalReplot()),
          getPlot(), SLOT(replot()));
}

TimeDisplayForm::~TimeDisplayForm()
{
  // Qt deletes children when parent is deleted

  // Don't worry about deleting Display Plots - they are deleted when parents are deleted
  delete d_int_validator;

  teardownControlPanel();
}

void
TimeDisplayForm::setupControlPanel(bool en)
{
  if(en) {
    setupControlPanel();
  }
  else {
    teardownControlPanel();
  }
}

void
TimeDisplayForm::setupControlPanel()
{
  if(d_controlpanel)
    delete d_controlpanel;

  // Create the control panel layout
  d_controlpanel = new TimeControlPanel(this);

  // Connect action items in menu to controlpanel widgets
  connect(d_autoscale_act, SIGNAL(triggered(bool)),
          d_controlpanel, SLOT(toggleAutoScale(bool)));
  connect(d_grid_act, SIGNAL(triggered(bool)),
          d_controlpanel, SLOT(toggleGrid(bool)));
  connect(d_tr_mode_menu, SIGNAL(whichTrigger(gr::qtgui::trigger_mode)),
	  d_controlpanel, SLOT(toggleTriggerMode(gr::qtgui::trigger_mode)));
  connect(this, SIGNAL(signalTriggerMode(gr::qtgui::trigger_mode)),
	  d_controlpanel, SLOT(toggleTriggerMode(gr::qtgui::trigger_mode)));
  connect(d_tr_slope_menu, SIGNAL(whichTrigger(gr::qtgui::trigger_slope)),
	  d_controlpanel, SLOT(toggleTriggerSlope(gr::qtgui::trigger_slope)));
  connect(this, SIGNAL(signalTriggerSlope(gr::qtgui::trigger_slope)),
	  d_controlpanel, SLOT(toggleTriggerSlope(gr::qtgui::trigger_slope)));
  connect(d_stop_act, SIGNAL(triggered()),
          d_controlpanel, SLOT(toggleStopButton()));
  d_layout->addLayout(d_controlpanel, 0, 1);

  d_controlpanel->toggleAutoScale(d_autoscale_act->isChecked());
  d_controlpanel->toggleGrid(d_grid_act->isChecked());
  d_controlpanel->toggleTriggerMode(getTriggerMode());
  d_controlpanel->toggleTriggerSlope(getTriggerSlope());

  d_controlpanelmenu->setChecked(true);
}

void
TimeDisplayForm::teardownControlPanel()
{
  if(d_controlpanel) {
    d_layout->removeItem(d_controlpanel);
    delete d_controlpanel;
    d_controlpanel = NULL;
  }
  d_controlpanelmenu->setChecked(false);
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

    d_current_units = units;

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

void
TimeDisplayForm::setYLabel(const std::string &label,
                           const std::string &unit)
{
  getPlot()->setYLabel(label, unit);
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
  emit signalNPoints(npoints);
}

void
TimeDisplayForm::setStem(bool en)
{
  d_stem = en;
  d_stemmenu->setChecked(en);
  getPlot()->stemPlot(d_stem);
  emit signalReplot();
}

void
TimeDisplayForm::autoScale(bool en)
{
  d_autoscale_state = en;
  d_autoscale_act->setChecked(en);
  getPlot()->setAutoScale(d_autoscale_state);
  emit signalReplot();
}

void
TimeDisplayForm::autoScaleShot()
{
  getPlot()->setAutoScaleShot();
  emit signalReplot();
}

void
TimeDisplayForm::setSemilogx(bool en)
{
  d_semilogx = en;
  d_semilogxmenu->setChecked(en);
  getPlot()->setSemilogx(d_semilogx);

}

void
TimeDisplayForm::setSemilogy(bool en)
{
  d_semilogy = en;
  d_semilogymenu->setChecked(en);
  getPlot()->setSemilogy(d_semilogy);
  emit signalReplot();
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

  if((d_trig_mode == gr::qtgui::TRIG_MODE_AUTO) || (d_trig_mode == gr::qtgui::TRIG_MODE_NORM)) {
    getPlot()->attachTriggerLines(true);
  }
  else {
    getPlot()->attachTriggerLines(false);
  }

  emit signalReplot();
  emit signalTriggerMode(mode);
}

void
TimeDisplayForm::updateTrigger(gr::qtgui::trigger_mode mode)
{
  // If auto or normal mode, popup trigger level box to set
  if((d_trig_mode == gr::qtgui::TRIG_MODE_AUTO) || (d_trig_mode == gr::qtgui::TRIG_MODE_NORM)) {
    d_tr_level_act->activate(QAction::Trigger);
    getPlot()->attachTriggerLines(true);
  }
  else {
    getPlot()->attachTriggerLines(false);
  }

  // if tag mode, popup tag key box to set
  if((d_trig_tag_key == "") && (d_trig_mode == gr::qtgui::TRIG_MODE_TAG))
    d_tr_tag_key_act->activate(QAction::Trigger);

  emit signalReplot();
  emit signalTriggerMode(mode);
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

  emit signalReplot();
  emit signalTriggerSlope(slope);
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

  if((d_trig_mode == gr::qtgui::TRIG_MODE_AUTO) || (d_trig_mode == gr::qtgui::TRIG_MODE_NORM)) {
    getPlot()->setTriggerLines(d_trig_delay*d_current_units, d_trig_level);
  }

  emit signalReplot();
}

void
TimeDisplayForm::setTriggerLevel(float level)
{
  d_trig_level = level;
  d_tr_level_act->setText(QString().setNum(d_trig_level));

  if((d_trig_mode == gr::qtgui::TRIG_MODE_AUTO) || (d_trig_mode == gr::qtgui::TRIG_MODE_NORM)) {
    getPlot()->setTriggerLines(d_trig_delay*d_current_units, d_trig_level);
  }

  emit signalReplot();
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

  if((d_trig_mode == gr::qtgui::TRIG_MODE_AUTO) || (d_trig_mode == gr::qtgui::TRIG_MODE_NORM)) {
    getPlot()->setTriggerLines(d_trig_delay*d_current_units, d_trig_level);
  }

  emit signalReplot();
}

void
TimeDisplayForm::setTriggerDelay(float delay)
{
  d_trig_delay = delay;
  d_tr_delay_act->setText(QString().setNum(d_trig_delay));

  if((d_trig_mode == gr::qtgui::TRIG_MODE_AUTO) || (d_trig_mode == gr::qtgui::TRIG_MODE_NORM)) {
    getPlot()->setTriggerLines(d_trig_delay*d_current_units, d_trig_level);
  }

  emit signalReplot();
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

  emit signalReplot();
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

  emit signalReplot();
}

void
TimeDisplayForm::setTriggerTagKey(const std::string &key)
{
  d_trig_tag_key = key;
  d_tr_tag_key_act->setText(QString().fromStdString(d_trig_tag_key));

  emit signalReplot();
}

std::string
TimeDisplayForm::getTriggerTagKey() const
{
  return d_trig_tag_key;
}



/********************************************************************
 * Notification messages from the control panel
 *******************************************************************/

void
TimeDisplayForm::notifyYAxisPlus()
{
#if QWT_VERSION < 0x060100
  QwtScaleDiv *ax = getPlot()->axisScaleDiv(QwtPlot::yLeft);
  double range = ax->upperBound() - ax->lowerBound();
  double step = range/20.0;
  getPlot()->setYaxis(ax->lowerBound()+step, ax->upperBound()+step);

#else

  QwtScaleDiv ax = getPlot()->axisScaleDiv(QwtPlot::yLeft);
  double range = ax.upperBound() - ax.lowerBound();
  double step = range/20.0;
  getPlot()->setYaxis(ax.lowerBound()+step, ax.upperBound()+step);
#endif
}

void
TimeDisplayForm::notifyYAxisMinus()
{
#if QWT_VERSION < 0x060100
  QwtScaleDiv *ax = getPlot()->axisScaleDiv(QwtPlot::yLeft);
  double range = ax->upperBound() - ax->lowerBound();
  double step = range/20.0;
  getPlot()->setYaxis(ax->lowerBound()-step, ax->upperBound()-step);

#else

  QwtScaleDiv ax = getPlot()->axisScaleDiv(QwtPlot::yLeft);
  double range = ax.upperBound() - ax.lowerBound();
  double step = range/20.0;
  getPlot()->setYaxis(ax.lowerBound()-step, ax.upperBound()-step);
#endif
}

void
TimeDisplayForm::notifyYRangePlus()
{
#if QWT_VERSION < 0x060100
  QwtScaleDiv *ax = getPlot()->axisScaleDiv(QwtPlot::yLeft);
  double range = ax->upperBound() - ax->lowerBound();
  double step = range/20.0;
  getPlot()->setYaxis(ax->lowerBound()-step, ax->upperBound()+step);

#else

  QwtScaleDiv ax = getPlot()->axisScaleDiv(QwtPlot::yLeft);
  double range = ax.upperBound() - ax.lowerBound();
  double step = range/20.0;
  getPlot()->setYaxis(ax.lowerBound()-step, ax.upperBound()+step);
#endif
}

void
TimeDisplayForm::notifyYRangeMinus()
{
#if QWT_VERSION < 0x060100
  QwtScaleDiv *ax = getPlot()->axisScaleDiv(QwtPlot::yLeft);
  double range = ax->upperBound() - ax->lowerBound();
  double step = range/20.0;
  getPlot()->setYaxis(ax->lowerBound()+step, ax->upperBound()-step);

#else

  QwtScaleDiv ax = getPlot()->axisScaleDiv(QwtPlot::yLeft);
  double range = ax.upperBound() - ax.lowerBound();
  double step = range/20.0;
  getPlot()->setYaxis(ax.lowerBound()+step, ax.upperBound()-step);
#endif
}


void
TimeDisplayForm::notifyXAxisPlus()
{
  // increase by 10%
  setNPoints(static_cast<int>(1.1*getNPoints()));
}

void
TimeDisplayForm::notifyXAxisMinus()
{
  // decrease by 10%
  setNPoints(static_cast<int>(0.9*getNPoints()));
}



void
TimeDisplayForm::notifyTriggerMode(const QString &mode)
{
  if(mode == "Free") {
    setTriggerMode(gr::qtgui::TRIG_MODE_FREE);
  }
  else if(mode == "Auto") {
    setTriggerMode(gr::qtgui::TRIG_MODE_AUTO);
  }
  else if(mode == "Normal") {
    setTriggerMode(gr::qtgui::TRIG_MODE_NORM);
  }
  else if(mode == "Tag") {
    setTriggerMode(gr::qtgui::TRIG_MODE_TAG);
    updateTrigger(gr::qtgui::TRIG_MODE_TAG);
  }
}

void
TimeDisplayForm::notifyTriggerSlope(const QString &slope)
{
  if(slope == "Positive") {
    setTriggerSlope(gr::qtgui::TRIG_SLOPE_POS);
  }
  else if(slope == "Negative") {
    setTriggerSlope(gr::qtgui::TRIG_SLOPE_NEG);
  }
}

void
TimeDisplayForm::notifyTriggerLevelPlus()
{
#if QWT_VERSION < 0x060100
  QwtScaleDiv *ax = getPlot()->axisScaleDiv(QwtPlot::yLeft);
  double range = ax->upperBound() - ax->lowerBound();

#else

  QwtScaleDiv ax = getPlot()->axisScaleDiv(QwtPlot::yLeft);
  double range = ax.upperBound() - ax.lowerBound();
#endif

  double step = range/20.0;
  emit signalTriggerLevel(getTriggerLevel() + step);
}

void
TimeDisplayForm::notifyTriggerLevelMinus()
{
#if QWT_VERSION < 0x060100
  QwtScaleDiv *ax = getPlot()->axisScaleDiv(QwtPlot::yLeft);
  double range = ax->upperBound() - ax->lowerBound();

#else

  QwtScaleDiv ax = getPlot()->axisScaleDiv(QwtPlot::yLeft);
  double range = ax.upperBound() - ax.lowerBound();
#endif

  double step = range/20.0;
  emit signalTriggerLevel(getTriggerLevel() - step);
}

void
TimeDisplayForm::notifyTriggerDelayPlus()
{
#if QWT_VERSION < 0x060100
  QwtScaleDiv *ax = getPlot()->axisScaleDiv(QwtPlot::xBottom);
  double range = ax->upperBound() - ax->lowerBound();

#else

  QwtScaleDiv ax = getPlot()->axisScaleDiv(QwtPlot::xBottom);
  double range = ax.upperBound() - ax.lowerBound();
#endif

  double step = range/20.0;
  double trig = getTriggerDelay() + step / d_current_units;
  emit signalTriggerDelay(trig);
}

void
TimeDisplayForm::notifyTriggerDelayMinus()
{
#if QWT_VERSION < 0x060100
  QwtScaleDiv *ax = getPlot()->axisScaleDiv(QwtPlot::xBottom);
  double range = ax->upperBound() - ax->lowerBound();

#else

  QwtScaleDiv ax = getPlot()->axisScaleDiv(QwtPlot::xBottom);
  double range = ax.upperBound() - ax.lowerBound();
#endif

  double step = range/20.0;
  double trig = getTriggerDelay() - step / d_current_units;
  if(trig < 0)
    trig = 0;
  emit signalTriggerDelay(trig);
}

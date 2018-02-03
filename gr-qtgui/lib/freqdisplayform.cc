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
#include <QSpacerItem>
#include <gnuradio/qtgui/freqdisplayform.h>
#include <gnuradio/qtgui/freqcontrolpanel.h>
#include <iostream>

FreqDisplayForm::FreqDisplayForm(int nplots, QWidget* parent)
  : DisplayForm(nplots, parent)
{
  d_int_validator = new QIntValidator(this);
  d_int_validator->setBottom(0);

  d_layout = new QGridLayout(this);
  d_display_plot = new FrequencyDisplayPlot(nplots, this);
  d_layout->addWidget(d_display_plot, 0, 0);

  d_layout->setColumnStretch(0, 1);
  setLayout(d_layout);

  d_controlpanel = NULL;

  d_num_real_data_points = 1024;
  d_fftsize = 1024;
  d_fftavg = 1.0;
  d_clicked = false;
  d_clicked_freq = 0;

  d_trig_mode = gr::qtgui::TRIG_MODE_FREE;
  d_trig_level = 0;
  d_trig_channel = 0;
  d_trig_tag_key = "";

  d_sizemenu = new FFTSizeMenu(this);
  d_avgmenu = new FFTAverageMenu(this);
  d_winmenu = new FFTWindowMenu(this);
  d_menu->addMenu(d_sizemenu);
  d_menu->addMenu(d_avgmenu);
  d_menu->addMenu(d_winmenu);
  connect(d_sizemenu, SIGNAL(whichTrigger(int)),
	  this, SLOT(setFFTSize(const int)));
  connect(d_avgmenu, SIGNAL(whichTrigger(float)),
	  this, SLOT(setFFTAverage(const float)));
  connect(d_winmenu, SIGNAL(whichTrigger(gr::filter::firdes::win_type)),
	  this, SLOT(setFFTWindowType(const gr::filter::firdes::win_type)));

  PopupMenu *maxymenu = new PopupMenu("Y Max", this);
  d_menu->addAction(maxymenu);
  connect(maxymenu, SIGNAL(whichTrigger(QString)),
	  this, SLOT(setYMax(QString)));

  PopupMenu *minymenu = new PopupMenu("Y Min", this);
  d_menu->addAction(minymenu);
  connect(minymenu, SIGNAL(whichTrigger(QString)),
	  this, SLOT(setYMin(QString)));

  d_maxhold_act = new QAction("Max Hold", this);
  d_maxhold_act->setCheckable(true);
  d_menu->addAction(d_maxhold_act);
  connect(d_maxhold_act, SIGNAL(triggered(bool)),
	  this, SLOT(notifyMaxHold(bool)));
  d_minhold_act = new QAction("Min Hold", this);
  d_minhold_act->setCheckable(true);
  d_menu->addAction(d_minhold_act);
  connect(d_minhold_act, SIGNAL(triggered(bool)),
	  this, SLOT(notifyMinHold(bool)));

  // Set up the trigger menu
  d_triggermenu = new QMenu("Trigger", this);
  d_tr_mode_menu = new TriggerModeMenu(this);
  d_tr_level_act = new PopupMenu("Level", this);
  d_tr_channel_menu = new TriggerChannelMenu(nplots, this);
  d_tr_tag_key_act = new PopupMenu("Tag Key", this);
  d_triggermenu->addMenu(d_tr_mode_menu);
  d_triggermenu->addAction(d_tr_level_act);
  d_triggermenu->addMenu(d_tr_channel_menu);
  d_triggermenu->addAction(d_tr_tag_key_act);
  d_menu->addMenu(d_triggermenu);

  d_controlpanelmenu = new QAction("Control Panel", this);
  d_controlpanelmenu->setCheckable(true);
  d_menu->addAction(d_controlpanelmenu);
  connect(d_controlpanelmenu, SIGNAL(triggered(bool)),
	  this, SLOT(setupControlPanel(bool)));

  setTriggerMode(gr::qtgui::TRIG_MODE_FREE);
  connect(d_tr_mode_menu, SIGNAL(whichTrigger(gr::qtgui::trigger_mode)),
          this, SLOT(setTriggerMode(gr::qtgui::trigger_mode)));
  // updates trigger state by calling set level or set tag key.
  connect(d_tr_mode_menu, SIGNAL(whichTrigger(gr::qtgui::trigger_mode)),
          this, SLOT(updateTrigger(gr::qtgui::trigger_mode)));

  setTriggerLevel(0);
  connect(d_tr_level_act, SIGNAL(whichTrigger(QString)),
          this, SLOT(setTriggerLevel(QString)));
  connect(this, SIGNAL(signalTriggerLevel(float)),
          this, SLOT(setTriggerLevel(float)));

  setTriggerChannel(0);
  connect(d_tr_channel_menu, SIGNAL(whichTrigger(int)),
          this, SLOT(setTriggerChannel(int)));

  setTriggerTagKey(std::string(""));
  connect(d_tr_tag_key_act, SIGNAL(whichTrigger(QString)),
          this, SLOT(setTriggerTagKey(QString)));

  connect(this, SIGNAL(signalClearMaxData()),
          getPlot(), SLOT(clearMaxData()));
  connect(this, SIGNAL(signalClearMinData()),
          getPlot(), SLOT(clearMinData()));
  connect(this, SIGNAL(signalSetMaxFFTVisible(bool)),
          getPlot(), SLOT(setMaxFFTVisible(bool)));
  connect(this, SIGNAL(signalSetMinFFTVisible(bool)),
          getPlot(), SLOT(setMinFFTVisible(bool)));

  Reset();

  connect(d_display_plot, SIGNAL(plotPointSelected(const QPointF)),
	  this, SLOT(onPlotPointSelected(const QPointF)));

  connect(this, SIGNAL(signalReplot()),
          getPlot(), SLOT(replot()));
}

FreqDisplayForm::~FreqDisplayForm()
{
  // Qt deletes children when parent is deleted

  // Don't worry about deleting Display Plots - they are deleted when parents are deleted
  delete d_int_validator;

  teardownControlPanel();
}

void
FreqDisplayForm::setupControlPanel(bool en)
{
  if(en) {
    setupControlPanel();
  }
  else {
    teardownControlPanel();
  }
}

void
FreqDisplayForm::setupControlPanel()
{
  if(d_controlpanel)
    delete d_controlpanel;

  // Create the control panel layout
  d_controlpanel = new FreqControlPanel(this);

  // Connect action items in menu to controlpanel widgets
  connect(d_grid_act, SIGNAL(triggered(bool)),
          d_controlpanel, SLOT(toggleGrid(bool)));
  connect(d_axislabelsmenu, SIGNAL(triggered(bool)),
          d_controlpanel, SLOT(toggleAxisLabels(bool)));
  connect(d_sizemenu, SIGNAL(whichTrigger(int)),
	  d_controlpanel, SLOT(toggleFFTSize(int)));
  connect(d_winmenu, SIGNAL(whichTrigger(gr::filter::firdes::win_type)),
	  d_controlpanel, SLOT(toggleFFTWindow(gr::filter::firdes::win_type)));
  connect(this, SIGNAL(signalFFTSize(int)),
	  d_controlpanel, SLOT(toggleFFTSize(int)));
  connect(this, SIGNAL(signalFFTWindow(gr::filter::firdes::win_type)),
	  d_controlpanel, SLOT(toggleFFTWindow(gr::filter::firdes::win_type)));
  connect(d_maxhold_act, SIGNAL(triggered(bool)),
          d_controlpanel, SLOT(toggleMaxHold(bool)));
  connect(d_minhold_act, SIGNAL(triggered(bool)),
          d_controlpanel, SLOT(toggleMinHold(bool)));
  connect(d_avgmenu, SIGNAL(whichTrigger(float)),
          d_controlpanel, SLOT(setFFTAverage(float)));
  connect(d_tr_mode_menu, SIGNAL(whichTrigger(gr::qtgui::trigger_mode)),
	  d_controlpanel, SLOT(toggleTriggerMode(gr::qtgui::trigger_mode)));
  connect(this, SIGNAL(signalTriggerMode(gr::qtgui::trigger_mode)),
	  d_controlpanel, SLOT(toggleTriggerMode(gr::qtgui::trigger_mode)));
  connect(d_stop_act, SIGNAL(triggered()),
          d_controlpanel, SLOT(toggleStopButton()));

  d_layout->addLayout(d_controlpanel, 0, 1);

  d_controlpanel->toggleGrid(d_grid_act->isChecked());
  d_controlpanel->toggleAxisLabels(d_axislabelsmenu->isChecked());
  d_controlpanelmenu->setChecked(true);
  d_controlpanel->toggleTriggerMode(getTriggerMode());
  d_controlpanel->toggleMaxHold(d_maxhold_act->isChecked());
  d_controlpanel->toggleMinHold(d_minhold_act->isChecked());
  d_controlpanel->setFFTAverage(getFFTAverage());

  emit signalFFTSize(getFFTSize());
  emit signalFFTWindow(getFFTWindowType());
}

void
FreqDisplayForm::teardownControlPanel()
{
  if(d_controlpanel) {
    d_layout->removeItem(d_controlpanel);
    delete d_controlpanel;
    d_controlpanel = NULL;
  }
  d_controlpanelmenu->setChecked(false);
}

FrequencyDisplayPlot*
FreqDisplayForm::getPlot()
{
  return ((FrequencyDisplayPlot*)d_display_plot);
}

void
FreqDisplayForm::newData(const QEvent *updateEvent)
{
  FreqUpdateEvent *fevent = (FreqUpdateEvent*)updateEvent;
  const std::vector<double*> dataPoints = fevent->getPoints();
  const uint64_t numDataPoints = fevent->getNumDataPoints();

  getPlot()->plotNewData(dataPoints, numDataPoints,
			 0, 0, 0, d_update_time);
}

void
FreqDisplayForm::customEvent( QEvent * e)
{
  if(e->type() == FreqUpdateEvent::Type()) {
    newData(e);
  }
  else if(e->type() == SpectrumFrequencyRangeEventType) {
    SetFreqEvent *fevent = (SetFreqEvent*)e;
    setFrequencyRange(fevent->getCenterFrequency(), fevent->getBandwidth());
  }
}

int
FreqDisplayForm::getFFTSize() const
{
  return d_fftsize;
}

float
FreqDisplayForm::getFFTAverage() const
{
  return d_fftavg;
}

gr::filter::firdes::win_type
FreqDisplayForm::getFFTWindowType() const
{
  return d_fftwintype;
}

void
FreqDisplayForm::setSampleRate(const QString &samprate)
{
  setFrequencyRange(d_center_freq, samprate.toDouble());
}

void
FreqDisplayForm::setFFTSize(const int newsize)
{
  d_fftsize = newsize;
  d_sizemenu->getActionFromSize(newsize)->setChecked(true);

  emit signalReplot();
  emit signalFFTSize(newsize);
}

void
FreqDisplayForm::setFFTAverage(const float newavg)
{
  d_fftavg = newavg;
  d_avgmenu->getActionFromAvg(newavg)->setChecked(true);
  emit signalReplot();
  //emit signalFFTAverage(newavg);
}

void
FreqDisplayForm::setFFTWindowType(const gr::filter::firdes::win_type newwin)
{
  d_fftwintype = newwin;
  d_winmenu->getActionFromWindow(newwin)->setChecked(true);
  emit signalReplot();
  emit signalFFTWindow(newwin);
}

void
FreqDisplayForm::setFrequencyRange(const double centerfreq,
				   const double bandwidth)
{
  std::string strunits[4] = {"Hz", "kHz", "MHz", "GHz"};
  double units10 = floor(log10(bandwidth));
  double units3  = std::max(floor(units10 / 3.0), 0.0);
  d_units = pow(10, (units10-fmod(units10, 3.0)));
  int iunit = static_cast<int>(units3);

  d_center_freq = centerfreq;
  d_samp_rate = bandwidth;

  getPlot()->setFrequencyRange(centerfreq, bandwidth,
			       d_units, strunits[iunit]);
}

void
FreqDisplayForm::setYaxis(double min, double max)
{
  getPlot()->setYaxis(min, max);
}

void FreqDisplayForm::setYLabel(const std::string &label,
                                const std::string &unit)
{
  getPlot()->setYLabel(label, unit);
}

void
FreqDisplayForm::setYMax(const QString &m)
{
  double new_max = m.toDouble();
  double cur_ymin = getPlot()->getYMin();
  if(new_max > cur_ymin)
    setYaxis(cur_ymin, new_max);
}

void
FreqDisplayForm::setYMin(const QString &m)
{
  double new_min = m.toDouble();
  double cur_ymax = getPlot()->getYMax();
  if(new_min < cur_ymax)
    setYaxis(new_min, cur_ymax);
}

void
FreqDisplayForm::autoScale(bool en)
{
  if(en) {
    d_autoscale_state = true;
  }
  else {
    d_autoscale_state = false;
  }

  d_autoscale_act->setChecked(en);
  getPlot()->setAutoScale(d_autoscale_state);
  emit signalReplot();
}

void
FreqDisplayForm::autoScaleShot()
{
  getPlot()->setAutoScaleShot();
  emit signalReplot();
}

void
FreqDisplayForm::setPlotPosHalf(bool half)
{
  getPlot()->setPlotPosHalf(half);
  emit signalReplot();
}

void
FreqDisplayForm::clearMaxHold()
{
  getPlot()->clearMaxData();
}

void
FreqDisplayForm::clearMinHold()
{
  getPlot()->clearMinData();
}

void
FreqDisplayForm::onPlotPointSelected(const QPointF p)
{
  d_clicked = true;
  d_clicked_freq = d_units*p.x();
}

bool
FreqDisplayForm::checkClicked()
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
FreqDisplayForm::getClickedFreq() const
{
  return d_clicked_freq;
}


/********************************************************************
 * TRIGGER METHODS
 *******************************************************************/

void
FreqDisplayForm::setTriggerMode(gr::qtgui::trigger_mode mode)
{
  d_trig_mode = mode;
  d_tr_mode_menu->getAction(mode)->setChecked(true);

  if((d_trig_mode == gr::qtgui::TRIG_MODE_AUTO) || (d_trig_mode == gr::qtgui::TRIG_MODE_NORM)) {
    getPlot()->attachTriggerLine(true);
  }
  else {
    getPlot()->attachTriggerLine(false);
  }

  emit signalReplot();
  emit signalTriggerMode(mode);
}

void
FreqDisplayForm::updateTrigger(gr::qtgui::trigger_mode mode)
{
  // If auto or normal mode, popup trigger level box to set
  if((d_trig_mode == gr::qtgui::TRIG_MODE_AUTO) || (d_trig_mode == gr::qtgui::TRIG_MODE_NORM)) {
    d_tr_level_act->activate(QAction::Trigger);
    getPlot()->attachTriggerLine(true);
  }
  else {
    getPlot()->attachTriggerLine(false);
  }

  // if tag mode, popup tag key box to set
  if((d_trig_tag_key == "") && (d_trig_mode == gr::qtgui::TRIG_MODE_TAG))
    d_tr_tag_key_act->activate(QAction::Trigger);

  emit signalReplot();
  emit signalTriggerMode(mode);
}

gr::qtgui::trigger_mode
FreqDisplayForm::getTriggerMode() const
{
  return d_trig_mode;
}

void
FreqDisplayForm::setTriggerLevel(QString s)
{
  d_trig_level = s.toFloat();

  if((d_trig_mode == gr::qtgui::TRIG_MODE_AUTO) || (d_trig_mode == gr::qtgui::TRIG_MODE_NORM)) {
    getPlot()->setTriggerLine(d_trig_level);
  }

  emit signalReplot();
}

void
FreqDisplayForm::setTriggerLevel(float level)
{
  d_trig_level = level;
  d_tr_level_act->setText(QString().setNum(d_trig_level));

  if((d_trig_mode == gr::qtgui::TRIG_MODE_AUTO) || (d_trig_mode == gr::qtgui::TRIG_MODE_NORM)) {
    getPlot()->setTriggerLine(d_trig_level);
  }

  emit signalReplot();
}

float
FreqDisplayForm::getTriggerLevel() const
{
  return d_trig_level;
}

void
FreqDisplayForm::setTriggerChannel(int channel)
{
  d_trig_channel = channel;
  d_tr_channel_menu->getAction(d_trig_channel)->setChecked(true);
  emit signalReplot();
}

int
FreqDisplayForm::getTriggerChannel() const
{
  return d_trig_channel;
}

void
FreqDisplayForm::setTriggerTagKey(QString s)
{
  d_trig_tag_key = s.toStdString();
}

void
FreqDisplayForm::setTriggerTagKey(const std::string &key)
{
  d_trig_tag_key = key;
  d_tr_tag_key_act->setText(QString().fromStdString(d_trig_tag_key));
  emit signalReplot();
}

std::string
FreqDisplayForm::getTriggerTagKey() const
{
  return d_trig_tag_key;
}



/********************************************************************
 * Notification messages from the control panel
 *******************************************************************/


void
FreqDisplayForm::notifyYAxisPlus()
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
FreqDisplayForm::notifyYAxisMinus()
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
FreqDisplayForm::notifyYRangePlus()
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
FreqDisplayForm::notifyYRangeMinus()
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
FreqDisplayForm::notifyFFTSize(const QString &s)
{
  setFFTSize(s.toInt());
}

void
FreqDisplayForm::notifyFFTWindow(const QString &s)
{
  if(s == "None") {
    d_fftwintype = gr::filter::firdes::WIN_NONE;
  }
  else if(s == "Hamming") {
    d_fftwintype = gr::filter::firdes::WIN_HAMMING;
  }
  else if(s == "Hann") {
    d_fftwintype = gr::filter::firdes::WIN_HANN;
  }
  else if(s == "Blackman") {
    d_fftwintype = gr::filter::firdes::WIN_BLACKMAN;
  }
  else if(s == "Blackman-harris") {
    d_fftwintype = gr::filter::firdes::WIN_BLACKMAN_hARRIS;
  }
  else if(s == "Rectangular") {
    d_fftwintype = gr::filter::firdes::WIN_RECTANGULAR;
  }
  else if(s == "Kaiser") {
    d_fftwintype = gr::filter::firdes::WIN_KAISER;
  }
  else if(s == "Flat-top") {
    d_fftwintype = gr::filter::firdes::WIN_FLATTOP;
  }

  d_winmenu->getActionFromWindow(d_fftwintype)->setChecked(true);
  emit signalReplot();
}


void
FreqDisplayForm::notifyMaxHold(bool en)
{
  d_maxhold_act->setChecked(en);
  emit signalClearMaxData();
  emit signalSetMaxFFTVisible(en);
}

void
FreqDisplayForm::notifyMinHold(bool en)
{
  d_minhold_act->setChecked(en);
  emit signalClearMinData();
  emit signalSetMinFFTVisible(en);
}


void
FreqDisplayForm::notifyTriggerMode(const QString &mode)
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
FreqDisplayForm::notifyTriggerLevelPlus()
{
  emit signalTriggerLevel(getTriggerLevel() + 1);
}

void
FreqDisplayForm::notifyTriggerLevelMinus()
{
  emit signalTriggerLevel(getTriggerLevel() - 1);
}

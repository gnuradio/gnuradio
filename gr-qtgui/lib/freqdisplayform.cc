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
#include <gnuradio/qtgui/freqdisplayform.h>
#include <iostream>

FreqDisplayForm::FreqDisplayForm(int nplots, QWidget* parent)
  : DisplayForm(nplots, parent)
{
  d_int_validator = new QIntValidator(this);
  d_int_validator->setBottom(0);

  d_layout = new QGridLayout(this);
  d_display_plot = new FrequencyDisplayPlot(nplots, this);
  d_layout->addWidget(d_display_plot, 0, 0);
  setLayout(d_layout);

  d_num_real_data_points = 1024;
  d_fftsize = 1024;
  d_fftavg = 1.0;
  d_clicked = false;
  d_clicked_freq = 0;

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

FreqDisplayForm::~FreqDisplayForm()
{
  // Qt deletes children when parent is deleted

  // Don't worry about deleting Display Plots - they are deleted when parents are deleted
  delete d_int_validator;
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
  getPlot()->replot();
}

void
FreqDisplayForm::setFFTAverage(const float newavg)
{
  d_fftavg = newavg;
  d_avgmenu->getActionFromAvg(newavg)->setChecked(true);
  getPlot()->replot();
}

void
FreqDisplayForm::setFFTWindowType(const gr::filter::firdes::win_type newwin)
{
  d_fftwintype = newwin;
  d_winmenu->getActionFromWindow(newwin)->setChecked(true);
  getPlot()->replot();
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
  getPlot()->replot();
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

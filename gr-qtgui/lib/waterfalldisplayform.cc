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
#include <QColorDialog>
#include <QMessageBox>
#include <gnuradio/qtgui/waterfalldisplayform.h>
#include <iostream>

WaterfallDisplayForm::WaterfallDisplayForm(int nplots, QWidget* parent)
  : DisplayForm(nplots, parent)
{
  _intValidator = new QIntValidator(this);
  _intValidator->setBottom(0);

  _layout = new QGridLayout(this);
  _displayPlot = new WaterfallDisplayPlot(nplots, this);
  _layout->addWidget(_displayPlot, 0, 0);
  setLayout(_layout);

  _center_freq = 0;
  _samp_rate = 0;

  _numRealDataPoints = 1024;
  _fftsize = 1024;
  _fftavg = 1.0;

  _min_val =  1000;
  _max_val = -1000;

  // We don't use the normal menus that are part of the displayform.
  // Clear them out to get rid of their resources.
  for(int i = 0; i < nplots; i++) {
    _lines_menu[i]->clear();
  }
  _line_title_act.clear();
  _line_color_menu.clear();
  _line_width_menu.clear();
  _line_style_menu.clear();
  _line_marker_menu.clear();
  _marker_alpha_menu.clear();

  // Now create our own menus
  for(int i = 0; i < nplots; i++) {
    ColorMapMenu *colormap = new ColorMapMenu(i, this);
    connect(colormap, SIGNAL(whichTrigger(int, const int, const QColor&, const QColor&)),
	    this, SLOT(setColorMap(int, const int, const QColor&, const QColor&)));
    _lines_menu[i]->addMenu(colormap);

    _marker_alpha_menu.push_back(new MarkerAlphaMenu(i, this));
    connect(_marker_alpha_menu[i], SIGNAL(whichTrigger(int, int)),
	    this, SLOT(setAlpha(int, int)));
    _lines_menu[i]->addMenu(_marker_alpha_menu[i]);
  }

  // One scales once when clicked, so no on/off toggling
  _autoscale_act->setText(tr("Auto Scale"));
  _autoscale_act->setCheckable(false);

  FFTSizeMenu *sizemenu = new FFTSizeMenu(this);
  FFTAverageMenu *avgmenu = new FFTAverageMenu(this);
  FFTWindowMenu *winmenu = new FFTWindowMenu(this);
  _menu->addMenu(sizemenu);
  _menu->addMenu(avgmenu);
  _menu->addMenu(winmenu);
  connect(sizemenu, SIGNAL(whichTrigger(int)),
	  this, SLOT(setFFTSize(const int)));
  connect(avgmenu, SIGNAL(whichTrigger(float)),
	  this, SLOT(setFFTAverage(const float)));
  connect(winmenu, SIGNAL(whichTrigger(gr::filter::firdes::win_type)),
	  this, SLOT(setFFTWindowType(const gr::filter::firdes::win_type)));

  Reset();

  connect(_displayPlot, SIGNAL(plotPointSelected(const QPointF)),
	  this, SLOT(onPlotPointSelected(const QPointF)));
}

WaterfallDisplayForm::~WaterfallDisplayForm()
{
  // Qt deletes children when parent is deleted

  // Don't worry about deleting Display Plots - they are deleted when parents are deleted
  delete _intValidator;
}

WaterfallDisplayPlot*
WaterfallDisplayForm::getPlot()
{
  return ((WaterfallDisplayPlot*)_displayPlot);
}

void
WaterfallDisplayForm::newData(const QEvent *updateEvent)
{
  WaterfallUpdateEvent *event = (WaterfallUpdateEvent*)updateEvent;
  const std::vector<double*> dataPoints = event->getPoints();
  const uint64_t numDataPoints = event->getNumDataPoints();
  const gr::high_res_timer_type dataTimestamp = event->getDataTimestamp();

  _min_val =  1000;
  _max_val = -1000;
  for(size_t i=0; i < dataPoints.size(); i++) {
    double *min_val = std::min_element(&dataPoints[i][0], &dataPoints[i][numDataPoints-1]);
    double *max_val = std::max_element(&dataPoints[i][0], &dataPoints[i][numDataPoints-1]);
    if(*min_val < _min_val)
      _min_val = *min_val;
    if(*max_val > _max_val)
      _max_val = *max_val;
  }

  getPlot()->plotNewData(dataPoints, numDataPoints, d_update_time, dataTimestamp, 0);
}

void
WaterfallDisplayForm::customEvent( QEvent * e)
{
  if(e->type() == WaterfallUpdateEvent::Type()) {
    newData(e);
  }
}

int
WaterfallDisplayForm::getFFTSize() const
{
  return _fftsize;
}

float
WaterfallDisplayForm::getFFTAverage() const
{
  return _fftavg;
}

gr::filter::firdes::win_type
WaterfallDisplayForm::getFFTWindowType() const
{
  return _fftwintype;
}

int
WaterfallDisplayForm::getColorMap(int which)
{
  return getPlot()->getIntensityColorMapType(which);
}

int
WaterfallDisplayForm::getAlpha(int which)
{
  return getPlot()->getAlpha(which);
}

double
WaterfallDisplayForm::getMinIntensity(int which)
{
  return getPlot()->getMinIntensity(which);
}

double
WaterfallDisplayForm::getMaxIntensity(int which)
{
  return getPlot()->getMaxIntensity(which);
}

void
WaterfallDisplayForm::setSampleRate(const QString &samprate)
{
  setFrequencyRange(_center_freq, samprate.toDouble());
}

void
WaterfallDisplayForm::setFFTSize(const int newsize)
{
  _fftsize = newsize;
}

void
WaterfallDisplayForm::setFFTAverage(const float newavg)
{
  _fftavg = newavg;
}

void
WaterfallDisplayForm::setFFTWindowType(const gr::filter::firdes::win_type newwin)
{
  _fftwintype = newwin;
}

void
WaterfallDisplayForm::setFrequencyRange(const double centerfreq,
					const double bandwidth)
{
  std::string strunits[4] = {"Hz", "kHz", "MHz", "GHz"};
  double units10 = floor(log10(bandwidth));
  double units3  = std::max(floor(units10 / 3.0), 0.0);
  double units = pow(10, (units10-fmod(units10, 3.0)));
  int iunit = static_cast<int>(units3);

  _center_freq = centerfreq;
  _samp_rate = bandwidth;

  getPlot()->setFrequencyRange(centerfreq, bandwidth,
			       units, strunits[iunit]);
  getPlot()->replot();
}

void
WaterfallDisplayForm::setColorMap(int which,
				  const int newType,
				  const QColor lowColor,
				  const QColor highColor)
{
  getPlot()->setIntensityColorMapType(which, newType,
				      lowColor, highColor);
  getPlot()->replot();
}

void
WaterfallDisplayForm::setAlpha(int which, int alpha)
{
  getPlot()->setAlpha(which, alpha);
  getPlot()->replot();
}

void
WaterfallDisplayForm::setIntensityRange(const double minIntensity,
					const double maxIntensity)
{
  getPlot()->setIntensityRange(minIntensity, maxIntensity);
  getPlot()->replot();
}

void
WaterfallDisplayForm::autoScale(bool en)
{
  double min_int = _min_val - 5;
  double max_int = _max_val + 10;

  getPlot()->setIntensityRange(min_int, max_int);
  getPlot()->replot();
}

void
WaterfallDisplayForm::clearData()
{
  getPlot()->clearData();
}

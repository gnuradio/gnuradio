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
#include <QColorDialog>
#include <QMessageBox>
#include <gnuradio/qtgui/waterfalldisplayform.h>
#include <iostream>

WaterfallDisplayForm::WaterfallDisplayForm(int nplots, QWidget* parent)
  : DisplayForm(nplots, parent)
{
  d_int_validator = new QIntValidator(this);
  d_int_validator->setBottom(0);

  d_layout = new QGridLayout(this);
  d_display_plot = new WaterfallDisplayPlot(nplots, this);
  d_layout->addWidget(d_display_plot, 0, 0);
  setLayout(d_layout);

  d_center_freq = 0;
  d_samp_rate = 0;

  d_fftsize = 1024;
  d_fftavg = 1.0;

  d_min_val =  1000;
  d_max_val = -1000;

  d_clicked = false;
  d_clicked_freq = 0;
  d_time_per_fft = 0;
  // We don't use the normal menus that are part of the displayform.
  // Clear them out to get rid of their resources.
  for(int i = 0; i < nplots; i++) {
    d_lines_menu[i]->clear();
  }
  d_line_title_act.clear();
  d_line_color_menu.clear();
  d_line_width_menu.clear();
  d_line_style_menu.clear();
  d_line_marker_menu.clear();
  d_marker_alpha_menu.clear();

  // Now create our own menus
  for(int i = 0; i < nplots; i++) {
    ColorMapMenu *colormap = new ColorMapMenu(i, this);
    connect(colormap, SIGNAL(whichTrigger(int, const int, const QColor&, const QColor&)),
	    this, SLOT(setColorMap(int, const int, const QColor&, const QColor&)));
    d_lines_menu[i]->addMenu(colormap);

    d_marker_alpha_menu.push_back(new MarkerAlphaMenu(i, this));
    connect(d_marker_alpha_menu[i], SIGNAL(whichTrigger(int, int)),
	    this, SLOT(setAlpha(int, int)));
    d_lines_menu[i]->addMenu(d_marker_alpha_menu[i]);
  }

  // One scales once when clicked, so no on/off toggling
  d_autoscale_act->setText(tr("Auto Scale"));
  d_autoscale_act->setCheckable(false);

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

  PopupMenu *maxintmenu = new PopupMenu("Int. Max", this);
  d_menu->addAction(maxintmenu);
  connect(maxintmenu, SIGNAL(whichTrigger(QString)),
	  this, SLOT(setMaxIntensity(QString)));

  PopupMenu *minintmenu = new PopupMenu("Int. Min", this);
  d_menu->addAction(minintmenu);
  connect(minintmenu, SIGNAL(whichTrigger(QString)),
	  this, SLOT(setMinIntensity(QString)));

  Reset();

  connect(d_display_plot, SIGNAL(plotPointSelected(const QPointF)),
	  this, SLOT(onPlotPointSelected(const QPointF)));
}

WaterfallDisplayForm::~WaterfallDisplayForm()
{
  // Qt deletes children when parent is deleted

  // Don't worry about deleting Display Plots - they are deleted when parents are deleted
  delete d_int_validator;
}

WaterfallDisplayPlot*
WaterfallDisplayForm::getPlot()
{
  return ((WaterfallDisplayPlot*)d_display_plot);
}

void
WaterfallDisplayForm::newData(const QEvent *updateEvent)
{
  WaterfallUpdateEvent *event = (WaterfallUpdateEvent*)updateEvent;
  const std::vector<double*> dataPoints = event->getPoints();
  const uint64_t numDataPoints = event->getNumDataPoints();
  const gr::high_res_timer_type dataTimestamp = event->getDataTimestamp();

  for(size_t i=0; i < dataPoints.size(); i++) {
    double *min_val = std::min_element(&dataPoints[i][0], &dataPoints[i][numDataPoints-1]);
    double *max_val = std::max_element(&dataPoints[i][0], &dataPoints[i][numDataPoints-1]);
    if(*min_val < d_min_val)
      d_min_val = *min_val;
    if(*max_val > d_max_val)
      d_max_val = *max_val;
  }

  getPlot()->plotNewData(dataPoints, numDataPoints, d_time_per_fft, dataTimestamp, 0);
}

void
WaterfallDisplayForm::customEvent( QEvent * e)
{
  if(e->type() == WaterfallUpdateEvent::Type()) {
    newData(e);
  }
  else if(e->type() == SpectrumFrequencyRangeEventType) {
    SetFreqEvent *fevent = (SetFreqEvent*)e;
    setFrequencyRange(fevent->getCenterFrequency(), fevent->getBandwidth());
  }
}

int
WaterfallDisplayForm::getFFTSize() const
{
  return d_fftsize;
}

float
WaterfallDisplayForm::getFFTAverage() const
{
  return d_fftavg;
}

gr::filter::firdes::win_type
WaterfallDisplayForm::getFFTWindowType() const
{
  return d_fftwintype;
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
  setFrequencyRange(d_center_freq, samprate.toDouble());
}

void
WaterfallDisplayForm::setFFTSize(const int newsize)
{
  d_fftsize = newsize;
  d_sizemenu->getActionFromSize(newsize)->setChecked(true);
  getPlot()->replot();
}

void
WaterfallDisplayForm::setFFTAverage(const float newavg)
{
  d_fftavg = newavg;
  d_avgmenu->getActionFromAvg(newavg)->setChecked(true);
  getPlot()->replot();
}

void
WaterfallDisplayForm::setFFTWindowType(const gr::filter::firdes::win_type newwin)
{
  d_fftwintype = newwin;
  d_winmenu->getActionFromWindow(newwin)->setChecked(true);
  getPlot()->replot();
}

void
WaterfallDisplayForm::setFrequencyRange(const double centerfreq,
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
  // reset max and min values
  d_min_val =  1000;
  d_max_val = -1000;

  d_cur_min_val = minIntensity;
  d_cur_max_val = maxIntensity;
  getPlot()->setIntensityRange(minIntensity, maxIntensity);
  getPlot()->replot();
}

void
WaterfallDisplayForm::setMaxIntensity(const QString &m)
{
  double new_max = m.toDouble();
  if(new_max > d_cur_min_val)
    setIntensityRange(d_cur_min_val, new_max);
}

void
WaterfallDisplayForm::setMinIntensity(const QString &m)
{
  double new_min = m.toDouble();
  if(new_min < d_cur_max_val)
    setIntensityRange(new_min, d_cur_max_val);
}

void
WaterfallDisplayForm::autoScale(bool en)
{
  double min_int = d_min_val - 5;
  double max_int = d_max_val + 10;

  setIntensityRange(min_int, max_int);
}

void
WaterfallDisplayForm::clearData()
{
  getPlot()->clearData();
}

void
WaterfallDisplayForm::onPlotPointSelected(const QPointF p)
{
  d_clicked = true;
  d_clicked_freq = d_units*p.x();
}

bool
WaterfallDisplayForm::checkClicked()
{
  if(d_clicked) {
    d_clicked = false;
    return true;
  }
  else {
    return false;
  }
}

void
WaterfallDisplayForm::setTimeTitle(const std::string title) {
    getPlot()->setAxisTitle(QwtPlot::yLeft, title.c_str());
}

float
WaterfallDisplayForm::getClickedFreq() const
{
  return d_clicked_freq;
}

void
WaterfallDisplayForm::setPlotPosHalf(bool half)
{
  getPlot()->setPlotPosHalf(half);
  getPlot()->replot();
}

void
WaterfallDisplayForm::setTimePerFFT(double t)
{
   d_time_per_fft = t;
}

double WaterfallDisplayForm::getTimePerFFT()
{
   return d_time_per_fft;
}
// Override displayform SetUpdateTime() to set FFT time
void
WaterfallDisplayForm::setUpdateTime(double t)
{
   d_update_time = t;
   // Assume times are equal unless explicitly told by setTimePerFFT()
   // This is the case when plotting using gr_spectrogram_plot
   d_time_per_fft = t;
}


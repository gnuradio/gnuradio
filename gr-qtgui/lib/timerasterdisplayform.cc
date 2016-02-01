/* -*- c++ -*- */
/*
 * Copyright 2012,2013 Free Software Foundation, Inc.
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
#include <gnuradio/qtgui/timerasterdisplayform.h>
#include <iostream>

TimeRasterDisplayForm::TimeRasterDisplayForm(int nplots,
					     double samp_rate,
					     double rows, double cols,
					     double zmax,
					     QWidget* parent)
  : DisplayForm(nplots, parent)
{
#if QWT_VERSION < 0x060000
  std::cerr << "Warning: QWT5 has been found which has serious performance issues with raster plots."
            << std::endl << "         Consider updating to QWT version 6 to use the time raster GUIs."
            << std::endl << std::endl;
#endif

  d_layout = new QGridLayout(this);
  d_display_plot = new TimeRasterDisplayPlot(nplots, samp_rate, rows, cols, this);
  d_layout->addWidget(d_display_plot, 0, 0);
  setLayout(d_layout);

  d_min_val =  10;
  d_max_val = -10;

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
    d_line_title_act.push_back(new LineTitleAction(i, this));
    connect(d_line_title_act[i], SIGNAL(whichTrigger(int, const QString&)),
	    this, SLOT(setLineLabel(int, const QString&)));
    d_lines_menu[i]->addAction(d_line_title_act[i]);

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

  PopupMenu *colsmenu = new PopupMenu("Num. Columns", this);
  d_menu->addAction(colsmenu);
  connect(colsmenu, SIGNAL(whichTrigger(QString)),
	  this, SLOT(setNumCols(QString)));

  PopupMenu *rowsmenu = new PopupMenu("Num. Rows", this);
  d_menu->addAction(rowsmenu);
  connect(rowsmenu, SIGNAL(whichTrigger(QString)),
	  this, SLOT(setNumRows(QString)));

  PopupMenu *maxintmenu = new PopupMenu("Int. Max", this);
  d_menu->addAction(maxintmenu);
  connect(maxintmenu, SIGNAL(whichTrigger(QString)),
	  this, SLOT(setMaxIntensity(QString)));

  PopupMenu *minintmenu = new PopupMenu("Int. Min", this);
  d_menu->addAction(minintmenu);
  connect(minintmenu, SIGNAL(whichTrigger(QString)),
	  this, SLOT(setMinIntensity(QString)));

  getPlot()->setIntensityRange(0, zmax);

  Reset();

  connect(d_display_plot, SIGNAL(plotPointSelected(const QPointF)),
	  this, SLOT(onPlotPointSelected(const QPointF)));
}

TimeRasterDisplayForm::~TimeRasterDisplayForm()
{
  // Don't worry about deleting Display Plots - they are deleted when
  // parents are deleted
}

TimeRasterDisplayPlot*
TimeRasterDisplayForm::getPlot()
{
  return ((TimeRasterDisplayPlot*)d_display_plot);
}

double
TimeRasterDisplayForm::numRows()
{
  return getPlot()->numRows();
}

double
TimeRasterDisplayForm::numCols()
{
  return getPlot()->numCols();
}

int
TimeRasterDisplayForm::getColorMap(int which)
{
  return getPlot()->getIntensityColorMapType(which);
}

int
TimeRasterDisplayForm::getAlpha(int which)
{
  return getPlot()->getAlpha(which);
}

double
TimeRasterDisplayForm::getMinIntensity(int which)
{
  return getPlot()->getMinIntensity(which);
}

double
TimeRasterDisplayForm::getMaxIntensity(int which)
{
  return getPlot()->getMaxIntensity(which);
}


void
TimeRasterDisplayForm::newData(const QEvent *updateEvent)
{
  TimeRasterUpdateEvent *event = (TimeRasterUpdateEvent*)updateEvent;
  const std::vector<double*> dataPoints = event->getPoints();
  const uint64_t numDataPoints = event->getNumDataPoints();

  for(size_t i=0; i < dataPoints.size(); i++) {
    double *min_val = std::min_element(&dataPoints[i][0], &dataPoints[i][numDataPoints-1]);
    double *max_val = std::max_element(&dataPoints[i][0], &dataPoints[i][numDataPoints-1]);
    if(*min_val < d_min_val)
      d_min_val = *min_val;
    if(*max_val > d_max_val)
      d_max_val = *max_val;
  }

  getPlot()->plotNewData(dataPoints, numDataPoints);
}

void
TimeRasterDisplayForm::customEvent( QEvent * e)
{
  if(e->type() == TimeRasterUpdateEvent::Type()) {
    newData(e);
  }
  else if(e->type() == TimeRasterSetSize::Type()) {
    double r = ((TimeRasterSetSize*)e)->nRows();
    double c = ((TimeRasterSetSize*)e)->nCols();
    getPlot()->setNumRows(r);
    getPlot()->setNumCols(c);
    getPlot()->replot();
  }
}

void
TimeRasterDisplayForm::setNumRows(double rows)
{
  getPlot()->setNumRows(rows);
  getPlot()->replot();
}

void
TimeRasterDisplayForm::setNumRows(QString rows)
{
  getPlot()->setNumRows(rows.toDouble());
  getPlot()->replot();
}

void
TimeRasterDisplayForm::setNumCols(double cols)
{
  getPlot()->setNumCols(cols);
  getPlot()->replot();
}

void
TimeRasterDisplayForm::setNumCols(QString cols)
{
  getPlot()->setNumCols(cols.toDouble());
  getPlot()->replot();
}

void
TimeRasterDisplayForm::setSampleRate(const double rate)
{
  getPlot()->setSampleRate(rate);
  getPlot()->replot();
}

void
TimeRasterDisplayForm::setSampleRate(const QString &rate)
{
  getPlot()->setSampleRate(rate.toDouble());
  getPlot()->replot();
}

void
TimeRasterDisplayForm::setColorMap(int which,
				   const int newType,
				   const QColor lowColor,
				   const QColor highColor)
{
  getPlot()->setIntensityColorMapType(which, newType,
				      lowColor, highColor);
  getPlot()->replot();
}

void
TimeRasterDisplayForm::setAlpha(int which, int alpha)
{
  getPlot()->setAlpha(which, alpha);
  getPlot()->replot();
}

void
TimeRasterDisplayForm::setIntensityRange(const double minIntensity,
                                         const double maxIntensity)
{
  // reset max and min values
  d_min_val =  10;
  d_max_val = -10;

  d_cur_min_val = minIntensity;
  d_cur_max_val = maxIntensity;
  getPlot()->setIntensityRange(minIntensity, maxIntensity);
  getPlot()->replot();
}

void
TimeRasterDisplayForm::setMaxIntensity(const QString &m)
{
  double new_max = m.toDouble();
  if(new_max > d_cur_min_val)
    setIntensityRange(d_cur_min_val, new_max);
}

void
TimeRasterDisplayForm::setMinIntensity(const QString &m)
{
  double new_min = m.toDouble();
  if(new_min < d_cur_max_val)
    setIntensityRange(new_min, d_cur_max_val);
}

void
TimeRasterDisplayForm::autoScale(bool en)
{
  setIntensityRange(d_min_val, d_max_val);
}

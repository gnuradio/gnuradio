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
#include <freqdisplayform.h>
#include <iostream>

FreqDisplayForm::FreqDisplayForm(int nplots, QWidget* parent)
  : QWidget(parent)
{
  _systemSpecifiedFlag = false;
  _intValidator = new QIntValidator(this);
  _intValidator->setBottom(0);

  _layout = new QGridLayout(this);
  _freqDisplayPlot = new FrequencyDisplayPlot(this);
  _layout->addWidget(_freqDisplayPlot, 0, 0);

  _nplots = nplots;
  _numRealDataPoints = 1024;

  setLayout(_layout);

  // Set the initial plot size
  resize(QSize(800, 600));

  // Set up a grid that can be turned on/off
  _grid = new QwtPlotGrid();
  _grid->setPen(QPen(QColor(Qt::gray)));

  // Create a set of actions for the menu
  _grid_on_act = new QAction("Grid On", this);
  _grid_on_act->setStatusTip(tr("Toggle Grid on"));
  connect(_grid_on_act, SIGNAL(triggered()), this, SLOT(setGridOn()));

  _grid_off_act = new QAction("Grid Off", this);
  _grid_off_act->setStatusTip(tr("Toggle Grid off"));
  connect(_grid_off_act, SIGNAL(triggered()), this, SLOT(setGridOff()));

  // Create a pop-up menu for manipulating the figure
  _menu = new QMenu(this);
  _menu->addAction(_grid_on_act);
  _menu->addAction(_grid_off_act);

  for(int i = 0; i < _nplots; i++) {
    _line_title_act.push_back(new LineTitleAction(i, this));
    _line_color_menu.push_back(new LineColorMenu(i, this));
    _line_width_menu.push_back(new LineWidthMenu(i, this));

    connect(_line_title_act[i], SIGNAL(whichTrigger(int, const QString&)),
	    this, SLOT(setTitle(int, const QString&)));

    for(int j = 0; j < _line_color_menu[i]->getNumActions(); j++) {
      connect(_line_color_menu[i], SIGNAL(whichTrigger(int, const QString&)),
	      this, SLOT(setColor(int, const QString&)));
    }

    for(int j = 0; j < _line_width_menu[i]->getNumActions(); j++) {
      connect(_line_width_menu[i], SIGNAL(whichTrigger(int, int)),
	      this, SLOT(setLineWidth(int, int)));
    }
    
    _lines_menu.push_back(new QMenu(_freqDisplayPlot->title(i), this));
    _lines_menu[i]->addAction(_line_title_act[i]);
    _lines_menu[i]->addMenu(_line_color_menu[i]);
    _lines_menu[i]->addMenu(_line_width_menu[i]);
    _menu->addMenu(_lines_menu[i]);
  }

  Reset();

  // Create a timer to update plots at the specified rate
  displayTimer = new QTimer(this);
  connect(displayTimer, SIGNAL(timeout()), this, SLOT(updateGuiTimer()));

  connect(_freqDisplayPlot, SIGNAL(plotPointSelected(const QPointF)),
	  this, SLOT(onFFTPlotPointSelected(const QPointF)));
}

FreqDisplayForm::~FreqDisplayForm()
{
  // Qt deletes children when parent is deleted

  // Don't worry about deleting Display Plots - they are deleted when parents are deleted
  delete _intValidator;

  displayTimer->stop();
  delete displayTimer;
}

void
FreqDisplayForm::newData(const FreqUpdateEvent *freqUpdateEvent)
{
  const std::vector<double*> dataPoints = freqUpdateEvent->getPoints();
  const uint64_t numDataPoints = freqUpdateEvent->getNumDataPoints();

  _freqDisplayPlot->PlotNewData(dataPoints[0],
				numDataPoints,
				0, 0, 0, d_update_time);
}

void
FreqDisplayForm::resizeEvent( QResizeEvent *e )
{
  QSize s = size();
  emit _freqDisplayPlot->resizeSlot(&s);
}

void
FreqDisplayForm::customEvent( QEvent * e)
{
  if(e->type() == FreqUpdateEvent::Type()) {
    FreqUpdateEvent* freqUpdateEvent = (FreqUpdateEvent*)e;
    newData(freqUpdateEvent);
  }
}

void
FreqDisplayForm::mousePressEvent( QMouseEvent * e)
{
  if(e->button() == Qt::RightButton) {
    QwtPlotLayout *plt = _freqDisplayPlot->plotLayout();
    QRectF cvs = plt->canvasRect();
    
    QRect plotrect;
    plotrect.setLeft(cvs.x()-plt->spacing()-plt->canvasMargin(0));
    plotrect.setRight(cvs.x()+cvs.width()+plt->spacing()+plt->canvasMargin(0));
    plotrect.setBottom(cvs.y()-plt->spacing()-plt->canvasMargin(0));
    plotrect.setTop(cvs.y()+cvs.width()+plt->spacing()+plt->canvasMargin(0));

    if(!plotrect.contains(e->pos())) {
      // Update the line titles if changed externally
      for(int i = 0; i < _nplots; i++) {
	_lines_menu[i]->setTitle(_freqDisplayPlot->title(i));
      }
      _menu->exec(e->globalPos());
    }
  }
}

void
FreqDisplayForm::updateGuiTimer()
{
  _freqDisplayPlot->canvas()->update();
}

void
FreqDisplayForm::onFFTPlotPointSelected(const QPointF p)
{
  emit plotPointSelected(p, 3);
}

void
FreqDisplayForm::setFrequencyRange(const double newCenterFrequency,
				   const double newStartFrequency,
				   const double newStopFrequency)
{
  double fdiff = std::max(fabs(newStartFrequency), fabs(newStopFrequency));

  if(fdiff > 0) {
    std::string strtime[4] = {"sec", "ms", "us", "ns"};
    double units10 = floor(log10(fdiff));
    double units3  = std::max(floor(units10 / 3.0), 0.0);
    double units = pow(10, (units10-fmod(units10, 3.0)));
    int iunit = static_cast<int>(units3);

    _startFrequency = newStartFrequency;
    _stopFrequency = newStopFrequency;
    double center = (_startFrequency + _stopFrequency)/2.0;

    _freqDisplayPlot->SetFrequencyRange(_startFrequency, _stopFrequency,
					center, false,  units, strtime[iunit]);
  }
}

void
FreqDisplayForm::Reset()
{
}


void
FreqDisplayForm::closeEvent( QCloseEvent *e )
{
  //if(_systemSpecifiedFlag){
  //  _system->SetWindowOpenFlag(false);
  //}

  qApp->processEvents();

  QWidget::closeEvent(e);
}

void
FreqDisplayForm::setUpdateTime(double t)
{
  d_update_time = t;
  displayTimer->start(d_update_time);
}

void
FreqDisplayForm::SetFrequencyRange(const double newCenterFrequency,
				   const double newStartFrequency,
				   const double newStopFrequency)
{
  double fdiff = std::max(fabs(newStartFrequency), fabs(newStopFrequency));

  if(fdiff > 0) {
    std::string strunits[4] = {"Hz", "kHz", "MHz", "GHz"};
    double units10 = floor(log10(fdiff));
    double units3  = std::max(floor(units10 / 3.0), 0.0);
    double units = pow(10, (units10-fmod(units10, 3.0)));
    int iunit = static_cast<int>(units3);

    _startFrequency = newStartFrequency;
    _stopFrequency = newStopFrequency;
    _centerFrequency = newCenterFrequency;

    _freqDisplayPlot->SetFrequencyRange(_startFrequency,
					_stopFrequency,
					_centerFrequency,
					false,
					units, strunits[iunit]);
  }
}

void
FreqDisplayForm::SetFrequencyAxis(double min, double max)
{
  _freqDisplayPlot->set_yaxis(min, max);
}

void
FreqDisplayForm::setTitle(int which, const QString &title)
{
  _freqDisplayPlot->setTitle(which, title);
}

void
FreqDisplayForm::setColor(int which, const QString &color)
{
  _freqDisplayPlot->setColor(which, color);
  _freqDisplayPlot->replot();
}

void
FreqDisplayForm::setLineWidth(int which, int width)
{
  _freqDisplayPlot->setLineWidth(which, width);
  _freqDisplayPlot->replot();
}

void
FreqDisplayForm::setGrid(bool on)
{
  if(on) {
    // will auto-detach if already attached.
    _grid->attach(_freqDisplayPlot);
    
  }
  else {
    _grid->detach();
  }
  _freqDisplayPlot->replot();
}

void
FreqDisplayForm::setGridOn()
{
  setGrid(true);
}

void
FreqDisplayForm::setGridOff()
{
  setGrid(false);
}

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
#include <QColorDialog>
#include <QMessageBox>
#include <timedisplayform.h>
#include <iostream>

TimeDisplayForm::TimeDisplayForm(int nplots, QWidget* parent)
  : QWidget(parent)
{
  _systemSpecifiedFlag = false;
  _intValidator = new QIntValidator(this);
  _intValidator->setBottom(0);

  _layout = new QGridLayout(this);
  _timeDomainDisplayPlot = new TimeDomainDisplayPlot(nplots, this);
  _layout->addWidget(_timeDomainDisplayPlot, 0, 0);

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
    _line_style_menu.push_back(new LineStyleMenu(i, this));
    _line_marker_menu.push_back(new LineMarkerMenu(i, this));

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

    for(int j = 0; j < _line_style_menu[i]->getNumActions(); j++) {
      connect(_line_style_menu[i], SIGNAL(whichTrigger(int, Qt::PenStyle)),
	      this, SLOT(setLineStyle(int, Qt::PenStyle)));
    }

    for(int j = 0; j < _line_marker_menu[i]->getNumActions(); j++) {
      connect(_line_marker_menu[i], SIGNAL(whichTrigger(int, QwtSymbol::Style)),
	      this, SLOT(setLineMarker(int, QwtSymbol::Style)));
    }
    
    _lines_menu.push_back(new QMenu(_timeDomainDisplayPlot->title(i), this));
    _lines_menu[i]->addAction(_line_title_act[i]);
    _lines_menu[i]->addMenu(_line_color_menu[i]);
    _lines_menu[i]->addMenu(_line_width_menu[i]);
    _lines_menu[i]->addMenu(_line_style_menu[i]);
    _lines_menu[i]->addMenu(_line_marker_menu[i]);
    _menu->addMenu(_lines_menu[i]);
  }

  Reset();

  // Create a timer to update plots at the specified rate
  displayTimer = new QTimer(this);
  connect(displayTimer, SIGNAL(timeout()), this, SLOT(updateGuiTimer()));

  connect(_timeDomainDisplayPlot, SIGNAL(plotPointSelected(const QPointF)),
	  this, SLOT(onTimePlotPointSelected(const QPointF)));
}

TimeDisplayForm::~TimeDisplayForm()
{
  // Qt deletes children when parent is deleted

  // Don't worry about deleting Display Plots - they are deleted when parents are deleted
  delete _intValidator;

  displayTimer->stop();
  delete displayTimer;
}

void
TimeDisplayForm::newData( const TimeUpdateEvent* spectrumUpdateEvent)
{
  const std::vector<double*> timeDomainDataPoints = spectrumUpdateEvent->getTimeDomainPoints();
  const uint64_t numTimeDomainDataPoints = spectrumUpdateEvent->getNumTimeDomainDataPoints();

  _timeDomainDisplayPlot->PlotNewData(timeDomainDataPoints,
				      numTimeDomainDataPoints,
				      d_update_time);
}

void
TimeDisplayForm::resizeEvent( QResizeEvent *e )
{
  QSize s = size();
  emit _timeDomainDisplayPlot->resizeSlot(&s);
}

void
TimeDisplayForm::customEvent( QEvent * e)
{
  if(e->type() == TimeUpdateEvent::Type()) {
    TimeUpdateEvent* timeUpdateEvent = (TimeUpdateEvent*)e;
    newData(timeUpdateEvent);
  }
  //else if(e->type() == SpectrumWindowCaptionEventType){
  //setWindowTitle(((SpectrumWindowCaptionEvent*)e)->getLabel());
  //}
  //else if(e->type() == 10009){
  //Reset();
  //if(_systemSpecifiedFlag){
  //  _system->ResetPendingGUIUpdateEvents();
  //}
  //}
}

void
TimeDisplayForm::mousePressEvent( QMouseEvent * e)
{
  if(e->button() == Qt::RightButton) {
    QwtPlotLayout *plt = _timeDomainDisplayPlot->plotLayout();
    QRectF cvs = plt->canvasRect();
    
    QRect plotrect;
    plotrect.setLeft(cvs.x()-plt->spacing()-plt->canvasMargin(0));
    plotrect.setRight(cvs.x()+cvs.width()+plt->spacing()+plt->canvasMargin(0));
    plotrect.setBottom(cvs.y()-plt->spacing()-plt->canvasMargin(0));
    plotrect.setTop(cvs.y()+cvs.width()+plt->spacing()+plt->canvasMargin(0));

    if(!plotrect.contains(e->pos())) {
      // Update the line titles if changed externally
      for(int i = 0; i < _nplots; i++) {
	_lines_menu[i]->setTitle(_timeDomainDisplayPlot->title(i));
      }
      _menu->exec(e->globalPos());
    }
  }
}

void
TimeDisplayForm::updateGuiTimer()
{
  _timeDomainDisplayPlot->canvas()->update();
}

void
TimeDisplayForm::onTimePlotPointSelected(const QPointF p)
{
  emit plotPointSelected(p, 3);
}

void
TimeDisplayForm::setFrequencyRange(const double newCenterFrequency,
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

    _timeDomainDisplayPlot->SetSampleRate(_stopFrequency - _startFrequency,
					  units, strtime[iunit]);
  }
}

void
TimeDisplayForm::Reset()
{
}


void
TimeDisplayForm::closeEvent( QCloseEvent *e )
{
  //if(_systemSpecifiedFlag){
  //  _system->SetWindowOpenFlag(false);
  //}

  qApp->processEvents();

  QWidget::closeEvent(e);
}

void
TimeDisplayForm::setTimeDomainAxis(double min, double max)
{
  _timeDomainDisplayPlot->setYaxis(min, max);
}

void
TimeDisplayForm::setUpdateTime(double t)
{
  d_update_time = t;
  displayTimer->start(d_update_time);
}

void
TimeDisplayForm::setTitle(int which, const QString &title)
{
  _timeDomainDisplayPlot->setTitle(which, title);
}

void
TimeDisplayForm::setColor(int which, const QString &color)
{
  _timeDomainDisplayPlot->setColor(which, color);
  _timeDomainDisplayPlot->replot();
}

void
TimeDisplayForm::setLineWidth(int which, int width)
{
  _timeDomainDisplayPlot->setLineWidth(which, width);
  _timeDomainDisplayPlot->replot();
}

void
TimeDisplayForm::setLineStyle(int which, Qt::PenStyle style)
{
  _timeDomainDisplayPlot->setLineStyle(which, style);
  _timeDomainDisplayPlot->replot();
}

void
TimeDisplayForm::setLineMarker(int which, QwtSymbol::Style marker)
{
  _timeDomainDisplayPlot->setLineMarker(which, marker);
  _timeDomainDisplayPlot->replot();
}

void
TimeDisplayForm::setGrid(bool on)
{
  if(on) {
    // will auto-detach if already attached.
    _grid->attach(_timeDomainDisplayPlot);
    
  }
  else {
    _grid->detach();
  }
  _timeDomainDisplayPlot->replot();
}

void
TimeDisplayForm::setGridOn()
{
  setGrid(true);
}

void
TimeDisplayForm::setGridOff()
{
  setGrid(false);
}

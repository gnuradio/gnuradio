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

  _numRealDataPoints = 1024;

  setLayout(_layout);

  // Set the initial plot size
  resize(QSize(800, 600));

  // Set up a grid that can be turned on/off
  _grid = new QwtPlotGrid();
  _grid->setPen(QPen(QColor(Qt::gray)));


  // Create a set of actions for the menu
  _gridOnAct = new QAction("Grid On", this);
  _gridOnAct->setStatusTip(tr("Toggle Grid on"));
  connect(_gridOnAct, SIGNAL(triggered()), this, SLOT(setGridOn()));

  _gridOffAct = new QAction("Grid Off", this);
  _gridOffAct->setStatusTip(tr("Toggle Grid off"));
  connect(_gridOffAct, SIGNAL(triggered()), this, SLOT(setGridOff()));

  // Create a pop-up menu for manipulating the figure
  _menu = new QMenu(this);
  _menu->addAction(_gridOnAct);
  _menu->addAction(_gridOffAct);

  // Note: These must be done AFTER the menu is created.
  // Set Up the Color Dialog Box
  setupColorDiag();

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
TimeDisplayForm::setTitle(int which, QString title)
{
  _timeDomainDisplayPlot->setTitle(which, title);
}

void
TimeDisplayForm::setColor(int which, QString color)
{
  _timeDomainDisplayPlot->setColor(which, color);
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


// CHANGE COLOR CONTROLS
void
TimeDisplayForm::setupColorDiag()
{
  // Create an action item for the menu
  _colorAct = new QAction("Line Color", this);
  _colorAct->setStatusTip(tr("Set line color"));
  connect(_colorAct, SIGNAL(triggered()), this, SLOT(setLineColorAsk()));

  // Add color box item to menu
  _menu->addAction(_colorAct);

  _color_diag = new QDialog(this);
  _color_diag->setModal(true);

  _color_comboBox = new QComboBox();
  _color_comboBox->addItem(tr("Blue"));
  _color_comboBox->addItem(tr("Red"));
  _color_comboBox->addItem(tr("Green"));
  _color_comboBox->addItem(tr("Black"));
  _color_comboBox->addItem(tr("Cyan"));
  _color_comboBox->addItem(tr("Magenta"));
  _color_comboBox->addItem(tr("Yellow"));
  _color_comboBox->addItem(tr("Gray"));
  _color_comboBox->addItem(tr("Darkred"));
  _color_comboBox->addItem(tr("Darkgreen"));
  _color_comboBox->addItem(tr("Darkblue"));
  _color_comboBox->addItem(tr("Darkgray"));

  QGridLayout *layout = new QGridLayout(_color_diag);
  QPushButton *btn_ok = new QPushButton(tr("OK"));
  QPushButton *btn_cancel = new QPushButton(tr("Cancel"));

  layout->addWidget(_color_comboBox, 0, 0, 1, 3);
  layout->addWidget(btn_ok, 1, 0);
  layout->addWidget(btn_cancel, 1, 1);

  connect(btn_ok, SIGNAL(clicked()), this, SLOT(setLineColorGet()));
  connect(btn_cancel, SIGNAL(clicked()), _color_diag, SLOT(close()));
}

void
TimeDisplayForm::setLineColorAsk()
{
  _color_diag->show();
}

void
TimeDisplayForm::setLineColorGet()
{
  setColor(0, _color_comboBox->currentText());
  _timeDomainDisplayPlot->replot();
  _color_diag->accept();
}


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

#include <displayform.h>
#include <iostream>

DisplayForm::DisplayForm(int nplots, QWidget* parent)
  : QWidget(parent), _nplots(nplots), _systemSpecifiedFlag(false)
{
  // Set the initial plot size
  resize(QSize(800, 600));

  // Set up a grid that can be turned on/off
  _grid = new QwtPlotGrid();
  _grid->setPen(QPen(QColor(Qt::gray)));

  // Create a set of actions for the menu
  _stop_act = new QAction("Stop", this);
  _stop_act->setStatusTip(tr("Start/Stop"));
  connect(_stop_act, SIGNAL(triggered()), this, SLOT(setStop()));
  _stop_state = false;

  _grid_act = new QAction("Grid On", this);
  _grid_act->setStatusTip(tr("Toggle Grid on/off"));
  connect(_grid_act, SIGNAL(triggered()), this, SLOT(setGrid()));
  _grid_state = false;

  // Create a pop-up menu for manipulating the figure
  _menu = new QMenu(this);
  _menu->addAction(_stop_act);
  _menu->addAction(_grid_act);

  for(int i = 0; i < _nplots; i++) {
    _line_title_act.push_back(new LineTitleAction(i, this));
    _line_color_menu.push_back(new LineColorMenu(i, this));
    _line_width_menu.push_back(new LineWidthMenu(i, this));
    _line_style_menu.push_back(new LineStyleMenu(i, this));
    _line_marker_menu.push_back(new LineMarkerMenu(i, this));
    _marker_alpha_menu.push_back(new MarkerAlphaMenu(i, this));

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

    for(int j = 0; j < _marker_alpha_menu[i]->getNumActions(); j++) {
      connect(_marker_alpha_menu[i], SIGNAL(whichTrigger(int, int)),
	      this, SLOT(setMarkerAlpha(int, int)));
    }
    
    _lines_menu.push_back(new QMenu(tr(""), this));
    _lines_menu[i]->addAction(_line_title_act[i]);
    _lines_menu[i]->addMenu(_line_color_menu[i]);
    _lines_menu[i]->addMenu(_line_width_menu[i]);
    _lines_menu[i]->addMenu(_line_style_menu[i]);
    _lines_menu[i]->addMenu(_line_marker_menu[i]);
    _lines_menu[i]->addMenu(_marker_alpha_menu[i]);
    _menu->addMenu(_lines_menu[i]);
  }

  Reset();

  // Create a timer to update plots at the specified rate
  d_displayTimer = new QTimer(this);
  connect(d_displayTimer, SIGNAL(timeout()), this, SLOT(updateGuiTimer()));
}

DisplayForm::~DisplayForm()
{
  // Qt deletes children when parent is deleted

  // Don't worry about deleting Display Plots - they are deleted when parents are deleted

  d_displayTimer->stop();
  delete d_displayTimer;
}

void
DisplayForm::resizeEvent( QResizeEvent *e )
{
  QSize s = size();
  emit _displayPlot->resizeSlot(&s);
}

void
DisplayForm::mousePressEvent( QMouseEvent * e)
{
  if(e->button() == Qt::RightButton) {
    QwtPlotLayout *plt = _displayPlot->plotLayout();
    QRectF cvs = plt->canvasRect();
    
    QRect plotrect;
    plotrect.setLeft(cvs.x()-plt->spacing()-plt->canvasMargin(0));
    plotrect.setRight(cvs.x()+cvs.width()+plt->spacing()+plt->canvasMargin(0));
    plotrect.setBottom(cvs.y()-plt->spacing()-plt->canvasMargin(0));
    plotrect.setTop(cvs.y()+cvs.width()+plt->spacing()+plt->canvasMargin(0));

    if(!plotrect.contains(e->pos())) {
      if(_stop_state == false)
	_stop_act->setText(tr("Stop"));
      else
	_stop_act->setText(tr("Start"));

      if(_grid_state == false)
	_grid_act->setText(tr("Grid On"));
      else
	_grid_act->setText(tr("Grid Off"));

      // Update the line titles if changed externally
      for(int i = 0; i < _nplots; i++) {
	_lines_menu[i]->setTitle(_displayPlot->title(i));
      }
      _menu->exec(e->globalPos());
    }
  }
}

void
DisplayForm::updateGuiTimer()
{
  _displayPlot->canvas()->update();
}

void
DisplayForm::onPlotPointSelected(const QPointF p)
{
  emit plotPointSelected(p, 3);
}

void
DisplayForm::Reset()
{
}


void
DisplayForm::closeEvent( QCloseEvent *e )
{
  qApp->processEvents();
  QWidget::closeEvent(e);
}

void
DisplayForm::setUpdateTime(double t)
{
  d_update_time = t;
  d_displayTimer->start(d_update_time);
}

void
DisplayForm::setTitle(int which, const QString &title)
{
  _displayPlot->setTitle(which, title);
}

void
DisplayForm::setColor(int which, const QString &color)
{
  _displayPlot->setColor(which, color);
  _displayPlot->replot();
}

void
DisplayForm::setLineWidth(int which, int width)
{
  _displayPlot->setLineWidth(which, width);
  _displayPlot->replot();
}

void
DisplayForm::setLineStyle(int which, Qt::PenStyle style)
{
  _displayPlot->setLineStyle(which, style);
  _displayPlot->replot();
}

void
DisplayForm::setLineMarker(int which, QwtSymbol::Style marker)
{
  _displayPlot->setLineMarker(which, marker);
  _displayPlot->replot();
}

void
DisplayForm::setMarkerAlpha(int which, int alpha)
{
  _displayPlot->setMarkerAlpha(which, alpha);
  _displayPlot->replot();
}

void
DisplayForm::setStop(bool on)
{
  if(!on) {
    // will auto-detach if already attached.
    _displayPlot->setStop(false);
    _stop_state = false;
  }
  else {
    _displayPlot->setStop(true);
    _stop_state = true;
  }
  _displayPlot->replot();
}

void
DisplayForm::setStop()
{
  if(_stop_state == false)
    setStop(true);
  else
    setStop(false);
}

void
DisplayForm::setGrid(bool on)
{
  if(on) {
    // will auto-detach if already attached.
    _grid->attach(_displayPlot);
    _grid_state = true;
  }
  else {
    _grid->detach();
    _grid_state = false;
  }
  _displayPlot->replot();
}

void
DisplayForm::setGrid()
{
  if(_grid_state == false)
    setGrid(true);
  else
    setGrid(false);
}

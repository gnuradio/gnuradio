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

#include <gnuradio/qtgui/displayform.h>
#include <iostream>
#include <QPixmap>
#include <QFileDialog>

DisplayForm::DisplayForm(int nplots, QWidget* parent)
  : QWidget(parent), _nplots(nplots), _systemSpecifiedFlag(false)
{
  _isclosed = false;

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

  _grid_act = new QAction("Grid", this);
  _grid_act->setCheckable(true);
  _grid_act->setStatusTip(tr("Toggle Grid on/off"));
  connect(_grid_act, SIGNAL(triggered(bool)),
          this, SLOT(setGrid(bool)));
  _grid_state = false;

  // Create a pop-up menu for manipulating the figure
  _menu_on = true;
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
	    this, SLOT(setLineLabel(int, const QString&)));

    for(int j = 0; j < _line_color_menu[i]->getNumActions(); j++) {
      connect(_line_color_menu[i], SIGNAL(whichTrigger(int, const QString&)),
	      this, SLOT(setLineColor(int, const QString&)));
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

  _samp_rate_act = new PopupMenu("Sample Rate", this);
  _samp_rate_act->setStatusTip(tr("Set Sample Rate"));
  connect(_samp_rate_act, SIGNAL(whichTrigger(QString)),
	  this, SLOT(setSampleRate(QString)));
  _menu->addAction(_samp_rate_act);

  _autoscale_act = new QAction("Auto Scale", this);
  _autoscale_act->setStatusTip(tr("Autoscale Plot"));
  _autoscale_act->setCheckable(true);
  connect(_autoscale_act, SIGNAL(triggered(bool)),
	  this, SLOT(autoScale(bool)));
  _autoscale_state = false;
  _menu->addAction(_autoscale_act);

  _save_act = new QAction("Save", this);
  _save_act->setStatusTip(tr("Save Figure"));
  connect(_save_act, SIGNAL(triggered()), this, SLOT(saveFigure()));
  _menu->addAction(_save_act);

  Reset();

  // Create a timer to update plots at the specified rate
  d_displayTimer = new QTimer(this);
  connect(d_displayTimer, SIGNAL(timeout()), this, SLOT(updateGuiTimer()));
}

DisplayForm::~DisplayForm()
{
  _isclosed = true;

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
  bool ctrloff = Qt::ControlModifier != QApplication::keyboardModifiers();
  if((e->button() == Qt::MidButton) && ctrloff && (_menu_on)) {
    if(_stop_state == false)
      _stop_act->setText(tr("Stop"));
    else
      _stop_act->setText(tr("Start"));

    // Update the line titles if changed externally
    for(int i = 0; i < _nplots; i++) {
      _lines_menu[i]->setTitle(_displayPlot->getLineLabel(i));
    }
    _menu->exec(e->globalPos());
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

bool
DisplayForm::isClosed() const
{
  return _isclosed;
}

void
DisplayForm::enableMenu(bool en)
{
  _menu_on = en;
}

void
DisplayForm::closeEvent(QCloseEvent *e)
{
  _isclosed = true;
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
DisplayForm::setTitle(const QString &title)
{
  _displayPlot->setTitle(title);
}

void
DisplayForm::setLineLabel(int which, const QString &label)
{
  _displayPlot->setLineLabel(which, label);
}

void
DisplayForm::setLineColor(int which, const QString &color)
{
  QColor c = QColor(color);
  _displayPlot->setLineColor(which, c);
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

QString
DisplayForm::title()
{
  return _displayPlot->title().text();
}

QString
DisplayForm::lineLabel(int which)
{
  return _displayPlot->getLineLabel(which);
}

QString
DisplayForm::lineColor(int which)
{
  return _displayPlot->getLineColor(which).name();
}

int
DisplayForm::lineWidth(int which)
{
  return _displayPlot->getLineWidth(which);
}

Qt::PenStyle
DisplayForm::lineStyle(int which)
{
  return _displayPlot->getLineStyle(which);
}

QwtSymbol::Style
DisplayForm::lineMarker(int which)
{
  return _displayPlot->getLineMarker(which);
}

int
DisplayForm::markerAlpha(int which)
{
  return _displayPlot->getMarkerAlpha(which);
}

void
DisplayForm::setSampleRate(const QString &rate)
{
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
DisplayForm::saveFigure()
{
  QPixmap qpix = QPixmap::grabWidget(this);

  QString types = QString(tr("JPEG file (*.jpg);;Portable Network Graphics file (*.png);;Bitmap file (*.bmp);;TIFF file (*.tiff)"));

  QString filename, filetype;
  QFileDialog *filebox = new QFileDialog(0, "Save Image", "./", types);
  filebox->setViewMode(QFileDialog::Detail);
  if(filebox->exec()) {
    filename = filebox->selectedFiles()[0];
    filetype = filebox->selectedNameFilter();
  }
  else {
    return;
  }

  if(filetype.contains(".jpg")) {
    qpix.save(filename, "JPEG");
  }
  else if(filetype.contains(".png")) {
    qpix.save(filename, "PNG");
  }
  else if(filetype.contains(".bmp")) {
    qpix.save(filename, "BMP");
  }
  else if(filetype.contains(".tiff")) {
    qpix.save(filename, "TIFF");
  }
  else {
    qpix.save(filename, "JPEG");
  }

  delete filebox;
}

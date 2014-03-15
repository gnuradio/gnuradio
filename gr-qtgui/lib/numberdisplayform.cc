/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
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
#include <gnuradio/qtgui/numberdisplayform.h>
#include <iostream>
#include <qwt_color_map.h>

NumberDisplayForm::NumberDisplayForm(int nplots, gr::qtgui::graph_t type,
                                     QWidget* parent)
  : QWidget(parent)
{
  d_nplots = nplots;
  d_layout = new QGridLayout(this);
  for(int i = 0; i < d_nplots; i++) {
    d_min.push_back(+1e32);
    d_max.push_back(-1e32);
    d_label.push_back(new QLabel(QString("Data %1").arg(i)));
    d_text_box.push_back(new QLabel(QString("0")));

    d_indicator.push_back(new QwtThermo());
    d_indicator[i]->setScale(-1, 1);
    d_indicator[i]->setOriginMode(QwtThermo::OriginCustom);
    d_indicator[i]->setOrigin(0.0);

    switch(type) {
    case(gr::qtgui::NUM_GRAPH_HORIZ):
      d_indicator[i]->setOrientation(Qt::Horizontal);
      d_layout->addWidget(d_label[i], 2*i, 0);
      d_layout->addWidget(d_text_box[i], 2*i, 1);
      d_layout->addWidget(d_indicator[i], 2*i+1, 1);
      break;
    case(gr::qtgui::NUM_GRAPH_VERT):
      d_indicator[i]->setOrientation(Qt::Vertical);
      d_layout->addWidget(d_label[i], 0, i);
      d_layout->addWidget(d_text_box[i], 1, i);
      d_layout->addWidget(d_indicator[i], 2, i);
      break;
    case(gr::qtgui::NUM_GRAPH_NONE):
    default:
      d_layout->addWidget(d_label[i], 0, i);
      d_layout->addWidget(d_text_box[i], 1, i);
    }

    setColor(i, QColor("black"), QColor("black"));
  }

  d_avg = 0.0;
  d_update_time = 0.1;

  d_menu_on = true;
  d_menu = new QMenu(this);

  // Create a set of actions for the menu
  d_stop_act = new QAction("Stop", this);
  d_stop_act->setStatusTip(tr("Start/Stop"));
  connect(d_stop_act, SIGNAL(triggered()), this, SLOT(setStop()));
  d_stop_state = false;
  d_menu->addAction(d_stop_act);

  // Menu items for each number line
  for(int i = 0; i < d_nplots; i++) {
    d_label_act.push_back(new LineTitleAction(i, this));
    connect(d_label_act[i], SIGNAL(whichTrigger(int, const QString&)),
	    this, SLOT(setLabel(int, const QString&)));

    d_label_menu.push_back(new QMenu(tr(""), this));
    d_label_menu[i]->addAction(d_label_act[i]);

    d_color_menu.push_back(new NumberColorMapMenu(i, this));
    connect(d_color_menu[i], SIGNAL(whichTrigger(int, const QColor&, const QColor&)),
            this, SLOT(setColor(int, const QColor&, const QColor&)));
    d_label_menu[i]->addMenu(d_color_menu[i]);

    d_menu->addMenu(d_label_menu[i]);
  }

  d_avg_menu = new FFTAverageMenu(this);
  d_avg_menu->setTitle("Average");
  d_avg_menu->setHigh(0.001);
  d_avg_menu->setMedium(0.01);
  d_avg_menu->setLow(0.1);
  connect(d_avg_menu, SIGNAL(whichTrigger(float)),
	  this, SLOT(setAverage(const float)));
  d_menu->addMenu(d_avg_menu);

  d_layout_menu = new NumberLayoutMenu(this);
  connect(d_layout_menu, SIGNAL(whichTrigger(gr::qtgui::graph_t)),
          this, SLOT(setGraphType(const gr::qtgui::graph_t)));
  d_menu->addMenu(d_layout_menu);

  d_autoscale_act = new QAction("Auto Scale", this);
  d_autoscale_act->setCheckable(true);
  connect(d_autoscale_act, SIGNAL(triggered(bool)),
          this, SLOT(autoScale(bool)));
  d_autoscale_state = false;
  d_menu->addAction(d_autoscale_act);

  d_update_time_menu = new PopupMenu("Update Time", this);
  connect(d_update_time_menu, SIGNAL(whichTrigger(QString)),
          this, SLOT(setUpdateTime(QString)));
  d_menu->addAction(d_update_time_menu);

  d_save_act = new QAction("Save", this);
  d_save_act->setStatusTip(tr("Save Figure"));
  connect(d_save_act, SIGNAL(triggered()), this, SLOT(saveFigure()));
  d_menu->addAction(d_save_act);

  setLayout(d_layout);
}

NumberDisplayForm::~NumberDisplayForm()
{
  // Qt deletes children when parent is deleted
}

void
NumberDisplayForm::mousePressEvent(QMouseEvent * e)
{
  bool ctrloff = Qt::ControlModifier != QApplication::keyboardModifiers();
  if((e->button() == Qt::MidButton) && ctrloff && (d_menu_on)) {
    if(d_stop_state == false)
      d_stop_act->setText(tr("Stop"));
    else
      d_stop_act->setText(tr("Start"));

    // Update the line titles if changed externally
    for(int i = 0; i < d_nplots; i++) {
      d_label_menu[i]->setTitle(label(i).c_str());
    }
    d_menu->exec(e->globalPos());
  }
}

void
NumberDisplayForm::setStop(bool on)
{
  if(!on) {
    d_stop_state = false;
  }
  else {
    d_stop_state = true;
  }
}

void
NumberDisplayForm::setStop()
{
  if(d_stop_state == false)
    setStop(true);
  else
    setStop(false);
}

void
NumberDisplayForm::saveFigure()
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

void
NumberDisplayForm::newData(const QEvent* updateEvent)
{
  if(!d_stop_state) {
    NumberUpdateEvent *tevent = (NumberUpdateEvent*)updateEvent;
    const std::vector<float> samples = tevent->getSamples();

    for(int i = 0; i < d_nplots; i++) {
      d_text_box[i]->setText(QString("%1").arg(samples[i], 4, ' '));
      d_indicator[i]->setValue(samples[i]);
      d_min[i] = std::min(d_min[i], samples[i]);
      d_max[i] = std::max(d_max[i], samples[i]);

      if(d_autoscale_state) {
        //d_indicator[i]->setRange(d_min[i], d_max[i], false);
        d_indicator[i]->setScale(d_min[i], d_max[i]);
      }
    }

  }
}

void
NumberDisplayForm::customEvent(QEvent * e)
{
  if(e->type() == NumberUpdateEvent::Type()) {
    newData(e);
  }
}

void
NumberDisplayForm::setGraphType(const gr::qtgui::graph_t type)
{
  d_graph_type = type;
  for(int i = 0; i < d_nplots; i++) {
    d_layout->removeWidget(d_indicator[i]);
    d_layout->removeWidget(d_label[i]);
    d_layout->removeWidget(d_text_box[i]);

    switch(d_graph_type) {
    case(gr::qtgui::NUM_GRAPH_HORIZ):
      d_indicator[i]->setOrientation(Qt::Horizontal);
      d_indicator[i]->setVisible(true);
      d_layout->addWidget(d_label[i], 2*i, 0);
      d_layout->addWidget(d_text_box[i], 2*i, 1);
      d_layout->addWidget(d_indicator[i], 2*i+1, 1);
      break;
    case(gr::qtgui::NUM_GRAPH_VERT):
      d_indicator[i]->setOrientation(Qt::Vertical);
      d_indicator[i]->setVisible(true);
      d_layout->addWidget(d_label[i], 0, i);
      d_layout->addWidget(d_text_box[i], 1, i);
      d_layout->addWidget(d_indicator[i], 2, i);
      break;
    case(gr::qtgui::NUM_GRAPH_NONE):
    default:
      d_indicator[i]->setVisible(false);
      d_layout->addWidget(d_label[i], 0, i);
      d_layout->addWidget(d_text_box[i], 1, i);
      break;
    }
  }

}

void
NumberDisplayForm::setColor(int which, const QColor &min, const QColor &max)
{
  QwtLinearColorMap *map = new QwtLinearColorMap();
  map->setColorInterval(min, max);
  d_indicator[which]->setColorMap(map);
}

void
NumberDisplayForm::setColorMin(int which, QString min)
{
  setColor(which, QColor(min), colorMax());
}

void
NumberDisplayForm::setColorMax(int which, QString max)
{
  setColor(which, colorMin(), QColor(max));
}

void
NumberDisplayForm::setLabel(int which, const std::string &label)
{
  d_label[which]->setText(label.c_str());
}

void
NumberDisplayForm::setLabel(int which, QString label)
{
  d_label[which]->setText(label);
}

void
NumberDisplayForm::setAverage(const float avg)
{
  d_avg = avg;
}

void
NumberDisplayForm::setUpdateTime(const float time)
{
  d_update_time = time;
}

void
NumberDisplayForm::setUpdateTime(QString time)
{
  setUpdateTime(time.toFloat());
}

gr::qtgui::graph_t
NumberDisplayForm::graphType() const
{
  return d_graph_type;
}

QColor
NumberDisplayForm::colorMin() const
{
  QwtLinearColorMap *map = static_cast<QwtLinearColorMap*>(d_indicator[0]->colorMap());
  return map->color1();
}

QColor
NumberDisplayForm::colorMax() const
{
  QwtLinearColorMap *map = static_cast<QwtLinearColorMap*>(d_indicator[0]->colorMap());
  return map->color2();
}

std::string
NumberDisplayForm::label(int which) const
{
  return d_label[which]->text().toStdString();
}

float
NumberDisplayForm::average() const
{
  return d_avg;
}

float
NumberDisplayForm::updateTime() const
{
  return d_update_time;
}

void
NumberDisplayForm::autoScale(bool on)
{
  d_autoscale_state = on;

  // Reset the autoscale limits
  for(int i = 0; i < d_nplots; i++) {
    d_min.push_back(+1e32);
    d_max.push_back(-1e32);
  }
}

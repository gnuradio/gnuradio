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
#include <QFileDialog>
#include <gnuradio/qtgui/numberdisplayform.h>
#include <iostream>
#include <qwt_color_map.h>

NumberDisplayForm::NumberDisplayForm(int nplots, gr::qtgui::graph_t type,
                                     QWidget* parent)
  : QWidget(parent)
{
  d_nplots = nplots;
  d_graph_type = type;
  d_title = new QLabel(QString(""));
  d_layout = new QGridLayout(this);
  for(int i = 0; i < d_nplots; i++) {
    d_min.push_back(+1e32);
    d_max.push_back(-1e32);
    d_label.push_back(new QLabel(QString("Data %1").arg(i)));
    d_unit.push_back("");
    d_factor.push_back(1);
    d_text_box.push_back(new QLabel(QString("0")));

    d_indicator.push_back(new QwtThermo());
    d_indicator[i]->setScale(-1, 1);

#if QWT_VERSION < 0x060100
#else
    d_indicator[i]->setOriginMode(QwtThermo::OriginCustom);
    d_indicator[i]->setOrigin(0.0);
#endif /* if QWT_VERSION < 0x060100 */

    switch(type) {
    case(gr::qtgui::NUM_GRAPH_HORIZ):
#if QWT_VERSION < 0x060100
      d_indicator[i]->setOrientation(Qt::Horizontal, QwtThermo::BottomScale);
#else
      d_indicator[i]->setOrientation(Qt::Horizontal);
#endif /* if QWT_VERSION < 0x060100 */
      d_layout->addWidget(d_label[i], 2*i, 0);
      d_layout->addWidget(d_text_box[i], 2*i, 1);
      d_layout->addWidget(d_indicator[i], 2*i+1, 1);
      break;
    case(gr::qtgui::NUM_GRAPH_VERT):
#if QWT_VERSION < 0x060100
      d_indicator[i]->setOrientation(Qt::Vertical, QwtThermo::LeftScale);
#else
      d_indicator[i]->setOrientation(Qt::Vertical);
#endif /* if QWT_VERSION < 0x060100 */
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

    d_factor_act.push_back(new ItemFloatAct(i, "Factor", this));
    connect(d_factor_act[i], SIGNAL(whichTrigger(int, float)),
            this, SLOT(setFactor(int, float)));
    d_label_menu[i]->addAction(d_factor_act[i]);

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
      float f = d_factor[i]*samples[i];
      d_text_box[i]->setText(QString("%1 %2").arg(f, 4, ' ').\
                             arg(QString(d_unit[i].c_str())));
      d_indicator[i]->setValue(f);
      d_min[i] = std::min(d_min[i], f);
      d_max[i] = std::max(d_max[i], f);

      if(d_autoscale_state) {
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
  int off = 0;

  // Remove all widgets from the layout
  QLayoutItem *item;
  while((item = d_layout->takeAt(0)) != NULL) {
    d_layout->removeItem(item);
  }

  // If we have a title, add it at the 0,0 grid point (top left)
  // set off = 1 to offset the rest of the widgets
  if(d_title->text().length() > 0) {
    d_layout->addWidget(d_title, 0, 0);
    off = 1;
  }

  d_graph_type = type;
  for(int i = 0; i < d_nplots; i++) {
    switch(d_graph_type) {
    case(gr::qtgui::NUM_GRAPH_HORIZ):
#if QWT_VERSION < 0x060100
      d_indicator[i]->setOrientation(Qt::Horizontal, QwtThermo::BottomScale);
#else
      d_indicator[i]->setOrientation(Qt::Horizontal);
#endif /* if QWT_VERSION < 0x060100 */
      d_indicator[i]->setVisible(true);
      d_layout->addWidget(d_label[i], 2*i+off, 0);
      d_layout->addWidget(d_text_box[i], 2*i+off, 1);
      d_layout->addWidget(d_indicator[i], 2*i+1+off, 1);
      break;
    case(gr::qtgui::NUM_GRAPH_VERT):
#if QWT_VERSION < 0x060100
      d_indicator[i]->setOrientation(Qt::Vertical, QwtThermo::LeftScale);
#else
      d_indicator[i]->setOrientation(Qt::Vertical);
#endif /* if QWT_VERSION < 0x060100 */
      d_indicator[i]->setVisible(true);
      d_layout->addWidget(d_label[i], 0+off, i);
      d_layout->addWidget(d_text_box[i], 1+off, i);
      d_layout->addWidget(d_indicator[i], 2+off, i);
      break;
    case(gr::qtgui::NUM_GRAPH_NONE):
    default:
      d_indicator[i]->setVisible(false);
      d_layout->addWidget(d_label[i], 0+off, i);
      d_layout->addWidget(d_text_box[i], 1+off, i);
      break;
    }
  }

}

void
NumberDisplayForm::setColor(int which, const QColor &min, const QColor &max)
{
  QwtLinearColorMap *map = new QwtLinearColorMap();
  map->setColorInterval(min, max);

#if QWT_VERSION < 0x060000
  d_indicator[which]->setFillColor(max);
#else
  d_indicator[which]->setColorMap(map);
#endif /* QWT_VERSION < 0x060000 */
}

void
NumberDisplayForm::setColorMin(int which, QString min)
{
  setColor(which, QColor(min), colorMax(which));
}

void
NumberDisplayForm::setColorMax(int which, QString max)
{
  setColor(which, colorMin(which), QColor(max));
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

void
NumberDisplayForm::setScale(int which, int min, int max)
{
  d_min[which] = min;
  d_max[which] = max;
  d_indicator[which]->setScale(min, max);
#if QWT_VERSION < 0x060100
  d_indicator[which]->setRange(min, max);
#endif
}

void
NumberDisplayForm::setScaleMin(int which, int min)
{
  setScale(which, min, d_max[which]);
}

void
NumberDisplayForm::setScaleMax(int which, int max)
{
  setScale(which, d_min[which], max);
}

gr::qtgui::graph_t
NumberDisplayForm::graphType() const
{
  return d_graph_type;
}

QColor
NumberDisplayForm::colorMin(int which) const
{
#if QWT_VERSION < 0x060000
  return d_indicator[which]->fillColor();
#else
  QwtLinearColorMap *map = static_cast<QwtLinearColorMap*>(d_indicator[which]->colorMap());
  return map->color1();
#endif /* QWT_VERSION < 0x060000 */
}

QColor
NumberDisplayForm::colorMax(int which) const
{
#if QWT_VERSION < 0x060000
  return d_indicator[which]->fillColor();
#else
  QwtLinearColorMap *map = static_cast<QwtLinearColorMap*>(d_indicator[which]->colorMap());
  return map->color2();
#endif /* QWT_VERSION < 0x060000 */
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

int
NumberDisplayForm::scaleMin(int which)
{
  return d_min[which];
}

int
NumberDisplayForm::scaleMax(int which)
{
  return d_max[which];
}

void
NumberDisplayForm::autoScale(bool on)
{
  d_autoscale_state = on;
  d_autoscale_act->setChecked(on);

  // Reset the autoscale limits
  for(int i = 0; i < d_nplots; i++) {
    d_min[i] = +1e32;
    d_max[i] = -1e32;
  }
}

std::string
NumberDisplayForm::title() const
{
  return d_title->text().toStdString();
}

void
NumberDisplayForm::setTitle(const std::string &title)
{
  std::string t = title;
  if(t.length() > 0)
    t = "<b><FONT SIZE=4>" + title + "</b>";
  d_title->setText(QString(t.c_str()));
  setGraphType(d_graph_type);
}

std::string
NumberDisplayForm::unit(int which) const
{
  if(static_cast<size_t>(which) >= d_unit.size())
    throw std::runtime_error("NumberDisplayForm::units: invalid 'which'.\n");

  return d_unit[which];
}

void
NumberDisplayForm::setUnit(int which, const std::string &unit)
{
  if(static_cast<size_t>(which) >= d_unit.size())
    throw std::runtime_error("NumberDisplayForm::setUnits: invalid 'which'.\n");

  d_unit[which] = unit;
}

float
NumberDisplayForm::factor(int which) const
{
  if(static_cast<size_t>(which) >= d_factor.size())
    throw std::runtime_error("NumberDisplayForm::factor: invalid 'which'.\n");

  return d_factor[which];
}

void
NumberDisplayForm::setFactor(int which, float factor)
{
  if(static_cast<size_t>(which) >= d_factor.size())
    throw std::runtime_error("NumberDisplayForm::setFactor: invalid 'which'.\n");

  d_factor[which] = factor;
}

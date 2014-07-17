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

#ifndef NUMBER_DISPLAY_FORM_H
#define NUMBER_DISPLAY_FORM_H

#include <gnuradio/qtgui/spectrumUpdateEvents.h>
#include <gnuradio/qtgui/form_menus.h>
#include <QtGui/QtGui>
#include <qwt_thermo.h>
#include <vector>

/*!
 * \brief DisplayForm child for managing number sink plots.
 * \ingroup qtgui_blk
 */
class NumberDisplayForm : public QWidget
{
  Q_OBJECT

  public:
  NumberDisplayForm(int nplots=1,
                    gr::qtgui::graph_t type=gr::qtgui::NUM_GRAPH_HORIZ,
                    QWidget* parent = 0);
  ~NumberDisplayForm();

  gr::qtgui::graph_t graphType() const;
  QColor colorMin(int which) const;
  QColor colorMax(int which) const;
  std::string label(int which) const;
  float average() const;
  float updateTime() const;
  int scaleMin(int which);
  int scaleMax(int which);
  std::string title() const;
  std::string unit(int which) const;
  float factor(int which) const;

public slots:
  void mousePressEvent(QMouseEvent * e);
  void customEvent(QEvent * e);
  void setStop(bool on);
  void setStop();
  void setGraphType(const gr::qtgui::graph_t type);
  void setColor(int which, const QColor &min, const QColor &max);
  void setColorMin(int which, QString min);
  void setColorMax(int which, QString max);
  void setLabel(int which, const std::string &label);
  void setLabel(int which, QString label);
  void setAverage(const float avg);
  void setUpdateTime(const float time);
  void setUpdateTime(QString time);
  void saveFigure();
  void setScale(int which, int min, int max);
  void setScaleMin(int which, int min);
  void setScaleMax(int which, int max);
  void autoScale(bool on);
  void setTitle(const std::string &title);
  void setUnit(int which, const std::string &unit);
  void setFactor(int which, float factor);

private slots:
  void newData(const QEvent*);

private:
  int d_nplots;
  QGridLayout *d_layout;
  std::vector<QLabel*> d_label;
  std::vector<QLabel*> d_text_box;
  std::vector<QwtThermo*> d_indicator;
  gr::qtgui::graph_t d_graph_type;
  float d_avg, d_update_time;
  std::vector<float> d_max, d_min;

  bool d_menu_on;
  bool d_stop_state;
  bool d_autoscale_state;

  QMenu *d_menu;
  QAction *d_stop_act;
  QList<QMenu*> d_label_menu;
  std::vector<LineTitleAction*> d_label_act;
  std::vector<ItemFloatAct*> d_factor_act;
  FFTAverageMenu *d_avg_menu;
  NumberLayoutMenu *d_layout_menu;
  std::vector<NumberColorMapMenu*> d_color_menu;
  PopupMenu *d_maxcolor_menu;
  QAction *d_autoscale_act;
  PopupMenu *d_update_time_menu;
  QAction *d_save_act;

  QLabel *d_title;
  std::vector<std::string> d_unit;
  std::vector<float> d_factor;
};

#endif /* NUMBER_DISPLAY_FORM_H */

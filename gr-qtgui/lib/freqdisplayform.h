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

#ifndef FREQ_DISPLAY_FORM_H
#define FREQ_DISPLAY_FORM_H

#include <spectrumUpdateEvents.h>
#include <FrequencyDisplayPlot.h>
#include <QtGui/QtGui>
#include <vector>

#include <qwt_plot_grid.h>
#include <qwt_plot_layout.h>

#include "form_menus.h"

class FreqDisplayForm : public QWidget
{
  Q_OBJECT

  public:
  FreqDisplayForm(int nplots=1, QWidget* parent = 0);
  ~FreqDisplayForm();

  void Reset();

public slots:
  void resizeEvent(QResizeEvent *e);
  void customEvent(QEvent *e);
  void mousePressEvent(QMouseEvent *e);

  void setFrequencyRange(const double newCenterFrequency,
			 const double newStartFrequency,
			 const double newStopFrequency);
  void closeEvent(QCloseEvent *e);

  void setUpdateTime(double t);

  void SetFrequencyRange(const double newCenterFrequency,
			 const double newStartFrequency,
			 const double newStopFrequency);
  void SetFrequencyAxis(double min, double max);

  void setTitle(int which, const QString &title);
  void setColor(int which, const QString &color);
  void setLineWidth(int which, int width);

  void setGrid(bool on);
  void setGridOn();
  void setGridOff();

private slots:
  void newData(const FreqUpdateEvent *freqUpdateEvent);
  void updateGuiTimer();

  void onFFTPlotPointSelected(const QPointF p);

signals:
  void plotPointSelected(const QPointF p, int type);

private:
  int _nplots;
  uint64_t _numRealDataPoints;
  QIntValidator* _intValidator;

  QGridLayout *_layout;
  FrequencyDisplayPlot* _freqDisplayPlot;
  bool _systemSpecifiedFlag;
  double _startFrequency;
  double _stopFrequency;
  double _centerFrequency;

  QwtPlotGrid *_grid;

  QMenu *_menu;
  QAction *_grid_on_act;
  QAction *_grid_off_act;

  QList<QMenu*> _lines_menu;
  QList<LineTitleAction*> _line_title_act;
  QList<LineColorMenu*> _line_color_menu;
  QList<LineWidthMenu*> _line_width_menu;

  QTimer *displayTimer;
  double d_update_time;
};

#endif /* FREQ_DISPLAY_FORM_H */

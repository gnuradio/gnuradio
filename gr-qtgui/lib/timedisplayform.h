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

#ifndef TIME_DISPLAY_FORM_H
#define TIME_DISPLAY_FORM_H

#include <spectrumUpdateEvents.h>
#include <FrequencyDisplayPlot.h>
#include <WaterfallDisplayPlot.h>
#include <TimeDomainDisplayPlot.h>
#include <ConstellationDisplayPlot.h>
#include <QtGui/QtGui>
#include <vector>

#include <qwt_plot_grid.h>
#include <qwt_plot_layout.h>

#include "form_menus.h"

class TimeDisplayForm : public QWidget
{
  Q_OBJECT

  public:
  TimeDisplayForm(int nplots=1, QWidget* parent = 0);
  ~TimeDisplayForm();

  void Reset();

public slots:
  void resizeEvent( QResizeEvent * e );
  void customEvent( QEvent * e );
  void mousePressEvent( QMouseEvent * e);

  void setFrequencyRange( const double newCenterFrequency,
			  const double newStartFrequency,
			  const double newStopFrequency );
  void closeEvent( QCloseEvent * e );

  void setTimeDomainAxis(double min, double max);

  void setUpdateTime(double t);

  void setTitle(int which, const QString &title);
  void setColor(int which, const QString &color);
  void setLineWidth(int which, int width);
  void setLineStyle(int which, Qt::PenStyle style);
  void setLineMarker(int which, QwtSymbol::Style style);

  void setStop(bool on);
  void setStop();

  void setGrid(bool on);
  void setGrid();

private slots:
  void newData( const TimeUpdateEvent* );
  void updateGuiTimer();

  void onTimePlotPointSelected(const QPointF p);

signals:
  void plotPointSelected(const QPointF p, int type);

private:
  int _nplots;
  uint64_t _numRealDataPoints;
  QIntValidator* _intValidator;

  QGridLayout *_layout;
  TimeDomainDisplayPlot* _timeDomainDisplayPlot;
  bool _systemSpecifiedFlag;
  double _startFrequency;
  double _stopFrequency;

  QwtPlotGrid *_grid;

  QMenu *_menu;

  QAction *_stop_act;
  bool _stop_state;
  QAction *_grid_act;
  bool _grid_state;


  QList<QMenu*> _lines_menu;
  QList<LineTitleAction*> _line_title_act;
  QList<LineColorMenu*> _line_color_menu;
  QList<LineWidthMenu*> _line_width_menu;
  QList<LineStyleMenu*> _line_style_menu;
  QList<LineMarkerMenu*> _line_marker_menu;

  QTimer *displayTimer;
  double d_update_time;
};

#endif /* TIME_DISPLAY_FORM_H */

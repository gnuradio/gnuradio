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

#ifndef DISPLAY_FORM_H
#define DISPLAY_FORM_H

#include <gnuradio/qtgui/spectrumUpdateEvents.h>
#include <gnuradio/qtgui/api.h>
#include <QtGui/QtGui>
#include <vector>

#include <qwt_plot_grid.h>
#include <qwt_plot_layout.h>

#include <gnuradio/qtgui/DisplayPlot.h>
#include <gnuradio/qtgui/form_menus.h>

/*!
 * \brief Base class for setting up and  managing QTGUI plot forms.
 * \ingroup qtgui_blk
 */
class QTGUI_API DisplayForm : public QWidget
{
  Q_OBJECT

  public:
  DisplayForm(int nplots=1, QWidget* parent = 0);
  ~DisplayForm();

  virtual DisplayPlot* getPlot() = 0;
  void Reset();
  bool isClosed() const;

  void enableMenu(bool en=true);

public slots:
  void resizeEvent( QResizeEvent * e );
  void mousePressEvent( QMouseEvent * e);
  virtual void customEvent( QEvent * e ) = 0;

  void closeEvent( QCloseEvent * e );

  void setUpdateTime(double t);

  void setTitle(const QString &title);
  void setLineLabel(int which, const QString &label);
  void setLineColor(int which, const QString &color);
  void setLineWidth(int which, int width);
  void setLineStyle(int which, Qt::PenStyle style);
  void setLineMarker(int which, QwtSymbol::Style style);
  void setMarkerAlpha(int which, int alpha);

  QString title();
  QString lineLabel(int which);
  QString lineColor(int which);
  int lineWidth(int which);
  Qt::PenStyle lineStyle(int which);
  QwtSymbol::Style lineMarker(int which);
  int markerAlpha(int which);

  virtual void setSampleRate(const QString &rate);

  void setStop(bool on);
  void setStop();

  void setGrid(bool on);

  void saveFigure();

private slots:
  virtual void newData(const QEvent*) = 0;
  virtual void autoScale(bool) = 0;
  void updateGuiTimer();

  void onPlotPointSelected(const QPointF p);

signals:
  void plotPointSelected(const QPointF p, int type);

protected:
  bool _isclosed;

  int _nplots;

  QGridLayout *_layout;
  DisplayPlot* _displayPlot;
  bool _systemSpecifiedFlag;

  QwtPlotGrid *_grid;

  bool   _menu_on;
  QMenu *_menu;

  QAction *_stop_act;
  bool _stop_state;
  QAction *_grid_act;
  bool _grid_state;

  QAction *_autoscale_act;
  bool _autoscale_state;

  QList<QMenu*> _lines_menu;
  QList<LineTitleAction*> _line_title_act;
  QList<LineColorMenu*> _line_color_menu;
  QList<LineWidthMenu*> _line_width_menu;
  QList<LineStyleMenu*> _line_style_menu;
  QList<LineMarkerMenu*> _line_marker_menu;
  QList<MarkerAlphaMenu*> _marker_alpha_menu;

  PopupMenu *_samp_rate_act;
  QAction *_save_act;

  QTimer *d_displayTimer;
  double d_update_time;
};

#endif /* DISPLAY_FORM_H */

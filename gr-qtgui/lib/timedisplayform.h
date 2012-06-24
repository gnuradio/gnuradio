/* -*- c++ -*- */
/*
 * Copyright 2011 Free Software Foundation, Inc.
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

class LineColorMenu: public QMenu
{
  Q_OBJECT

public:
  LineColorMenu(int which, QWidget *parent)
    : QMenu("Line Color", parent), d_which(which)
  {
    d_act.push_back(new QAction("Blue", this));
    d_act.push_back(new QAction("Red", this));
    d_act.push_back(new QAction("Green", this));
    d_act.push_back(new QAction("Black", this));
    d_act.push_back(new QAction("Cyan", this));
    d_act.push_back(new QAction("Magenta", this));
    d_act.push_back(new QAction("Yellow", this));
    d_act.push_back(new QAction("Gray", this));
    d_act.push_back(new QAction("Dark Red", this));
    d_act.push_back(new QAction("Dark Green", this));
    d_act.push_back(new QAction("Dark Blue", this));
    d_act.push_back(new QAction("Dark Gray", this));

    connect(d_act[0], SIGNAL(triggered()), this, SLOT(getBlue()));
    connect(d_act[1], SIGNAL(triggered()), this, SLOT(getRed()));
    connect(d_act[2], SIGNAL(triggered()), this, SLOT(getGreen()));
    connect(d_act[3], SIGNAL(triggered()), this, SLOT(getBlack()));
    connect(d_act[4], SIGNAL(triggered()), this, SLOT(getCyan()));
    connect(d_act[5], SIGNAL(triggered()), this, SLOT(getMagenta()));
    connect(d_act[6], SIGNAL(triggered()), this, SLOT(getYellow()));
    connect(d_act[7], SIGNAL(triggered()), this, SLOT(getGray()));
    connect(d_act[8], SIGNAL(triggered()), this, SLOT(getDarkRed()));
    connect(d_act[9], SIGNAL(triggered()), this, SLOT(getDarkGreen()));
    connect(d_act[10], SIGNAL(triggered()), this, SLOT(getDarkBlue()));
    connect(d_act[11], SIGNAL(triggered()), this, SLOT(getDarkGray()));

    QListIterator<QAction*> i(d_act);
    while(i.hasNext()) {
      QAction *a = i.next();
      addAction(a);
    }
  }

  ~LineColorMenu()
  {}

  int getNumActions() const
  {
    return d_act.size();
  }
  
  QAction * getAction(int which)
  {
    if(which < d_act.size())
      return d_act[which];
    else
      throw std::runtime_error("LineColorMenu::getAction: which out of range.\n");
  }

signals:
  void whichTrigger(int which, const QString &name);

public slots:
  void getBlue() { emit whichTrigger(d_which, "blue"); }
  void getRed() { emit whichTrigger(d_which, "red"); }
  void getGreen() { emit whichTrigger(d_which, "green"); }
  void getBlack() { emit whichTrigger(d_which, "black"); }
  void getCyan() { emit whichTrigger(d_which, "cyan"); }
  void getMagenta() { emit whichTrigger(d_which, "magenta"); }
  void getYellow() { emit whichTrigger(d_which, "yellow"); }
  void getGray() { emit whichTrigger(d_which, "gray"); }
  void getDarkRed() { emit whichTrigger(d_which, "darkred"); }
  void getDarkGreen() { emit whichTrigger(d_which, "darkgreen"); }
  void getDarkBlue() { emit whichTrigger(d_which, "darkblue"); }
  void getDarkGray() { emit whichTrigger(d_which, "darkgray"); }

private:
  QList<QAction *> d_act;
  int d_which;
};


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

  void setGrid(bool on);
  void setGridOn();
  void setGridOff();

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
  QAction *_grid_on_act;
  QAction *_grid_off_act;

  QList<QMenu*> _lines_menu;
  QList<LineColorMenu*> _line_color_menu;

  QTimer *displayTimer;
  double d_update_time;
};

#endif /* TIME_DISPLAY_FORM_H */

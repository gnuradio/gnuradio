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
#include <QtGui/QApplication>
#include <QtGui/QGridLayout>
#include <QValidator>
#include <QTimer>
#include <vector>

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
  void setFrequencyRange( const double newCenterFrequency,
			  const double newStartFrequency,
			  const double newStopFrequency );
  void closeEvent( QCloseEvent * e );

  void setTimeDomainAxis(double min, double max);

  void setUpdateTime(double t);

  void setTitle(int which, QString title);
  void setColor(int which, QString color);

private slots:
  void newData( const TimeUpdateEvent* );
  void updateGuiTimer();

  void onTimePlotPointSelected(const QPointF p);

signals:
  void plotPointSelected(const QPointF p, int type);

private:
  uint64_t _numRealDataPoints;
  QIntValidator* _intValidator;

  QGridLayout *_layout;
  TimeDomainDisplayPlot* _timeDomainDisplayPlot;
  bool _systemSpecifiedFlag;
  double _startFrequency;
  double _stopFrequency;
  
  QTimer *displayTimer;
  double d_update_time;
};

#endif /* TIME_DISPLAY_FORM_H */

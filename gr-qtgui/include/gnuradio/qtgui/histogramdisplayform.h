/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
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

#ifndef HISTOGRAM_DISPLAY_FORM_H
#define HISTOGRAM_DISPLAY_FORM_H

#include <gnuradio/qtgui/spectrumUpdateEvents.h>
#include <gnuradio/qtgui/HistogramDisplayPlot.h>
#include <QtGui/QtGui>
#include <vector>

#include <gnuradio/qtgui/displayform.h>

/*!
 * \brief DisplayForm child for managing histogram domain plots.
 * \ingroup qtgui_blk
 */
class HistogramDisplayForm : public DisplayForm
{
  Q_OBJECT

  public:
  HistogramDisplayForm(int nplots=1, QWidget* parent = 0);
  ~HistogramDisplayForm();

  HistogramDisplayPlot* getPlot();

  int getNPoints() const;

public slots:
  void customEvent(QEvent * e);

  void setYaxis(double min, double max);
  void setXaxis(double min, double max);
  void setNPoints(const int);
  void autoScale(bool en);
  void setSemilogx(bool en);
  void setSemilogy(bool en);

  void setNumBins(const int);
  void setAccumulate(bool en);
  bool getAccumulate();
  void autoScaleX();

private slots:
  void newData(const QEvent*);

private:
  QIntValidator* d_int_validator;

  double d_startFrequency;
  double d_stopFrequency;

  int d_npoints;

  bool d_semilogx;
  bool d_semilogy;

  NPointsMenu *d_nptsmenu;
  NPointsMenu *d_nbinsmenu;
  QAction *d_semilogxmenu;
  QAction *d_semilogymenu;

  QAction *d_accum_act;
  QAction *d_autoscalex_act;
  bool d_autoscalex_state;
};

#endif /* HISTOGRAM_DISPLAY_FORM_H */

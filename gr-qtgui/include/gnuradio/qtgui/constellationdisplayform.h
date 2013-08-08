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

#ifndef CONSTELLATION_DISPLAY_FORM_H
#define CONSTELLATION_DISPLAY_FORM_H

#include <gnuradio/qtgui/api.h>
#include <gnuradio/qtgui/spectrumUpdateEvents.h>
#include <gnuradio/qtgui/ConstellationDisplayPlot.h>
#include <QtGui/QtGui>
#include <vector>

#include <gnuradio/qtgui/displayform.h>

/*!
 * \brief DisplayForm child for managing constellaton (I&Q) plots.
 * \ingroup qtgui_blk
 */
class QTGUI_API ConstellationDisplayForm : public DisplayForm
{
  Q_OBJECT

public:
  ConstellationDisplayForm(int nplots=1, QWidget* parent = 0);
  ~ConstellationDisplayForm();

  ConstellationDisplayPlot* getPlot();

  int getNPoints() const;

public slots:
  void customEvent(QEvent * e);
  void setNPoints(const int);

  void setSampleRate(const QString &samprate);
  void setYaxis(double min, double max);
  void setXaxis(double min, double max);
  void autoScale(bool en);

private slots:
  void newData(const QEvent*);

private:
  QIntValidator* _intValidator;
  int d_npoints;
};

#endif /* CONSTELLATION_DISPLAY_FORM_H */

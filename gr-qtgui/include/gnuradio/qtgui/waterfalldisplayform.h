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

#ifndef WATERFALL_DISPLAY_FORM_H
#define WATERFALL_DISPLAY_FORM_H

#include <gnuradio/qtgui/spectrumUpdateEvents.h>
#include <gnuradio/qtgui/WaterfallDisplayPlot.h>
#include <QtGui/QtGui>
#include <vector>
#include <gnuradio/filter/firdes.h>

#include <gnuradio/qtgui/displayform.h>

/*!
 * \brief DisplayForm child for managing waterfall (spectrogram) plots.
 * \ingroup qtgui_blk
 */
class WaterfallDisplayForm : public DisplayForm
{
  Q_OBJECT

  public:
  WaterfallDisplayForm(int nplots=1, QWidget* parent = 0);
  ~WaterfallDisplayForm();

  WaterfallDisplayPlot* getPlot();

  int getFFTSize() const;
  float getFFTAverage() const;
  gr::filter::firdes::win_type getFFTWindowType() const;

  int getColorMap(int which);
  int getAlpha(int which);
  double getMinIntensity(int which);
  double getMaxIntensity(int which);

  void clearData();

public slots:
  void customEvent(QEvent *e);

  void setSampleRate(const QString &samprate);
  void setFFTSize(const int);
  void setFFTAverage(const float);
  void setFFTWindowType(const gr::filter::firdes::win_type);

  void setFrequencyRange(const double centerfreq,
			 const double bandwidth);

  void setIntensityRange(const double minIntensity,
			 const double maxIntensity);

  void setAlpha(int which, int alpha);

  void setColorMap(int which,
		   const int newType,
		   const QColor lowColor=QColor("white"),
		   const QColor highColor=QColor("white"));

  void autoScale(bool en=false);

private slots:
  void newData(const QEvent *updateEvent);

private:
  uint64_t _numRealDataPoints;
  QIntValidator* _intValidator;

  double _samp_rate, _center_freq;
  int _fftsize;
  float _fftavg;
  gr::filter::firdes::win_type _fftwintype;

  double _min_val;
  double _max_val;
};

#endif /* WATERFALL_DISPLAY_FORM_H */

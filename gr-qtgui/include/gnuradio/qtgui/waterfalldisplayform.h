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
  double getTimePerFFT();
  float getFFTAverage() const;
  gr::filter::firdes::win_type getFFTWindowType() const;

  int getColorMap(int which);
  int getAlpha(int which);
  double getMinIntensity(int which);
  double getMaxIntensity(int which);

  void clearData();

  // returns the frequency that was last double-clicked on by the user
  float getClickedFreq() const;

  // checks if there was a double-click event; reset if there was
  bool checkClicked();

public slots:
  void customEvent(QEvent *e);
  void setTimeTitle(const std::string);
  void setSampleRate(const QString &samprate);
  void setFFTSize(const int);
  void setFFTAverage(const float);
  void setFFTWindowType(const gr::filter::firdes::win_type);

  void setFrequencyRange(const double centerfreq,
			 const double bandwidth);

  void setIntensityRange(const double minIntensity,
			 const double maxIntensity);
  void setMaxIntensity(const QString &m);
  void setMinIntensity(const QString &m);

  void setAlpha(int which, int alpha);

  void setColorMap(int which,
		   const int newType,
		   const QColor lowColor=QColor("white"),
		   const QColor highColor=QColor("white"));

  void autoScale(bool en=false);
  void setPlotPosHalf(bool half);
  void setTimePerFFT(double t);
  void setUpdateTime(double t);

private slots:
  void newData(const QEvent *updateEvent);
  void onPlotPointSelected(const QPointF p);

private:
  QIntValidator* d_int_validator;

  double d_samp_rate, d_center_freq;
  int d_fftsize;
  double d_time_per_fft;
  float d_fftavg;
  gr::filter::firdes::win_type d_fftwintype;
  double d_units;

  bool d_clicked;
  double d_clicked_freq;

  double d_min_val, d_cur_min_val;
  double d_max_val, d_cur_max_val;

  FFTSizeMenu *d_sizemenu;
  FFTAverageMenu *d_avgmenu;
  FFTWindowMenu *d_winmenu;
};

#endif /* WATERFALL_DISPLAY_FORM_H */

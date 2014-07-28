/* -*- c++ -*- */
/*
 * Copyright 2008-2011 Free Software Foundation, Inc.
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

#ifndef SPECTRUM_DISPLAY_FORM_H
#define SPECTRUM_DISPLAY_FORM_H

#include <spectrumdisplayform.ui.h>

class SpectrumGUIClass;
#include <gnuradio/qtgui/SpectrumGUIClass.h>

#include <gnuradio/qtgui/SpectrumGUIClass.h>
#include <gnuradio/qtgui/FrequencyDisplayPlot.h>
#include <gnuradio/qtgui/WaterfallDisplayPlot.h>
#include <gnuradio/qtgui/TimeDomainDisplayPlot.h>
#include <gnuradio/qtgui/ConstellationDisplayPlot.h>
#include <QValidator>
#include <QTimer>
#include <vector>

class SpectrumDisplayForm : public QWidget, public Ui::SpectrumDisplayForm
{
  Q_OBJECT

  public:
  SpectrumDisplayForm(QWidget* parent = 0);
  ~SpectrumDisplayForm();

  void setSystem(SpectrumGUIClass * newSystem, const uint64_t numFFTDataPoints,
		 const uint64_t numTimeDomainDataPoints);

  int getAverageCount();
  void setAverageCount(const int newCount);
  void reset();
  void averageDataReset();
  void resizeBuffers(const uint64_t numFFTDataPoints,
		     const uint64_t numTimeDomainDataPoints);

  // returns the frequency that was last double-clicked on by the user
  float getClickedFreq() const;

  // checks if there was a double-click event; reset if there was
  bool checkClicked();

public slots:
  void resizeEvent(QResizeEvent * e);
  void customEvent(QEvent * e);
  void avgLineEdit_valueChanged(int valueString);
  void maxHoldCheckBox_toggled(bool newState);
  void minHoldCheckBox_toggled(bool newState);
  void minHoldResetBtn_clicked();
  void maxHoldResetBtn_clicked();
  void tabChanged(int index);

  void setFrequencyRange(const double newCenterFrequency,
			 const double newStartFrequency,
			 const double newStopFrequency);
  void closeEvent(QCloseEvent * e);
  void windowTypeChanged(int newItem);
  void useRFFrequenciesCB(bool useRFFlag);
  void toggleRFFrequencies(bool en);
  void waterfallMaximumIntensityChangedCB(double);
  void waterfallMinimumIntensityChangedCB(double);
  void waterfallIntensityColorTypeChanged(int);
  void waterfallAutoScaleBtnCB();
  void fftComboBoxSelectedCB(const QString&);

  void toggleTabFrequency(const bool state);
  void toggleTabWaterfall(const bool state);
  void toggleTabTime(const bool state);
  void toggleTabConstellation(const bool state);

  void setTimeDomainAxis(double min, double max);
  void setConstellationAxis(double xmin, double xmax,
			    double ymin, double ymax);
  void setConstellationPenSize(int size);
  void setFrequencyAxis(double min, double max);
  void setUpdateTime(double t);

private slots:
  void newFrequencyData(const SpectrumUpdateEvent*);
  void updateGuiTimer();

  void onFFTPlotPointSelected(const QPointF p);
  void onWFallPlotPointSelected(const QPointF p);
  void onTimePlotPointSelected(const QPointF p);
  void onConstPlotPointSelected(const QPointF p);

signals:
  void plotPointSelected(const QPointF p, int type);

private:
  void _averageHistory(const double * newBuffer);

  int _historyEntryCount;
  int _historyEntry;
  std::vector<double*>* _historyVector;
  double* _averagedValues;
  uint64_t _numRealDataPoints;
  double* _realFFTDataPoints;
  QIntValidator* _intValidator;
  FrequencyDisplayPlot* _frequencyDisplayPlot;
  WaterfallDisplayPlot* _waterfallDisplayPlot;
  TimeDomainDisplayPlot* _timeDomainDisplayPlot;
  ConstellationDisplayPlot* _constellationDisplayPlot;
  SpectrumGUIClass* _system;
  bool _systemSpecifiedFlag;
  double _centerFrequency;
  double _startFrequency;
  double _noiseFloorAmplitude;
  double _peakFrequency;
  double _peakAmplitude;
  double _stopFrequency;

  double d_units;
  bool d_clicked;
  double d_clicked_freq;

  //SpectrumUpdateEvent _lastSpectrumEvent;

  // whether or not to use a particular display
  int d_plot_fft;
  int d_plot_waterfall;
  int d_plot_time;
  int d_plot_constellation;

  QTimer *displayTimer;
  double d_update_time;
};

#endif /* SPECTRUM_DISPLAY_FORM_H */

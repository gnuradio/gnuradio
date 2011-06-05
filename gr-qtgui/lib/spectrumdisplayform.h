/* -*- c++ -*- */
/*
 * Copyright 2008,2009,2010,2011 Free Software Foundation, Inc.
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

#include "spectrumdisplayform.ui.h"

class SpectrumGUIClass;
#include <SpectrumGUIClass.h>

#include <SpectrumGUIClass.h>
#include <FrequencyDisplayPlot.h>
#include <WaterfallDisplayPlot.h>
#include <TimeDomainDisplayPlot.h>
#include <ConstellationDisplayPlot.h>
#include <QValidator>
#include <QTimer>
#include <vector>

class SpectrumDisplayForm : public QWidget, public Ui::SpectrumDisplayForm
{
  Q_OBJECT

  public:
  SpectrumDisplayForm(QWidget* parent = 0);
  ~SpectrumDisplayForm();
  
  void setSystem( SpectrumGUIClass * newSystem, const uint64_t numFFTDataPoints,
		  const uint64_t numTimeDomainDataPoints );

  int GetAverageCount();
  void SetAverageCount( const int newCount );
  void Reset();
  void AverageDataReset();
  void ResizeBuffers( const uint64_t numFFTDataPoints,
		      const uint64_t numTimeDomainDataPoints );
  
public slots:
  void resizeEvent( QResizeEvent * e );
  void customEvent( QEvent * e );
  void AvgLineEdit_valueChanged( int valueString );
  void MaxHoldCheckBox_toggled( bool newState );
  void MinHoldCheckBox_toggled( bool newState );
  void MinHoldResetBtn_clicked();
  void MaxHoldResetBtn_clicked();
  void TabChanged(int index);

  void SetFrequencyRange( const double newCenterFrequency,
			  const double newStartFrequency,
			  const double newStopFrequency );
  void closeEvent( QCloseEvent * e );
  void WindowTypeChanged( int newItem );
  void UseRFFrequenciesCB( bool useRFFlag );
  void waterfallMaximumIntensityChangedCB(double);
  void waterfallMinimumIntensityChangedCB(double);
  void WaterfallIntensityColorTypeChanged(int);
  void WaterfallAutoScaleBtnCB();
  void FFTComboBoxSelectedCB(const QString&);

  void ToggleTabFrequency(const bool state);
  void ToggleTabWaterfall(const bool state);
  void ToggleTabTime(const bool state);
  void ToggleTabConstellation(const bool state);

  void SetTimeDomainAxis(double min, double max);
  void SetConstellationAxis(double xmin, double xmax,
			    double ymin, double ymax);
  void SetConstellationPenSize(int size);
  void SetFrequencyAxis(double min, double max);
  void SetUpdateTime(double t);

private slots:
  void newFrequencyData( const SpectrumUpdateEvent* );
  void UpdateGuiTimer();

  void onFFTPlotPointSelected(const QPointF p);
  void onWFallPlotPointSelected(const QPointF p);
  void onTimePlotPointSelected(const QPointF p);
  void onConstPlotPointSelected(const QPointF p);

signals:
  void plotPointSelected(const QPointF p, int type);

private:
  void _AverageHistory( const double * newBuffer );

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

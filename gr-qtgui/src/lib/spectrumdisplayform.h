#ifndef SPECTRUM_DISPLAY_FORM_H
#define SPECTRUM_DISPLAY_FORM_H

#include "spectrumdisplayform_ui.h"

class SpectrumGUIClass;
#include <SpectrumGUIClass.h>

#include <SpectrumGUIClass.h>
#include <FrequencyDisplayPlot.h>
#include <WaterfallDisplayPlot.h>
#include <Waterfall3DDisplayPlot.h>
#include <TimeDomainDisplayPlot.h>
#include <ConstellationDisplayPlot.h>
#include <QValidator>
#include <vector>

class SpectrumDisplayForm : public QWidget, public Ui::SpectrumDisplayForm
{
  Q_OBJECT

  public:
  SpectrumDisplayForm(bool useOpenGL = true, QWidget* parent = 0);
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
  void AvgLineEdit_textChanged( const QString & valueString );
  void MaxHoldCheckBox_toggled( bool newState );
  void MinHoldCheckBox_toggled( bool newState );
  void MinHoldResetBtn_clicked();
  void MaxHoldResetBtn_clicked();
  void PowerLineEdit_textChanged( const QString& valueString );
  void SetFrequencyRange( const double newStartFrequency,
			  const double newStopFrequency,
			  const double newCenterFrequency );
  void closeEvent( QCloseEvent * e );
  void WindowTypeChanged( int newItem );
  void UseRFFrequenciesCB( bool useRFFlag );
  void waterfallMaximumIntensityChangedCB(double);
  void waterfallMinimumIntensityChangedCB(double);
  void WaterfallIntensityColorTypeChanged(int);
  void WaterfallAutoScaleBtnCB();
  void waterfall3DMaximumIntensityChangedCB(double);
  void waterfall3DMinimumIntensityChangedCB(double);
  void Waterfall3DIntensityColorTypeChanged(int);
  void Waterfall3DAutoScaleBtnCB();
  void FFTComboBoxSelectedCB(const QString&);

  void ToggleTabFrequency(const bool state);
  void ToggleTabWaterfall(const bool state);
  void ToggleTabWaterfall3D(const bool state);
  void ToggleTabTime(const bool state);
  void ToggleTabConstellation(const bool state);

  void SetTimeDomainAxis(double min, double max);
  void SetConstellationAxis(double xmin, double xmax,
			    double ymin, double ymax);
  void SetFrequencyAxis(double min, double max);

private slots:
  void newFrequencyData( const SpectrumUpdateEvent* );

protected:

private:
  void _AverageHistory( const double * newBuffer );

  bool _useOpenGL;
  int _historyEntryCount;
  int _historyEntry;
  std::vector<double*>* _historyVector;
  double* _averagedValues;
  uint64_t _numRealDataPoints;
  double* _realFFTDataPoints;
  QIntValidator* _intValidator;
  FrequencyDisplayPlot* _frequencyDisplayPlot;
  WaterfallDisplayPlot* _waterfallDisplayPlot;
  Waterfall3DDisplayPlot* _waterfall3DDisplayPlot;
  TimeDomainDisplayPlot* _timeDomainDisplayPlot;
  ConstellationDisplayPlot* _constellationDisplayPlot;
  SpectrumGUIClass* _system;
  bool _systemSpecifiedFlag;
  double _centerFrequency;
  double _startFrequency;
  double _noiseFloorAmplitude;
  double _peakFrequency;
  double _peakAmplitude;
  static int _openGLWaterfall3DFlag;
  double _stopFrequency;

  // whether or not to use a particular display
  int d_plot_fft;
  int d_plot_waterfall;
  int d_plot_waterfall3d;
  int d_plot_time;
  int d_plot_constellation;
};

#endif /* SPECTRUM_DISPLAY_FORM_H */

#ifndef FREQUENCY_DISPLAY_PLOT_HPP
#define FREQUENCY_DISPLAY_PLOT_HPP

#include <cstdio>
#include <qwt_plot.h>
#include <qwt_painter.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_engine.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_marker.h>
#include <highResTimeFunctions.h>
#include <qwt_symbol.h>

class FrequencyDisplayPlot:public QwtPlot{
  Q_OBJECT

public:
  FrequencyDisplayPlot(QWidget*);
  virtual ~FrequencyDisplayPlot();

  void SetFrequencyRange(const double, const double, 
			 const double, const bool,
			 const double units=1000.0, 
			 const std::string &strunits = "kHz");
  double GetStartFrequency()const;
  double GetStopFrequency()const;

  void PlotNewData(const double* dataPoints, const int64_t numDataPoints,
		   const double noiseFloorAmplitude, const double peakFrequency,
		   const double peakAmplitude, const double timeInterval);
  
  void ClearMaxData();
  void ClearMinData();
  
  void SetMaxFFTVisible(const bool);
  void SetMinFFTVisible(const bool);
				   
  virtual void replot();

  void set_yaxis(double min, double max);

public slots:
  void resizeSlot( QSize *e );
  void SetLowerIntensityLevel(const double);
  void SetUpperIntensityLevel(const double);

protected:

private:

  void _resetXAxisPoints();
  
  double _startFrequency;
  double _stopFrequency;
  double _maxYAxis;
  double _minYAxis;
  
  QwtPlotCurve* _fft_plot_curve;
  QwtPlotCurve* _min_fft_plot_curve;
  QwtPlotCurve* _max_fft_plot_curve;

  QwtPlotMarker* _lower_intensity_marker;
  QwtPlotMarker* _upper_intensity_marker;

  QwtPlotPanner* _panner;
  QwtPlotZoomer* _zoomer;

  QwtPlotMarker *_markerPeakAmplitude;
  QwtPlotMarker *_markerNoiseFloorAmplitude;
  
  double* _dataPoints;
  double* _xAxisPoints;

  double* _minFFTPoints;
  double* _maxFFTPoints;
  int64_t _numPoints;

  double _peakFrequency;
  double _peakAmplitude;

  double _noiseFloorAmplitude;

  timespec _lastReplot;

  bool _useCenterFrequencyFlag;
};

#endif /* FREQUENCY_DISPLAY_PLOT_HPP */

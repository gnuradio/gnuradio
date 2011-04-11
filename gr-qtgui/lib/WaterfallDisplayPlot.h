#ifndef WATERFALL_DISPLAY_PLOT_HPP
#define WATERFALL_DISPLAY_PLOT_HPP

#include <stdint.h>
#include <cstdio>
#include <qwt_plot.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>

#include <qtgui_util.h>
#include <plot_waterfall.h>

#include <highResTimeFunctions.h>

class WaterfallDisplayPlot:public QwtPlot{
  Q_OBJECT

public:
  WaterfallDisplayPlot(QWidget*);
  virtual ~WaterfallDisplayPlot();

  void Reset();

  void SetFrequencyRange(const double, const double, 
			 const double, const bool,
			 const double units=1000.0, 
			 const std::string &strunits = "kHz");
  double GetStartFrequency()const;
  double GetStopFrequency()const;

  void PlotNewData(const double* dataPoints, const int64_t numDataPoints,
		   const double timePerFFT, const timespec timestamp,
		   const int droppedFrames);

  void SetIntensityRange(const double minIntensity, const double maxIntensity);

  virtual void replot(void);

  int GetIntensityColorMapType()const;
  void SetIntensityColorMapType( const int, const QColor, const QColor );
  const QColor GetUserDefinedLowIntensityColor()const;
  const QColor GetUserDefinedHighIntensityColor()const;

  enum{
    INTENSITY_COLOR_MAP_TYPE_MULTI_COLOR = 0,
    INTENSITY_COLOR_MAP_TYPE_WHITE_HOT = 1,
    INTENSITY_COLOR_MAP_TYPE_BLACK_HOT = 2,
    INTENSITY_COLOR_MAP_TYPE_INCANDESCENT = 3,
    INTENSITY_COLOR_MAP_TYPE_USER_DEFINED = 4
  };

public slots:
  void resizeSlot( QSize *s );
  void OnPickerPointSelected(const QwtDoublePoint & p);
 
signals:
  void UpdatedLowerIntensityLevel(const double);
  void UpdatedUpperIntensityLevel(const double);
  void plotPointSelected(const QPointF p);

protected:

private:
  void _UpdateIntensityRangeDisplay();

  double _startFrequency;
  double _stopFrequency;
  int    _xAxisMultiplier;

  PlotWaterfall *d_spectrogram;

  QwtPlotPanner* _panner;
  QwtPlotZoomer* _zoomer;

  QwtDblClickPlotPicker *_picker;

  WaterfallData* _waterfallData;

  timespec _lastReplot;

  bool _useCenterFrequencyFlag;

  int64_t _numPoints;

  int _intensityColorMapType;
  QColor _userDefinedLowIntensityColor;
  QColor _userDefinedHighIntensityColor;
};

#endif /* WATERFALL_DISPLAY_PLOT_HPP */

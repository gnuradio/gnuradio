#ifndef TIME_DOMAIN_DISPLAY_PLOT_HPP
#define TIME_DOMAIN_DISPLAY_PLOT_HPP

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

class TimeDomainDisplayPlot:public QwtPlot{
  Q_OBJECT

public:
  TimeDomainDisplayPlot(QWidget*);
  virtual ~TimeDomainDisplayPlot();

  void PlotNewData(const double* realDataPoints, const double* imagDataPoints, const int64_t numDataPoints);
    
  void SetImaginaryDataVisible(const bool);
				   
  virtual void replot();

  void set_yaxis(double min, double max);

protected slots:
  void LegendEntryChecked(QwtPlotItem *plotItem, bool on);

protected:

private:
  void _resetXAxisPoints();

  QwtPlotCurve* _real_plot_curve;
  QwtPlotCurve* _imag_plot_curve;

  QwtPlotPanner* _panner;
  QwtPlotZoomer* _zoomer;
  
  double* _realDataPoints;
  double* _imagDataPoints;
  double* _xAxisPoints;

  timespec _lastReplot;

  int64_t _numPoints;

  double _displayIntervalTime;
};

#endif /* TIME_DOMAIN_DISPLAY_PLOT_HPP */

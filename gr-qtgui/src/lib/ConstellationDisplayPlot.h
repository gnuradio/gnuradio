#ifndef CONSTELLATION_DISPLAY_PLOT_HPP
#define CONSTELLATION_DISPLAY_PLOT_HPP

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

class ConstellationDisplayPlot : public QwtPlot
{
  Q_OBJECT

public:
  ConstellationDisplayPlot(QWidget*);
  virtual ~ConstellationDisplayPlot();

  void PlotNewData(const double* realDataPoints, 
		   const double* imagDataPoints, 
		   const int64_t numDataPoints,
		   const double timeInterval);
    
  virtual void replot();

  void set_xaxis(double min, double max);
  void set_yaxis(double min, double max);
  void set_axis(double xmin, double xmax,
		double ymin, double ymax);
  void set_pen_size(int size);

public slots:
  void resizeSlot( QSize *s );

protected slots:
  void LegendEntryChecked(QwtPlotItem *plotItem, bool on);

protected:

private:
  QwtPlotCurve* _plot_curve;

  QwtPlotPanner* _panner;
  QwtPlotZoomer* _zoomer;
  
  double* _realDataPoints;
  double* _imagDataPoints;

  timespec _lastReplot;

  int64_t _numPoints;
  int64_t _penSize;
};

#endif /* CONSTELLATION_DISPLAY_PLOT_HPP */

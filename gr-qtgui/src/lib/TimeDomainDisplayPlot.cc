#ifndef TIME_DOMAIN_DISPLAY_PLOT_C
#define TIME_DOMAIN_DISPLAY_PLOT_C

#include <TimeDomainDisplayPlot.h>

#include <qwt_scale_draw.h>
#include <qwt_legend.h>


class TimeDomainDisplayZoomer: public QwtPlotZoomer
{
public:
  TimeDomainDisplayZoomer(QwtPlotCanvas* canvas):QwtPlotZoomer(canvas)
  {
    setTrackerMode(QwtPicker::AlwaysOn);
  }

  virtual ~TimeDomainDisplayZoomer(){

  }
  
  virtual void updateTrackerText(){
    updateDisplay();
  }

protected:
  virtual QwtText trackerText( const QwtDoublePoint& p ) const 
  {
    QwtText t(QString("Sample %1, %2 V").arg(p.x(), 0, 'f', 0).arg(p.y(), 0, 'f', 4));

    return t;
  }
};

TimeDomainDisplayPlot::TimeDomainDisplayPlot(QWidget* parent):QwtPlot(parent){
  timespec_reset(&_lastReplot);

  resize(parent->width(), parent->height());

  _displayIntervalTime = (1.0/10.0); // 1/10 of a second between updates

  _numPoints = 1024;
  _realDataPoints = new double[_numPoints];
  _imagDataPoints = new double[_numPoints];
  _xAxisPoints = new double[_numPoints];

  // Disable polygon clipping
  QwtPainter::setDeviceClipping(false);
  
  // We don't need the cache here
  canvas()->setPaintAttribute(QwtPlotCanvas::PaintCached, false);
  canvas()->setPaintAttribute(QwtPlotCanvas::PaintPacked, false);

  QPalette palette;
  palette.setColor(canvas()->backgroundRole(), QColor("white"));
  canvas()->setPalette(palette);  

  setAxisScaleEngine(QwtPlot::xBottom, new QwtLinearScaleEngine);
  setAxisScale(QwtPlot::xBottom, 0, _numPoints);
  setAxisTitle(QwtPlot::xBottom, "Sample Number");

  setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);
  setAxisScale(QwtPlot::yLeft, -1.0, 1.0);
  setAxisTitle(QwtPlot::yLeft, "Normalized Voltage");

  // Automatically deleted when parent is deleted
  _real_plot_curve = new QwtPlotCurve("Real Data");
  _real_plot_curve->attach(this);
  _real_plot_curve->setPen(QPen(Qt::blue));
  _real_plot_curve->setRawData(_xAxisPoints, _realDataPoints, _numPoints);

  _imag_plot_curve = new QwtPlotCurve("Imaginary Data");
  _imag_plot_curve->attach(this);
  _imag_plot_curve->setPen(QPen(Qt::magenta));
  _imag_plot_curve->setRawData(_xAxisPoints, _imagDataPoints, _numPoints);
  // _imag_plot_curve->setVisible(false);

  memset(_realDataPoints, 0x0, _numPoints*sizeof(double));
  memset(_imagDataPoints, 0x0, _numPoints*sizeof(double));
  memset(_xAxisPoints, 0x0, _numPoints*sizeof(double));

  _resetXAxisPoints();

  replot();

  _zoomer = new TimeDomainDisplayZoomer(canvas());
#if QT_VERSION < 0x040000
  _zoomer->setMousePattern(QwtEventPattern::MouseSelect2,
			  Qt::RightButton, Qt::ControlModifier);
#else
  _zoomer->setMousePattern(QwtEventPattern::MouseSelect2,
			  Qt::RightButton, Qt::ControlModifier);
#endif
  _zoomer->setMousePattern(QwtEventPattern::MouseSelect3,
			  Qt::RightButton);

  _panner = new QwtPlotPanner(canvas());
  _panner->setAxisEnabled(QwtPlot::yRight, false);
  _panner->setMouseButton(Qt::MidButton);

  // Avoid jumping when labels with more/less digits
  // appear/disappear when scrolling vertically

  const QFontMetrics fm(axisWidget(QwtPlot::yLeft)->font());
  QwtScaleDraw *sd = axisScaleDraw(QwtPlot::yLeft);
  sd->setMinimumExtent( fm.width("100.00") );

  const QColor c(Qt::darkRed);
  _zoomer->setRubberBandPen(c);
  _zoomer->setTrackerPen(c);

  QwtLegend* legendDisplay = new QwtLegend(this);
  legendDisplay->setItemMode(QwtLegend::CheckableItem);
  insertLegend(legendDisplay);

  connect(this, SIGNAL( legendChecked(QwtPlotItem *, bool ) ), this, SLOT( LegendEntryChecked(QwtPlotItem *, bool ) ));
}

TimeDomainDisplayPlot::~TimeDomainDisplayPlot(){
  delete[] _realDataPoints;
  delete[] _imagDataPoints;
  delete[] _xAxisPoints;

  // _fft_plot_curves deleted when parent deleted
  // _zoomer and _panner deleted when parent deleted
}



void TimeDomainDisplayPlot::replot(){

  const timespec startTime = get_highres_clock();
  
  QwtPlot::replot();

  double differenceTime = (diff_timespec(get_highres_clock(), startTime));

  differenceTime *= 99.0;
  // Require at least a 10% duty cycle
  if(differenceTime > (1.0/10.0)){
    _displayIntervalTime = differenceTime;
  }
}

void TimeDomainDisplayPlot::PlotNewData(const double* realDataPoints, const double* imagDataPoints, const int64_t numDataPoints){
  if(numDataPoints > 0){

    if(numDataPoints != _numPoints){
      _numPoints = numDataPoints;

      delete[] _realDataPoints;
      delete[] _imagDataPoints;
      delete[] _xAxisPoints;
      _realDataPoints = new double[_numPoints];
      _imagDataPoints = new double[_numPoints];
      _xAxisPoints = new double[_numPoints];
      
      _real_plot_curve->setRawData(_xAxisPoints, _realDataPoints, _numPoints);
      _imag_plot_curve->setRawData(_xAxisPoints, _imagDataPoints, _numPoints);

      _resetXAxisPoints();
    }
    memcpy(_realDataPoints, realDataPoints, numDataPoints*sizeof(double));
    memcpy(_imagDataPoints, imagDataPoints, numDataPoints*sizeof(double));

  }

  // Allow at least a 50% duty cycle
  if(diff_timespec(get_highres_clock(), _lastReplot) > _displayIntervalTime){
    // Only replot the screen if it is visible
    if(isVisible()){
      replot();
    }
    _lastReplot = get_highres_clock();
  }
}

void TimeDomainDisplayPlot::SetImaginaryDataVisible(const bool visibleFlag){
  _imag_plot_curve->setVisible(visibleFlag);
}

void TimeDomainDisplayPlot::_resetXAxisPoints(){
  for(long loc = 0; loc < _numPoints; loc++){
    _xAxisPoints[loc] = loc;
  }
  setAxisScale(QwtPlot::xBottom, 0, _numPoints);
}

void TimeDomainDisplayPlot::LegendEntryChecked(QwtPlotItem* plotItem, bool on){
  plotItem->setVisible(!on);
}

#endif /* TIME_DOMAIN_DISPLAY_PLOT_C */

#ifndef TIME_DOMAIN_DISPLAY_PLOT_C
#define TIME_DOMAIN_DISPLAY_PLOT_C

#include <TimeDomainDisplayPlot.h>

#include <qwt_scale_draw.h>
#include <qwt_legend.h>


class TimePrecisionClass
{
public:
  TimePrecisionClass(const int timePrecision)
  {
    _timePrecision = timePrecision;
  }

  virtual ~TimePrecisionClass()
  {
  }

  virtual unsigned int GetTimePrecision() const
  {
    return _timePrecision;
  }

  virtual void SetTimePrecision(const unsigned int newPrecision)
  {
    _timePrecision = newPrecision;
  }
protected:
  unsigned int _timePrecision;
};


class TimeDomainDisplayZoomer: public QwtPlotZoomer, public TimePrecisionClass
{
public:
  TimeDomainDisplayZoomer(QwtPlotCanvas* canvas, const unsigned int timePrecision)
    : QwtPlotZoomer(canvas),TimePrecisionClass(timePrecision)
  {
    setTrackerMode(QwtPicker::AlwaysOn);
  }

  virtual ~TimeDomainDisplayZoomer(){

  }
  
  virtual void updateTrackerText(){
    updateDisplay();
  }

  void SetUnitType(const std::string &type)
  {
    _unitType = type;
  }

protected:
  using QwtPlotZoomer::trackerText;
  virtual QwtText trackerText( const QwtDoublePoint& p ) const 
  {
    QwtText t(QString("%1 %2, %3 V").arg(p.x(), 0, 'f', GetTimePrecision()).
	      arg(_unitType.c_str()).
	      arg(p.y(), 0, 'f', 4));

    return t;
  }

private:
  std::string _unitType;
};

TimeDomainDisplayPlot::TimeDomainDisplayPlot(QWidget* parent):QwtPlot(parent)
{
  timespec_reset(&_lastReplot);

  resize(parent->width(), parent->height());

  _numPoints = 1024;
  _realDataPoints = new double[_numPoints];
  _imagDataPoints = new double[_numPoints];
  _xAxisPoints = new double[_numPoints];

  _zoomer = new TimeDomainDisplayZoomer(canvas(), 0);

  // Disable polygon clipping
  QwtPainter::setDeviceClipping(false);
  
  // We don't need the cache here
  canvas()->setPaintAttribute(QwtPlotCanvas::PaintCached, false);
  canvas()->setPaintAttribute(QwtPlotCanvas::PaintPacked, false);

  QPalette palette;
  palette.setColor(canvas()->backgroundRole(), QColor("white"));
  canvas()->setPalette(palette);  

  setAxisScaleEngine(QwtPlot::xBottom, new QwtLinearScaleEngine);
  set_xaxis(0, _numPoints);
  setAxisTitle(QwtPlot::xBottom, "Time (sec)");

  setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);
  set_yaxis(-2.0, 2.0);
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

  _sampleRate = 1;
  _resetXAxisPoints();

  replot();

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

  connect(this, SIGNAL( legendChecked(QwtPlotItem *, bool ) ), 
	  this, SLOT( LegendEntryChecked(QwtPlotItem *, bool ) ));
}

TimeDomainDisplayPlot::~TimeDomainDisplayPlot(){
  delete[] _realDataPoints;
  delete[] _imagDataPoints;
  delete[] _xAxisPoints;

  // _fft_plot_curves deleted when parent deleted
  // _zoomer and _panner deleted when parent deleted
}

void
TimeDomainDisplayPlot::set_yaxis(double min, double max)
{
  setAxisScale(QwtPlot::yLeft, min, max);
  _zoomer->setZoomBase();
}

void
TimeDomainDisplayPlot::set_xaxis(double min, double max)
{
  setAxisScale(QwtPlot::xBottom, min, max);
  _zoomer->setZoomBase();
}


void TimeDomainDisplayPlot::replot()
{
  QwtPlot::replot();
}

void
TimeDomainDisplayPlot::resizeSlot( QSize *s )
{
  resize(s->width(), s->height());
}

void TimeDomainDisplayPlot::PlotNewData(const double* realDataPoints,
					const double* imagDataPoints,
					const int64_t numDataPoints,
					const double timeInterval)
{
  if((numDataPoints > 0) && 
     (diff_timespec(get_highres_clock(), _lastReplot) > timeInterval)) {
  
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

      set_xaxis(0, numDataPoints);

      _resetXAxisPoints();
    }

    memcpy(_realDataPoints, realDataPoints, numDataPoints*sizeof(double));
    memcpy(_imagDataPoints, imagDataPoints, numDataPoints*sizeof(double));

    replot();

    _lastReplot = get_highres_clock();
  }
}

void TimeDomainDisplayPlot::SetImaginaryDataVisible(const bool visibleFlag)
{
  _imag_plot_curve->setVisible(visibleFlag);
}

void TimeDomainDisplayPlot::_resetXAxisPoints()
{
  double delt = 1.0/_sampleRate;
  for(long loc = 0; loc < _numPoints; loc++){
    _xAxisPoints[loc] = loc*delt;
  }
  setAxisScale(QwtPlot::xBottom, 0, _numPoints*delt);

  // Set up zoomer base for maximum unzoom x-axis
  // and reset to maximum unzoom level
  QwtDoubleRect zbase = _zoomer->zoomBase();
  zbase.setLeft(0);
  zbase.setRight(_numPoints*delt);
  _zoomer->zoom(zbase);
  _zoomer->setZoomBase(zbase);
  _zoomer->zoom(0);
}

void TimeDomainDisplayPlot::LegendEntryChecked(QwtPlotItem* plotItem, bool on)
{
  plotItem->setVisible(!on);
}

void
TimeDomainDisplayPlot::SetSampleRate(double sr, double units,
				     const std::string &strunits)
{
  double newsr = sr/units;
  if(newsr != _sampleRate) {
    _sampleRate = sr/units;
    _resetXAxisPoints();
    
    // While we could change the displayed sigfigs based on the unit being
    // displayed, I think it looks better by just setting it to 4 regardless.
    //double display_units = ceil(log10(units)/2.0);
    double display_units = 4;
    setAxisTitle(QwtPlot::xBottom, QString("Time (%1)").arg(strunits.c_str()));
    ((TimeDomainDisplayZoomer*)_zoomer)->SetTimePrecision(display_units);
    ((TimeDomainDisplayZoomer*)_zoomer)->SetUnitType(strunits);
  }
}

#endif /* TIME_DOMAIN_DISPLAY_PLOT_C */

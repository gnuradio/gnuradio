#ifndef CONSTELLATION_DISPLAY_PLOT_C
#define CONSTELLATION_DISPLAY_PLOT_C

#include <ConstellationDisplayPlot.h>

#include <qwt_scale_draw.h>
#include <qwt_legend.h>


class ConstellationDisplayZoomer: public QwtPlotZoomer
{
public:
  ConstellationDisplayZoomer(QwtPlotCanvas* canvas):QwtPlotZoomer(canvas)
  {
    setTrackerMode(QwtPicker::AlwaysOn);
  }

  virtual ~ConstellationDisplayZoomer(){

  }
  
  virtual void updateTrackerText(){
    updateDisplay();
  }

protected:
  using QwtPlotZoomer::trackerText;
  virtual QwtText trackerText( const QwtDoublePoint& p ) const 
  {
    QwtText t(QString("(%1, %2)").arg(p.x(), 0, 'f', 4).
	      arg(p.y(), 0, 'f', 4));
    return t;
  }
};

ConstellationDisplayPlot::ConstellationDisplayPlot(QWidget* parent)
  : QwtPlot(parent)
{
  timespec_reset(&_lastReplot);

  resize(parent->width(), parent->height());

  _numPoints = 1024;
  _penSize = 5;
  _realDataPoints = new double[_numPoints];
  _imagDataPoints = new double[_numPoints];

  // Disable polygon clipping
  QwtPainter::setDeviceClipping(false);
  
  // We don't need the cache here
  canvas()->setPaintAttribute(QwtPlotCanvas::PaintCached, false);
  canvas()->setPaintAttribute(QwtPlotCanvas::PaintPacked, false);

  QPalette palette;
  palette.setColor(canvas()->backgroundRole(), QColor("white"));
  canvas()->setPalette(palette);  

  setAxisScaleEngine(QwtPlot::xBottom, new QwtLinearScaleEngine);
  set_xaxis(-2.0, 2.0);
  setAxisTitle(QwtPlot::xBottom, "In-phase");

  setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);
  set_yaxis(-2.0, 2.0);
  setAxisTitle(QwtPlot::yLeft, "Quadrature");

  // Automatically deleted when parent is deleted
  _plot_curve = new QwtPlotCurve("Constellation Points");
  _plot_curve->attach(this);
  _plot_curve->setPen(QPen(Qt::blue, _penSize, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
  _plot_curve->setStyle(QwtPlotCurve::Dots);
  _plot_curve->setRawData(_realDataPoints, _imagDataPoints, _numPoints);

  memset(_realDataPoints, 0x0, _numPoints*sizeof(double));
  memset(_imagDataPoints, 0x0, _numPoints*sizeof(double));

  replot();

  _zoomer = new ConstellationDisplayZoomer(canvas());
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

  connect(this, SIGNAL( legendChecked(QwtPlotItem *, bool ) ), 
	  this, SLOT( LegendEntryChecked(QwtPlotItem *, bool ) ));
}

ConstellationDisplayPlot::~ConstellationDisplayPlot()
{
  delete[] _realDataPoints;
  delete[] _imagDataPoints;

  // _fft_plot_curves deleted when parent deleted
  // _zoomer and _panner deleted when parent deleted
}

void 
ConstellationDisplayPlot::set_pen_size(int size)
{
  if(size > 0 && size < 30){
    _penSize = size;
    _plot_curve->setPen(QPen(Qt::blue, _penSize, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
  }
}


void
ConstellationDisplayPlot::set_xaxis(double min, double max)
{
  setAxisScale(QwtPlot::xBottom, min, max);
}

void
ConstellationDisplayPlot::set_yaxis(double min, double max)
{
  setAxisScale(QwtPlot::yLeft, min, max);
}

void
ConstellationDisplayPlot::set_axis(double xmin, double xmax,
				   double ymin, double ymax)
{
  set_xaxis(xmin, xmax);
  set_yaxis(ymin, ymax);
}

void ConstellationDisplayPlot::replot()
{
  QwtPlot::replot();
}

void
ConstellationDisplayPlot::resizeSlot( QSize *s )
{
  resize(s->width(), s->height());
}

void ConstellationDisplayPlot::PlotNewData(const double* realDataPoints,
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
      _realDataPoints = new double[_numPoints];
      _imagDataPoints = new double[_numPoints];
      
      _plot_curve->setRawData(_realDataPoints, _imagDataPoints, _numPoints);
    }

    memcpy(_realDataPoints, realDataPoints, numDataPoints*sizeof(double));
    memcpy(_imagDataPoints, imagDataPoints, numDataPoints*sizeof(double));

    replot();
    
    _lastReplot = get_highres_clock();
  }
}

void
ConstellationDisplayPlot::LegendEntryChecked(QwtPlotItem* plotItem, bool on)
{
  plotItem->setVisible(!on);
}

#endif /* CONSTELLATION_DISPLAY_PLOT_C */

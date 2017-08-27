/* -*- c++ -*- */
/*
 * Copyright 2008-2012 Free Software Foundation, Inc.
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

#ifndef TIME_DOMAIN_DISPLAY_PLOT_C
#define TIME_DOMAIN_DISPLAY_PLOT_C

#include <gnuradio/qtgui/TimeDomainDisplayPlot.h>

#include <qwt_scale_draw.h>
#include <qwt_legend.h>
#include <QColor>
#include <cmath>
#include <iostream>
#include <volk/volk.h>

class TimePrecisionClass
{
public:
  TimePrecisionClass(const int timePrecision)
  {
    d_timePrecision = timePrecision;
  }

  virtual ~TimePrecisionClass()
  {
  }

  virtual unsigned int getTimePrecision() const
  {
    return d_timePrecision;
  }

  virtual void setTimePrecision(const unsigned int newPrecision)
  {
    d_timePrecision = newPrecision;
  }
protected:
  unsigned int d_timePrecision;
};


class TimeDomainDisplayZoomer: public QwtPlotZoomer, public TimePrecisionClass
{
public:
#if QWT_VERSION < 0x060100
  TimeDomainDisplayZoomer(QwtPlotCanvas* canvas, const unsigned int timePrecision)
#else /* QWT_VERSION < 0x060100 */
  TimeDomainDisplayZoomer(QWidget* canvas, const unsigned int timePrecision)
#endif /* QWT_VERSION < 0x060100 */
  : QwtPlotZoomer(canvas),TimePrecisionClass(timePrecision),d_yUnitType("V")
  {
    setTrackerMode(QwtPicker::AlwaysOn);
  }

  virtual ~TimeDomainDisplayZoomer()
  {
  }

  virtual void updateTrackerText()
  {
    updateDisplay();
  }

  void setUnitType(const std::string &type)
  {
    d_unitType = type;
  }

  std::string unitType()
  {
    return d_unitType;
  }

  void setYUnitType(const std::string &type)
  {
    d_yUnitType = type;
  }

protected:
  using QwtPlotZoomer::trackerText;
  virtual QwtText trackerText( const QPoint& p ) const
  {
    QwtText t;
    QwtDoublePoint dp = QwtPlotZoomer::invTransform(p);
    if((fabs(dp.y()) > 0.0001) && (fabs(dp.y()) < 10000)) {
      t.setText(QString("%1 %2, %3 %4").
		arg(dp.x(), 0, 'f', getTimePrecision()).
		arg(d_unitType.c_str()).
		arg(dp.y(), 0, 'f', 4).
		arg(d_yUnitType.c_str()));
    }
    else {
      t.setText(QString("%1 %2, %3 %4").
		arg(dp.x(), 0, 'f', getTimePrecision()).
		arg(d_unitType.c_str()).
		arg(dp.y(), 0, 'e', 4).
		arg(d_yUnitType.c_str()));
    }

    return t;
  }

private:
  std::string d_unitType;
  std::string d_yUnitType;
};


/***********************************************************************
 * Main Time domain plotter widget
 **********************************************************************/
TimeDomainDisplayPlot::TimeDomainDisplayPlot(int nplots, QWidget* parent)
  : DisplayPlot(nplots, parent)
{
  d_numPoints = 1024;
  d_xdata = new double[d_numPoints];
  memset(d_xdata, 0x0, d_numPoints*sizeof(double));

  d_tag_text_color = Qt::black;
  d_tag_background_color = Qt::white;
  d_tag_background_style = Qt::NoBrush;

  d_zoomer = new TimeDomainDisplayZoomer(canvas(), 0);

#if QWT_VERSION < 0x060000
  d_zoomer->setSelectionFlags(QwtPicker::RectSelection | QwtPicker::DragSelection);
#endif

  d_zoomer->setMousePattern(QwtEventPattern::MouseSelect2,
			   Qt::RightButton, Qt::ControlModifier);
  d_zoomer->setMousePattern(QwtEventPattern::MouseSelect3,
			   Qt::RightButton);

  const QColor c(Qt::darkRed);
  d_zoomer->setRubberBandPen(c);
  d_zoomer->setTrackerPen(c);

  d_semilogx = false;
  d_semilogy = false;
  d_autoscale_shot = false;

  setAxisScaleEngine(QwtPlot::xBottom, new QwtLinearScaleEngine);
  setXaxis(0, d_numPoints);
  setAxisTitle(QwtPlot::xBottom, "Time (sec)");

  setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);
  setYaxis(-2.0, 2.0);
  setAxisTitle(QwtPlot::yLeft, "Amplitude");

  QList<QColor> colors;
  colors << QColor(Qt::blue) << QColor(Qt::red) << QColor(Qt::green)
	 << QColor(Qt::black) << QColor(Qt::cyan) << QColor(Qt::magenta)
	 << QColor(Qt::yellow) << QColor(Qt::gray) << QColor(Qt::darkRed)
	 << QColor(Qt::darkGreen) << QColor(Qt::darkBlue) << QColor(Qt::darkGray)
	 // cycle through all colors again to increase time_sink_f input limit
	 // from 12 to 24, otherwise you get a segfault
	 << QColor(Qt::blue) << QColor(Qt::red) << QColor(Qt::green)
	 << QColor(Qt::black) << QColor(Qt::cyan) << QColor(Qt::magenta)
	 << QColor(Qt::yellow) << QColor(Qt::gray) << QColor(Qt::darkRed)
	 << QColor(Qt::darkGreen) << QColor(Qt::darkBlue) << QColor(Qt::darkGray);
	 
  // Setup dataPoints and plot vectors
  // Automatically deleted when parent is deleted
  for(int i = 0; i < d_nplots; i++) {
    d_ydata.push_back(new double[d_numPoints]);
    memset(d_ydata[i], 0x0, d_numPoints*sizeof(double));

    d_plot_curve.push_back(new QwtPlotCurve(QString("Data %1").arg(i)));
    d_plot_curve[i]->attach(this);
    d_plot_curve[i]->setPen(QPen(colors[i]));
    d_plot_curve[i]->setRenderHint(QwtPlotItem::RenderAntialiased);

    QwtSymbol *symbol = new QwtSymbol(QwtSymbol::NoSymbol, QBrush(colors[i]),
				      QPen(colors[i]), QSize(7,7));

#if QWT_VERSION < 0x060000
    d_plot_curve[i]->setRawData(d_xdata, d_ydata[i], d_numPoints);
    d_plot_curve[i]->setSymbol(*symbol);
#else
    d_plot_curve[i]->setRawSamples(d_xdata, d_ydata[i], d_numPoints);
    d_plot_curve[i]->setSymbol(symbol);
#endif
  }

  d_sample_rate = 1;
  _resetXAxisPoints();

  d_tag_markers.resize(d_nplots);
  d_tag_markers_en = std::vector<bool>(d_nplots, true);

  d_trigger_lines[0] = new QwtPlotMarker();
  d_trigger_lines[0]->setLineStyle(QwtPlotMarker::HLine);
  d_trigger_lines[0]->setLinePen(QPen(Qt::red, 0.6, Qt::DashLine));
  d_trigger_lines[0]->setRenderHint(QwtPlotItem::RenderAntialiased);
  d_trigger_lines[0]->setXValue(0.0);
  d_trigger_lines[0]->setYValue(0.0);

  d_trigger_lines[1] = new QwtPlotMarker();
  d_trigger_lines[1]->setLineStyle(QwtPlotMarker::VLine);
  d_trigger_lines[1]->setLinePen(QPen(Qt::red, 0.6, Qt::DashLine));
  d_trigger_lines[1]->setRenderHint(QwtPlotItem::RenderAntialiased);
  d_trigger_lines[1]->setXValue(0.0);
  d_trigger_lines[1]->setYValue(0.0);
}


TimeDomainDisplayPlot::~TimeDomainDisplayPlot()
{
  for(int i = 0; i < d_nplots; i++)
    delete [] d_ydata[i];
  delete[] d_xdata;

  // d_zoomer and _panner deleted when parent deleted
}

void
TimeDomainDisplayPlot::replot()
{
  QwtPlot::replot();
}

void
TimeDomainDisplayPlot::plotNewData(const std::vector<double*> dataPoints,
				   const int64_t numDataPoints,
				   const double timeInterval,
                                   const std::vector< std::vector<gr::tag_t> > &tags)
{
  if(!d_stop) {
    if((numDataPoints > 0)) {
      if(numDataPoints != d_numPoints){
	d_numPoints = numDataPoints;

	delete[] d_xdata;
	d_xdata = new double[d_numPoints];

	for(int i = 0; i < d_nplots; i++) {
	  delete[] d_ydata[i];
	  d_ydata[i] = new double[d_numPoints];

#if QWT_VERSION < 0x060000
	  d_plot_curve[i]->setRawData(d_xdata, d_ydata[i], d_numPoints);
#else
	  d_plot_curve[i]->setRawSamples(d_xdata, d_ydata[i], d_numPoints);
#endif
	}

	_resetXAxisPoints();
      }

      for(int i = 0; i < d_nplots; i++) {
	if(d_semilogy) {
	  for(int n = 0; n < numDataPoints; n++)
	    d_ydata[i][n] = fabs(dataPoints[i][n]);
	}
	else {
	  memcpy(d_ydata[i], dataPoints[i], numDataPoints*sizeof(double));
	}
      }

      // Detach and delete any tags that were plotted last time
      for(int n = 0; n < d_nplots; n++) {
        for(size_t i = 0; i < d_tag_markers[n].size(); i++) {
          d_tag_markers[n][i]->detach();
          delete d_tag_markers[n][i];
        }
        d_tag_markers[n].clear();
      }

      // Plot and attach any new tags found.
      // First test if this was a complex input where real/imag get
      // split here into two stream.
      if(tags.size() > 0) {
        bool cmplx = false;
        int mult = (int)d_nplots / (int)tags.size();
        if(mult == 2)
          cmplx = true;

        std::vector< std::vector<gr::tag_t> >::const_iterator tag = tags.begin();
        for(int i = 0; i < d_nplots; i+=mult) {
          std::vector<gr::tag_t>::const_iterator t;
          for(t = tag->begin(); t != tag->end(); t++) {
            uint64_t offset = (*t).offset;

            // Ignore tag if its offset is outside our plottable vector.
            if(offset >= (uint64_t)d_numPoints) {
              continue;
            }

            double sample_offset = double(offset)/d_sample_rate;

            std::stringstream s;
            s << (*t).key << ": " << (*t).value;

            // Select the right input stream to put the tag on. If real,
            // just use i; if it's a complex stream, find the max of the
            // real and imaginary parts and put the tag on that one.
            int which = i;
            if(cmplx) {
              bool show0 = d_plot_curve[i]->isVisible();
              bool show1 = d_plot_curve[i+1]->isVisible();

              // If we are showing both streams, select the inptu stream
              // with the larger value
              if(show0 && show1) {
                if(fabs(d_ydata[i][offset]) < fabs(d_ydata[i+1][offset]))
                  which = i+1;
              }
              else {
                // If show0, we keep which = i; otherwise, use i+1.
                if(show1)
                  which = i+1;
              }
            }

            double yval = d_ydata[which][offset];

            // Find if we already have a marker at this location
            std::vector<QwtPlotMarker*>::iterator mitr;
            for(mitr = d_tag_markers[which].begin(); mitr != d_tag_markers[which].end(); mitr++) {
              if((*mitr)->xValue() == sample_offset) {
                break;
              }
            }

            // If no matching marker, create a new one
            if(mitr == d_tag_markers[which].end()) {
              bool show = d_plot_curve[which]->isVisible();

              QwtPlotMarker *m = new QwtPlotMarker();
              m->setXValue(sample_offset);
              m->setYValue(yval);

              QBrush brush(getTagBackgroundColor(), getTagBackgroundStyle());

              QPen pen;
              pen.setColor(Qt::black);
              pen.setWidth(1);

              QwtSymbol *sym = new QwtSymbol(QwtSymbol::NoSymbol, brush, pen, QSize(12,12));

              if(yval >= 0) {
                sym->setStyle(QwtSymbol::DTriangle);
                m->setLabelAlignment(Qt::AlignTop);
              }
              else {
                sym->setStyle(QwtSymbol::UTriangle);
                m->setLabelAlignment(Qt::AlignBottom);
              }

#if QWT_VERSION < 0x060000
              m->setSymbol(*sym);
#else
              m->setSymbol(sym);
#endif
              QwtText tag_label(s.str().c_str());
              tag_label.setColor(getTagTextColor());
              m->setLabel(tag_label);

              m->attach(this);

              if(!(show && d_tag_markers_en[which])) {
                m->hide();
              }

              d_tag_markers[which].push_back(m);
            }
            else {
              // Prepend the new tag to the existing marker
              // And set it at the max value
              if(fabs(yval) < fabs((*mitr)->yValue()))
                (*mitr)->setYValue(yval);
              QString orig = (*mitr)->label().text();
              s << std::endl;
              orig.prepend(s.str().c_str());

              QwtText newtext(orig);
              newtext.setColor(getTagTextColor());

              QBrush brush(getTagBackgroundColor(), getTagBackgroundStyle());
              newtext.setBackgroundBrush(brush);

              (*mitr)->setLabel(newtext);
            }
          }

          tag++;
        }
      }

      if(d_autoscale_state) {
	double bottom=1e20, top=-1e20;
	for(int n = 0; n < d_nplots; n++) {
	  for(int64_t point = 0; point < numDataPoints; point++) {
	    if(d_ydata[n][point] < bottom) {
	      bottom = d_ydata[n][point];
	    }
	    if(d_ydata[n][point] > top) {
	      top = d_ydata[n][point];
	    }
	  }
	}
	_autoScale(bottom, top);
        if(d_autoscale_shot) {
          d_autoscale_state = false;
          d_autoscale_shot = false;
        }
      }

      replot();
    }
  }
}

void
TimeDomainDisplayPlot::legendEntryChecked(QwtPlotItem* plotItem, bool on)
{
  // When line is turned on/off, immediately show/hide tag markers
  for(int n = 0; n < d_nplots; n++) {
    if(plotItem == d_plot_curve[n]) {
      for(size_t i = 0; i < d_tag_markers[n].size(); i++) {
        if(!(!on && d_tag_markers_en[n]))
          d_tag_markers[n][i]->hide();
        else
          d_tag_markers[n][i]->show();
      }
    }
  }
  DisplayPlot::legendEntryChecked(plotItem, on);
}

void
TimeDomainDisplayPlot::legendEntryChecked(const QVariant &plotItem, bool on, int index)
{
#if QWT_VERSION < 0x060100
  std::runtime_error("TimeDomainDisplayPlot::legendEntryChecked with QVariant not enabled in this version of QWT.\n");
#else
  QwtPlotItem *p = infoToItem(plotItem);
  legendEntryChecked(p, on);
#endif /* QWT_VERSION < 0x060100 */
}

void
TimeDomainDisplayPlot::_resetXAxisPoints()
{
  double delt = 1.0/d_sample_rate;
  for(long loc = 0; loc < d_numPoints; loc++){
    d_xdata[loc] = loc*delt;
  }

  // Set up zoomer base for maximum unzoom x-axis
  // and reset to maximum unzoom level
  QwtDoubleRect zbase = d_zoomer->zoomBase();

  if(d_semilogx) {
    setAxisScale(QwtPlot::xBottom, 1e-1, d_numPoints*delt);
    zbase.setLeft(1e-1);
  }
  else {
    setAxisScale(QwtPlot::xBottom, 0, d_numPoints*delt);
    zbase.setLeft(0);
  }

  zbase.setRight(d_numPoints*delt);
  d_zoomer->zoom(zbase);
  d_zoomer->setZoomBase(zbase);
  d_zoomer->zoom(0);

}

void
TimeDomainDisplayPlot::_autoScale(double bottom, double top)
{
  // Auto scale the y-axis with a margin of 20% (10 dB for log scale)
  double _bot = bottom - fabs(bottom)*0.20;
  double _top = top + fabs(top)*0.20;
  if(d_semilogy) {
    if(bottom > 0) {
      setYaxis(_bot-10, _top+10);
    }
    else {
      setYaxis(1e-3, _top+10);
    }
  }
  else {
    setYaxis(_bot, _top);
  }
}

void
TimeDomainDisplayPlot::setAutoScale(bool state)
{
  d_autoscale_state = state;
}

void
TimeDomainDisplayPlot::setAutoScaleShot()
{
  d_autoscale_state = true;
  d_autoscale_shot = true;
}


void
TimeDomainDisplayPlot::setSampleRate(double sr, double units,
				     const std::string &strunits)
{
  double newsr = sr/units;
  if((newsr != d_sample_rate) ||
     (((TimeDomainDisplayZoomer*)d_zoomer)->unitType() != strunits)) {
    d_sample_rate = sr/units;
    _resetXAxisPoints();

    // While we could change the displayed sigfigs based on the unit being
    // displayed, I think it looks better by just setting it to 4 regardless.
    //double display_units = ceil(log10(units)/2.0);
    double display_units = 4;
    setAxisTitle(QwtPlot::xBottom, QString("Time (%1)").arg(strunits.c_str()));
    ((TimeDomainDisplayZoomer*)d_zoomer)->setTimePrecision(display_units);
    ((TimeDomainDisplayZoomer*)d_zoomer)->setUnitType(strunits);
  }
}

double
TimeDomainDisplayPlot::sampleRate() const
{
  return d_sample_rate;
}

void
TimeDomainDisplayPlot::stemPlot(bool en)
{
  if(en) {
    for(int i = 0; i < d_nplots; i++) {
      d_plot_curve[i]->setStyle(QwtPlotCurve::Sticks);
      setLineMarker(i, QwtSymbol::Ellipse);
    }
  }
  else {
    for(int i = 0; i < d_nplots; i++) {
      d_plot_curve[i]->setStyle(QwtPlotCurve::Lines);
      setLineMarker(i, QwtSymbol::NoSymbol);
    }
  }
}

void
TimeDomainDisplayPlot::setSemilogx(bool en)
{
  d_semilogx = en;
  if(!d_semilogx) {
    setAxisScaleEngine(QwtPlot::xBottom, new QwtLinearScaleEngine);
  }
  else {
#if QWT_VERSION < 0x060100
    setAxisScaleEngine(QwtPlot::xBottom, new QwtLog10ScaleEngine);
#else /* QWT_VERSION < 0x060100 */
    setAxisScaleEngine(QwtPlot::xBottom, new QwtLogScaleEngine);
#endif /*QWT_VERSION < 0x060100 */
  }
  _resetXAxisPoints();
}

void
TimeDomainDisplayPlot::setSemilogy(bool en)
{
  if(d_semilogy != en) {
    d_semilogy = en;

#if QWT_VERSION < 0x060100
    double max = axisScaleDiv(QwtPlot::yLeft)->upperBound();
#else /* QWT_VERSION < 0x060100 */
    double max = axisScaleDiv(QwtPlot::yLeft).upperBound();
#endif /* QWT_VERSION < 0x060100 */

    if(!d_semilogy) {
      setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);
      setYaxis(-pow(10.0, max/10.0), pow(10.0, max/10.0));
    }
    else {
#if QWT_VERSION < 0x060100
      setAxisScaleEngine(QwtPlot::yLeft, new QwtLog10ScaleEngine);
#else /* QWT_VERSION < 0x060100 */
      setAxisScaleEngine(QwtPlot::yLeft, new QwtLogScaleEngine);
#endif /*QWT_VERSION < 0x060100 */
      setYaxis(1e-10, 10.0*log10(max));
    }
  }
}

void
TimeDomainDisplayPlot::enableTagMarker(int which, bool en)
{
  if((size_t)which < d_tag_markers_en.size())
    d_tag_markers_en[which] = en;
  else
    throw std::runtime_error("TimeDomainDisplayPlot: enabled tag marker does not exist.\n");
}

const QColor
TimeDomainDisplayPlot::getTagTextColor()
{
  return d_tag_text_color;
}

const QColor
TimeDomainDisplayPlot::getTagBackgroundColor()
{
  return d_tag_background_color;
}

const Qt::BrushStyle
TimeDomainDisplayPlot::getTagBackgroundStyle()
{
  return d_tag_background_style;
}

void
TimeDomainDisplayPlot::setTagTextColor(QColor c)
{
  d_tag_text_color = c;
}

void
TimeDomainDisplayPlot::setTagBackgroundColor(QColor c)
{
  d_tag_background_color = c;
}

void
TimeDomainDisplayPlot::setTagBackgroundStyle(Qt::BrushStyle b)
{
  d_tag_background_style = b;
}


void
TimeDomainDisplayPlot::setYLabel(const std::string &label,
                                 const std::string &unit)
{
  std::string l = label;
  if(unit.length() > 0)
    l += " (" + unit + ")";
  setAxisTitle(QwtPlot::yLeft, QString(l.c_str()));
  ((TimeDomainDisplayZoomer*)d_zoomer)->setYUnitType(unit);
}

void
TimeDomainDisplayPlot::attachTriggerLines(bool en)
{
  if(en) {
    d_trigger_lines[0]->attach(this);
    d_trigger_lines[1]->attach(this);
  }
  else {
    d_trigger_lines[0]->detach();
    d_trigger_lines[1]->detach();
  }
}

void
TimeDomainDisplayPlot::setTriggerLines(double x, double y)
{
  d_trigger_lines[0]->setXValue(x);
  d_trigger_lines[0]->setYValue(y);
  d_trigger_lines[1]->setXValue(x);
  d_trigger_lines[1]->setYValue(y);
}

#endif /* TIME_DOMAIN_DISPLAY_PLOT_C */

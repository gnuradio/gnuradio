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

#ifndef DOMAIN_DISPLAY_PLOT_H
#define DOMAIN_DISPLAY_PLOT_H

#include <stdint.h>
#include <cstdio>
#include <vector>
#include <qwt_plot.h>
#include <qwt_painter.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_engine.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_marker.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>
#include <gnuradio/qtgui/utils.h>

#if QWT_VERSION >= 0x060000
#include <qwt_compat.h>
#endif

typedef QList<QColor> QColorList;
Q_DECLARE_METATYPE ( QColorList )

#if QWT_VERSION < 0x060100
#include <qwt_legend_item.h>
#else /* QWT_VERSION < 0x060100 */
#include <qwt_legend_data.h>
#include <qwt_legend_label.h>
#endif /* QWT_VERSION < 0x060100 */

/*!
 * \brief QWidget base plot to build QTGUI plotting tools.
 * \ingroup qtgui_blk
 */
class DisplayPlot:public QwtPlot
{
  Q_OBJECT

  Q_PROPERTY ( QColor line_color1 READ getLineColor1 WRITE setLineColor1 )
  Q_PROPERTY ( QColor line_color2 READ getLineColor2 WRITE setLineColor2 )
  Q_PROPERTY ( QColor line_color3 READ getLineColor3 WRITE setLineColor3 )
  Q_PROPERTY ( QColor line_color4 READ getLineColor4 WRITE setLineColor4 )
  Q_PROPERTY ( QColor line_color5 READ getLineColor5 WRITE setLineColor5 )
  Q_PROPERTY ( QColor line_color6 READ getLineColor6 WRITE setLineColor6 )
  Q_PROPERTY ( QColor line_color7 READ getLineColor7 WRITE setLineColor7 )
  Q_PROPERTY ( QColor line_color8 READ getLineColor8 WRITE setLineColor8 )
  Q_PROPERTY ( QColor line_color9 READ getLineColor9 WRITE setLineColor9 )

  Q_PROPERTY ( int line_width1 READ getLineWidth1 WRITE setLineWidth1 )
  Q_PROPERTY ( int line_width2 READ getLineWidth2 WRITE setLineWidth2 )
  Q_PROPERTY ( int line_width3 READ getLineWidth3 WRITE setLineWidth3 )
  Q_PROPERTY ( int line_width4 READ getLineWidth4 WRITE setLineWidth4 )
  Q_PROPERTY ( int line_width5 READ getLineWidth5 WRITE setLineWidth5 )
  Q_PROPERTY ( int line_width6 READ getLineWidth6 WRITE setLineWidth6 )
  Q_PROPERTY ( int line_width7 READ getLineWidth7 WRITE setLineWidth7 )
  Q_PROPERTY ( int line_width8 READ getLineWidth8 WRITE setLineWidth8 )
  Q_PROPERTY ( int line_width9 READ getLineWidth9 WRITE setLineWidth9 )

  Q_PROPERTY ( Qt::PenStyle line_style1 READ getLineStyle1 WRITE setLineStyle1 )
  Q_PROPERTY ( Qt::PenStyle line_style2 READ getLineStyle2 WRITE setLineStyle2 )
  Q_PROPERTY ( Qt::PenStyle line_style3 READ getLineStyle3 WRITE setLineStyle3 )
  Q_PROPERTY ( Qt::PenStyle line_style4 READ getLineStyle4 WRITE setLineStyle4 )
  Q_PROPERTY ( Qt::PenStyle line_style5 READ getLineStyle5 WRITE setLineStyle5 )
  Q_PROPERTY ( Qt::PenStyle line_style6 READ getLineStyle6 WRITE setLineStyle6 )
  Q_PROPERTY ( Qt::PenStyle line_style7 READ getLineStyle7 WRITE setLineStyle7 )
  Q_PROPERTY ( Qt::PenStyle line_style8 READ getLineStyle8 WRITE setLineStyle8 )
  Q_PROPERTY ( Qt::PenStyle line_style9 READ getLineStyle9 WRITE setLineStyle9 )

  typedef QwtSymbol::Style QwtSymbolStyle;

  Q_ENUMS ( QwtSymbolStyle )
  Q_PROPERTY ( QwtSymbolStyle line_marker1 READ getLineMarker1 WRITE setLineMarker1 )
  Q_PROPERTY ( QwtSymbolStyle line_marker2 READ getLineMarker2 WRITE setLineMarker2 )
  Q_PROPERTY ( QwtSymbolStyle line_marker3 READ getLineMarker3 WRITE setLineMarker3 )
  Q_PROPERTY ( QwtSymbolStyle line_marker4 READ getLineMarker4 WRITE setLineMarker4 )
  Q_PROPERTY ( QwtSymbolStyle line_marker5 READ getLineMarker5 WRITE setLineMarker5 )
  Q_PROPERTY ( QwtSymbolStyle line_marker6 READ getLineMarker6 WRITE setLineMarker6 )
  Q_PROPERTY ( QwtSymbolStyle line_marker7 READ getLineMarker7 WRITE setLineMarker7 )
  Q_PROPERTY ( QwtSymbolStyle line_marker8 READ getLineMarker8 WRITE setLineMarker8 )
  Q_PROPERTY ( QwtSymbolStyle line_marker9 READ getLineMarker9 WRITE setLineMarker9 )

  Q_PROPERTY ( int marker_alpha1 READ getMarkerAlpha1 WRITE setMarkerAlpha1 )
  Q_PROPERTY ( int marker_alpha2 READ getMarkerAlpha2 WRITE setMarkerAlpha2 )
  Q_PROPERTY ( int marker_alpha3 READ getMarkerAlpha3 WRITE setMarkerAlpha3 )
  Q_PROPERTY ( int marker_alpha4 READ getMarkerAlpha4 WRITE setMarkerAlpha4 )
  Q_PROPERTY ( int marker_alpha5 READ getMarkerAlpha5 WRITE setMarkerAlpha5 )
  Q_PROPERTY ( int marker_alpha6 READ getMarkerAlpha6 WRITE setMarkerAlpha6 )
  Q_PROPERTY ( int marker_alpha7 READ getMarkerAlpha7 WRITE setMarkerAlpha7 )
  Q_PROPERTY ( int marker_alpha8 READ getMarkerAlpha8 WRITE setMarkerAlpha8 )
  Q_PROPERTY ( int marker_alpha9 READ getMarkerAlpha9 WRITE setMarkerAlpha9 )

  Q_PROPERTY ( QColor zoomer_color READ getZoomerColor WRITE setZoomerColor )
  Q_PROPERTY ( QColor palette_color READ getPaletteColor WRITE setPaletteColor )
  Q_PROPERTY ( int yaxis_label_font_size READ getYaxisLabelFontSize WRITE setYaxisLabelFontSize )
  Q_PROPERTY ( int xaxis_label_font_size READ getXaxisLabelFontSize WRITE setXaxisLabelFontSize )
  Q_PROPERTY ( int axes_label_font_size READ getAxesLabelFontSize WRITE setAxesLabelFontSize )

public:
  DisplayPlot(int nplots, QWidget*);
  virtual ~DisplayPlot();

  virtual void replot() = 0;

  const QColor getLineColor1 () const;
  const QColor getLineColor2 () const;
  const QColor getLineColor3 () const;
  const QColor getLineColor4 () const;
  const QColor getLineColor5 () const;
  const QColor getLineColor6 () const;
  const QColor getLineColor7 () const;
  const QColor getLineColor8 () const;
  const QColor getLineColor9 () const;

  int getLineWidth1 () const;
  int getLineWidth2 () const;
  int getLineWidth3 () const;
  int getLineWidth4 () const;
  int getLineWidth5 () const;
  int getLineWidth6 () const;
  int getLineWidth7 () const;
  int getLineWidth8 () const;
  int getLineWidth9 () const;

  const Qt::PenStyle getLineStyle1 () const;
  const Qt::PenStyle getLineStyle2 () const;
  const Qt::PenStyle getLineStyle3 () const;
  const Qt::PenStyle getLineStyle4 () const;
  const Qt::PenStyle getLineStyle5 () const;
  const Qt::PenStyle getLineStyle6 () const;
  const Qt::PenStyle getLineStyle7 () const;
  const Qt::PenStyle getLineStyle8 () const;
  const Qt::PenStyle getLineStyle9 () const;

  const QwtSymbol::Style getLineMarker1 () const;
  const QwtSymbol::Style getLineMarker2 () const;
  const QwtSymbol::Style getLineMarker3 () const;
  const QwtSymbol::Style getLineMarker4 () const;
  const QwtSymbol::Style getLineMarker5 () const;
  const QwtSymbol::Style getLineMarker6 () const;
  const QwtSymbol::Style getLineMarker7 () const;
  const QwtSymbol::Style getLineMarker8 () const;
  const QwtSymbol::Style getLineMarker9 () const;

  int getMarkerAlpha1 () const;
  int getMarkerAlpha2 () const;
  int getMarkerAlpha3 () const;
  int getMarkerAlpha4 () const;
  int getMarkerAlpha5 () const;
  int getMarkerAlpha6 () const;
  int getMarkerAlpha7 () const;
  int getMarkerAlpha8 () const;
  int getMarkerAlpha9 () const;

  QColor getZoomerColor() const;
  QColor getPaletteColor() const;
  int getAxisLabelFontSize(int axisId) const;
  int getYaxisLabelFontSize() const;
  int getXaxisLabelFontSize() const;
  int getAxesLabelFontSize() const;

  // Make sure to create your won PlotNewData method in the derived
  // class:
  // void PlotNewData(...);

public slots:
  virtual void disableLegend();
  virtual void setAxisLabels(bool en);
  virtual void setYaxis(double min, double max);
  virtual void setXaxis(double min, double max);
  virtual void setLineLabel(int which, QString label);
  virtual QString getLineLabel(int which);
  virtual void setLineColor(int which, QColor color);
  virtual QColor getLineColor(int which) const;
  virtual void setLineWidth(int which, int width);
  virtual int getLineWidth(int which) const;
  virtual void setLineStyle(int which, Qt::PenStyle style);
  virtual const Qt::PenStyle getLineStyle(int which) const;
  virtual void setLineMarker(int which, QwtSymbol::Style marker);
  virtual const QwtSymbol::Style getLineMarker(int which) const;
  virtual void setMarkerAlpha(int which, int alpha);
  virtual int getMarkerAlpha(int which) const;

  // Need a function for each curve for setting via stylesheet.
  // Can't use preprocessor directives because we're inside a Q_OBJECT.
  void setLineColor1 (QColor);
  void setLineColor2 (QColor);
  void setLineColor3 (QColor);
  void setLineColor4 (QColor);
  void setLineColor5 (QColor);
  void setLineColor6 (QColor);
  void setLineColor7 (QColor);
  void setLineColor8 (QColor);
  void setLineColor9 (QColor);

  void setLineWidth1 (int);
  void setLineWidth2 (int);
  void setLineWidth3 (int);
  void setLineWidth4 (int);
  void setLineWidth5 (int);
  void setLineWidth6 (int);
  void setLineWidth7 (int);
  void setLineWidth8 (int);
  void setLineWidth9 (int);

  void setLineStyle1 (Qt::PenStyle);
  void setLineStyle2 (Qt::PenStyle);
  void setLineStyle3 (Qt::PenStyle);
  void setLineStyle4 (Qt::PenStyle);
  void setLineStyle5 (Qt::PenStyle);
  void setLineStyle6 (Qt::PenStyle);
  void setLineStyle7 (Qt::PenStyle);
  void setLineStyle8 (Qt::PenStyle);
  void setLineStyle9 (Qt::PenStyle);

  void setLineMarker1 (QwtSymbol::Style);
  void setLineMarker2 (QwtSymbol::Style);
  void setLineMarker3 (QwtSymbol::Style);
  void setLineMarker4 (QwtSymbol::Style);
  void setLineMarker5 (QwtSymbol::Style);
  void setLineMarker6 (QwtSymbol::Style);
  void setLineMarker7 (QwtSymbol::Style);
  void setLineMarker8 (QwtSymbol::Style);
  void setLineMarker9 (QwtSymbol::Style);

  void setMarkerAlpha1 (int);
  void setMarkerAlpha2 (int);
  void setMarkerAlpha3 (int);
  void setMarkerAlpha4 (int);
  void setMarkerAlpha5 (int);
  void setMarkerAlpha6 (int);
  void setMarkerAlpha7 (int);
  void setMarkerAlpha8 (int);
  void setMarkerAlpha9 (int);

  void setZoomerColor(QColor c);
  void setPaletteColor(QColor c);
  void setAxisLabelFontSize(int axisId, int fs);
  void setYaxisLabelFontSize(int fs);
  void setXaxisLabelFontSize(int fs);
  void setAxesLabelFontSize(int fs);

  void setStop(bool on);

  void resizeSlot(QSize *s);

  // Because of the preprocessing of slots in QT, these are not
  // easily separated by the version check. Make one for each
  // version until it's worked out.
  void onPickerPointSelected(const QwtDoublePoint & p);
  void onPickerPointSelected6(const QPointF & p);

signals:
  void plotPointSelected(const QPointF p);

protected slots:
  virtual void legendEntryChecked(QwtPlotItem *plotItem, bool on);
  virtual void legendEntryChecked(const QVariant &plotItem, bool on, int index);

protected:
  int d_nplots;
  std::vector<QwtPlotCurve*> d_plot_curve;

  QwtPlotPanner* d_panner;
  QwtPlotZoomer* d_zoomer;

  QwtDblClickPlotPicker *d_picker;
  QwtPlotMagnifier *d_magnifier;

  int64_t d_numPoints;

  bool d_stop;

  QList<QColor> d_trace_colors;

  bool d_autoscale_state;
};

#endif /* DOMAIN_DISPLAY_PLOT_H */

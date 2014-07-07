/* -*- c++ -*- */
/*
 * Copyright 2012,2013 Free Software Foundation, Inc.
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

#include <iostream>
#include <qimage.h>
#include <qpen.h>
#include <qpainter.h>
#include "qwt_painter.h"
#include "qwt_scale_map.h"
#include "qwt_color_map.h"
#include <gnuradio/qtgui/plot_raster.h>

#if QWT_VERSION < 0x060000
#include "qwt_double_interval.h"
#endif

typedef QVector<QRgb> QwtColorTable;

class PlotTimeRasterImage: public QImage
{
    // This class hides some Qt3/Qt4 API differences
public:
    PlotTimeRasterImage(const QSize &size, QwtColorMap::Format format):
      QImage(size, format == QwtColorMap::RGB
	     ? QImage::Format_ARGB32 : QImage::Format_Indexed8 )
  {
  }

  PlotTimeRasterImage(const QImage &other):
    QImage(other)
  {
  }

  void initColorTable(const QImage& other)
  {
    setColorTable(other.colorTable());
  }
};

class PlotTimeRaster::PrivateData
{
public:
  PrivateData()
  {
    data = NULL;
    colorMap = new QwtLinearColorMap();
  }

  ~PrivateData()
  {
    delete colorMap;
  }

  TimeRasterData *data;
  QwtColorMap *colorMap;
};

/*!
  Sets the following item attributes:
  - QwtPlotItem::AutoScale: true
  - QwtPlotItem::Legend:    false

  The z value is initialized to 8.0.

  \param title Title

  \sa QwtPlotItem::setItemAttribute(), QwtPlotItem::setZ()
*/
PlotTimeRaster::PlotTimeRaster(const QString &title)
    : QwtPlotRasterItem(title)
{
    d_data = new PrivateData();

    setItemAttribute(QwtPlotItem::AutoScale, true);
    setItemAttribute(QwtPlotItem::Legend, false);

    setZ(1.0);
}

//! Destructor
PlotTimeRaster::~PlotTimeRaster()
{
  delete d_data;
}

const TimeRasterData*
PlotTimeRaster::data()const
{
  return d_data->data;
}

void
PlotTimeRaster::setData(TimeRasterData *data)
{
  d_data->data = data;
}

//! \return QwtPlotItem::Rtti_PlotSpectrogram
int
PlotTimeRaster::rtti() const
{
  return QwtPlotItem::Rtti_PlotGrid;
}

/*!
  Change the color map

  Often it is useful to display the mapping between intensities and
  colors as an additional plot axis, showing a color bar.

  \param map Color Map

  \sa colorMap(), QwtScaleWidget::setColorBarEnabled(),
  QwtScaleWidget::setColorMap()
*/
void
PlotTimeRaster::setColorMap(const QwtColorMap *map)
{
    delete d_data->colorMap;
    d_data->colorMap = (QwtColorMap*)map;

    invalidateCache();
    itemChanged();
}

/*!
  \return Color Map used for mapping the intensity values to colors
  \sa setColorMap()
*/
const QwtColorMap &PlotTimeRaster::colorMap() const
{
  return *d_data->colorMap;
}

/*!
  \return Bounding rect of the data
  \sa QwtRasterData::boundingRect
*/
#if QWT_VERSION < 0x060000
QwtDoubleRect PlotTimeRaster::boundingRect() const
{
  return d_data->data->boundingRect();
}
#endif

/*!
  \brief Returns the recommended raster for a given rect.

  F.e the raster hint is used to limit the resolution of
  the image that is rendered.

  \param rect Rect for the raster hint
  \return data().rasterHint(rect)
*/
#if QWT_VERSION < 0x060000
QSize PlotTimeRaster::rasterHint(const QwtDoubleRect &rect) const
{
  return d_data->data->rasterHint(rect);
}
#endif

/*!
  \brief Render an image from the data and color map.

  The area is translated into a rect of the paint device.
  For each pixel of this rect the intensity is mapped
  into a color.

  \param xMap X-Scale Map
  \param yMap Y-Scale Map
  \param area Area that should be rendered in scale coordinates.

  \return A QImage::Format_Indexed8 or QImage::Format_ARGB32 depending
  on the color map.

  \sa QwtRasterData::intensity(), QwtColorMap::rgb(),
  QwtColorMap::colorIndex()
*/
#if QWT_VERSION < 0x060000
QImage PlotTimeRaster::renderImage(const QwtScaleMap &xMap,
				   const QwtScaleMap &yMap,
				   const QwtDoubleRect &area) const
#else
QImage PlotTimeRaster::renderImage(const QwtScaleMap &xMap,
				   const QwtScaleMap &yMap,
				   const QRectF &area,
				   const QSize &size) const
#endif
{
  if(area.isEmpty())
    return QImage();

#if QWT_VERSION < 0x060000
  QRect rect = transform(xMap, yMap, area);
  const QSize res = d_data->data->rasterHint(area);
#else
  QRect rect = QwtScaleMap::transform(xMap, yMap, area).toRect();
  const QSize res(-1,-1);

#endif

  QwtScaleMap xxMap = xMap;
  QwtScaleMap yyMap = yMap;

  if(res.isValid()) {
    /*
      It is useless to render an image with a higher resolution
      than the data offers. Of course someone will have to
      scale this image later into the size of the given rect, but f.e.
      in case of postscript this will done on the printer.
    */
    rect.setSize(rect.size().boundedTo(res));

    int px1 = rect.x();
    int px2 = rect.x() + rect.width();
    if(xMap.p1() > xMap.p2())
      qSwap(px1, px2);

    double sx1 = area.x();
    double sx2 = area.x() + area.width();
    if(xMap.s1() > xMap.s2())
      qSwap(sx1, sx2);

    int py1 = rect.y();
    int py2 = rect.y() + rect.height();
    if(yMap.p1() > yMap.p2())
      qSwap(py1, py2);

    double sy1 = area.y();
    double sy2 = area.y() + area.height();
    if(yMap.s1() > yMap.s2())
      qSwap(sy1, sy2);

    xxMap.setPaintInterval(px1, px2);
    xxMap.setScaleInterval(sx1, sx2);
    yyMap.setPaintInterval(py1, py2);
    yyMap.setScaleInterval(sy1, sy2);
  }

  PlotTimeRasterImage image(rect.size(), d_data->colorMap->format());

#if QWT_VERSION < 0x060000
  const QwtDoubleInterval intensityRange = d_data->data->range();
#else
  const QwtInterval intensityRange = d_data->data->interval(Qt::ZAxis);
#endif
  if(!intensityRange.isValid())
    return image;

  d_data->data->initRaster(area, rect.size());

  if(d_data->colorMap->format() == QwtColorMap::RGB) {
    for(int y = rect.top(); y <= rect.bottom(); y++) {
      const double ty = yyMap.invTransform(y);

      QRgb *line = (QRgb *)image.scanLine(y - rect.top());

      for(int x = rect.left(); x <= rect.right(); x++) {
	const double tx = xxMap.invTransform(x);

	*line++ = d_data->colorMap->rgb(intensityRange,
					d_data->data->value(tx, ty));
      }
    }
    d_data->data->incrementResidual();
  }
  else if(d_data->colorMap->format() == QwtColorMap::Indexed) {
    image.setColorTable(d_data->colorMap->colorTable(intensityRange));

    for(int y = rect.top(); y <= rect.bottom(); y++) {
      const double ty = yyMap.invTransform(y);

      unsigned char *line = image.scanLine(y - rect.top());
      for(int x = rect.left(); x <= rect.right(); x++) {
	const double tx = xxMap.invTransform(x);

	*line++ = d_data->colorMap->colorIndex(intensityRange,
					       d_data->data->value(tx, ty));
      }
    }
  }

  d_data->data->discardRaster();

  // Mirror the image in case of inverted maps

  const bool hInvert = xxMap.p1() > xxMap.p2();
  const bool vInvert = yyMap.p1() > yyMap.p2();
  if(hInvert || vInvert) {
    image = image.mirrored(hInvert, vInvert);
  }

  return image;
}

#if QWT_VERSION < 0x060000
QwtDoubleInterval
PlotTimeRaster::interval(Qt::Axis ax) const
{
  return d_data->data->range();
}

#else

QwtInterval
PlotTimeRaster::interval(Qt::Axis ax) const
{
  return d_data->data->interval(ax);
}
#endif

/*!
  \brief Draw the raster

  \param painter Painter
  \param xMap Maps x-values into pixel coordinates.
  \param yMap Maps y-values into pixel coordinates.
  \param canvasRect Contents rect of the canvas in painter coordinates

  \sa setDisplayMode, renderImage,
  QwtPlotRasterItem::draw, drawContourLines
*/

void PlotTimeRaster::draw(QPainter *painter,
			  const QwtScaleMap &xMap,
			  const QwtScaleMap &yMap,
			  const QRect &canvasRect) const
{
  QwtPlotRasterItem::draw(painter, xMap, yMap, canvasRect);
}


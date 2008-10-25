#ifndef PLOT_WATERFALL_H
#define PLOT_WATERFALL_H

#include <qglobal.h>
#include <waterfallGlobalData.h>

#include "qwt_valuelist.h" 
#include "qwt_plot_rasteritem.h" 

class QwtColorMap;

/*!
  \brief A plot item, which displays a waterfall spectrogram

  A waterfall displays threedimenional data, where the 3rd dimension
  ( the intensity ) is displayed using colors. The colors are calculated
  from the values using a color map.
  
  \image html spectrogram3.png

  \sa QwtRasterData, QwtColorMap
*/

class QWT_EXPORT PlotWaterfall: public QwtPlotRasterItem
{
public:
    explicit PlotWaterfall(WaterfallData* data, const QString &title = QString::null);
    virtual ~PlotWaterfall();

    const WaterfallData* data()const;

    void setColorMap(const QwtColorMap &);
    const QwtColorMap &colorMap() const;

    virtual QwtDoubleRect boundingRect() const;
    virtual QSize rasterHint(const QwtDoubleRect &) const;

    virtual int rtti() const;

    virtual void draw(QPainter *p,
        const QwtScaleMap &xMap, const QwtScaleMap &yMap,
        const QRect &rect) const;

protected:
    virtual QImage renderImage(
        const QwtScaleMap &xMap, const QwtScaleMap &yMap, 
        const QwtDoubleRect &rect) const;

private:
    class PrivateData;
    PrivateData *d_data;
};

#endif

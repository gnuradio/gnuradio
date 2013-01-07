#ifndef WATERFALL_GLOBAL_DATA_HPP
#define WATERFALL_GLOBAL_DATA_HPP

#include <qwt_raster_data.h>
#include <inttypes.h>

#if QWT_VERSION >= 0x060000
#include <qwt_point_3d.h>  // doesn't seem necessary, but is...
#include <qwt_compat.h>
#endif

class WaterfallData: public QwtRasterData
{
public:
  WaterfallData(const double, const double, const uint64_t, const unsigned int);
  virtual ~WaterfallData();

  virtual void reset();
  virtual void copy(const WaterfallData*);

  virtual void resizeData(const double, const double, const uint64_t);

  virtual QwtRasterData *copy() const;

#if QWT_VERSION < 0x060000
  virtual QwtDoubleInterval range() const;
  virtual void setRange(const QwtDoubleInterval&);
#endif

  virtual double value(double x, double y) const;

  virtual uint64_t getNumFFTPoints()const;
  virtual void addFFTData(const double*, const uint64_t, const int);

  virtual double* getSpectrumDataBuffer()const;
  virtual void setSpectrumDataBuffer(const double*);

  virtual int getNumLinesToUpdate()const;
  virtual void setNumLinesToUpdate(const int);
  virtual void incrementNumLinesToUpdate();

protected:

  double* _spectrumData;
  uint64_t _fftPoints;
  uint64_t _historyLength;
  int _numLinesToUpdate;

#if QWT_VERSION < 0x060000
  QwtDoubleInterval _intensityRange;
#else
  QwtInterval _intensityRange;
#endif

private:

};

#endif /* WATERFALL_GLOBAL_DATA_HPP */

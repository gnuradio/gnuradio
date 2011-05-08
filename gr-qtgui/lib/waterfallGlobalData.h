#ifndef WATERFALL_GLOBAL_DATA_HPP
#define WATERFALL_GLOBAL_DATA_HPP

#include <qwt_raster_data.h>
#include <inttypes.h>


class WaterfallData: public QwtRasterData
{
public:
  WaterfallData(const double, const double, const uint64_t, const unsigned int);
  virtual ~WaterfallData();
  
  virtual void Reset();
  virtual void Copy(const WaterfallData*);
  
  virtual void ResizeData(const double, const double, const uint64_t);
  
  virtual QwtRasterData *copy() const;
  virtual QwtDoubleInterval range() const;
  virtual void setRange(const QwtDoubleInterval&);

  virtual double value(double x, double y) const;
  
  virtual uint64_t GetNumFFTPoints()const;
  virtual void addFFTData(const double*, const uint64_t, const int);
  
  virtual double* GetSpectrumDataBuffer()const;
  virtual void SetSpectrumDataBuffer(const double*);
  
  virtual int GetNumLinesToUpdate()const;
  virtual void SetNumLinesToUpdate(const int);
  virtual void IncrementNumLinesToUpdate();

protected:

  double* _spectrumData;
  uint64_t _fftPoints;
  uint64_t _historyLength;
  int _numLinesToUpdate;
  QwtDoubleInterval _intensityRange;

private:

};

#endif /* WATERFALL_GLOBAL_DATA_HPP */

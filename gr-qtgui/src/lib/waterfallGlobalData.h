#ifndef WATERFALL_GLOBAL_DATA_HPP
#define WATERFALL_GLOBAL_DATA_HPP

#include <qwt_raster_data.h>
#include <qwt3d_function.h>
#include <inttypes.h>

class Waterfall3DDisplayPlot;

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

class Waterfall3DData: public WaterfallData, public Qwt3D::Function
{
public:
  Waterfall3DData(const double, const double, const uint64_t, const unsigned int);
  virtual ~Waterfall3DData();
    
  virtual void ResizeData(const double, const double, const uint64_t);

  virtual bool create();
  virtual void setMesh(unsigned int columns, unsigned int rows); //!< Sets number of rows and columns. 

  virtual double operator()(double x, double y);

  virtual double GetFloorValue()const;
  virtual void SetFloorValue(const double);

  virtual double minZ()const;
  virtual double maxZ()const;

protected:
  void _ResizeMesh();

  double** data;
  double _floorValue;

private:

};


#endif /* WATERFALL_GLOBAL_DATA_HPP */

#ifndef WATERFALL_3D_DISPLAY_PLOT_HPP
#define WATERFALL_3D_DISPLAY_PLOT_HPP

#include <highResTimeFunctions.h>

#include <waterfallGlobalData.h>
#include <qwt3d_surfaceplot.h>

#include <qwt3d_color.h>
#include <qwt_color_map.h>

class Waterfall3DColorMap:public Qwt3D::Color, public QwtLinearColorMap{
public:
  Waterfall3DColorMap();
  virtual ~Waterfall3DColorMap();

  virtual Qwt3D::RGBA operator()(double x, double y, double z)const;
  virtual Qwt3D::ColorVector& createVector(Qwt3D::ColorVector& vec);

  virtual void SetInterval(const double minValue, const double maxValue);

protected:
  
private:
  QwtDoubleInterval _interval;
};

class Waterfall3DDisplayPlot:public Qwt3D::SurfacePlot{
  Q_OBJECT

  protected:
  class IntensityScale:public Qwt3D::LinearScale{

  friend class Qwt3D::Axis;
  friend class Qwt3D::qwt3d_ptr<Qwt3D::Scale>;

  private:
    double _floor;

  public:
    explicit IntensityScale(const double newFloor):_floor(newFloor){ }
    virtual ~IntensityScale(){}

    virtual QString ticLabel(unsigned int idx) const{
      if (idx<majors_p.size())
	{
	  return QString("%1").arg( majors_p[idx] + GetFloorValue(), 0, 'f', 0 );
	}
      return QString("");
    }

    virtual double GetFloorValue()const{ return _floor; }
    virtual void SetFloorValue(const double newFloor){ _floor = newFloor; }

    //! Returns a new heap based object utilized from qwt3d_ptr
    Scale* clone() const {return new IntensityScale(*this);} 
  };
  
  class TimeScale:public Qwt3D::LinearScale{

    friend class Qwt3D::Axis;
    friend class Qwt3D::qwt3d_ptr<Qwt3D::Scale>;
    friend class Waterfall3DDisplayPlot;

  private:
    Waterfall3DDisplayPlot* _plot;

  public:
    TimeScale(Waterfall3DDisplayPlot* plot ):_plot(plot){
    }
    virtual ~TimeScale(){
    }

    virtual QString ticLabel(unsigned int idx) const{
      if (idx<majors_p.size())
	{
	  const timespec markerTime = timespec_add(_plot->_dataTimestamp, -(_plot->_timePerFFT) * majors_p[idx]);
	  struct tm timeTm;
	  gmtime_r(&markerTime.tv_sec, &timeTm);
	  
	  char* timeBuffer = new char[128];
	  snprintf(timeBuffer, 128, "%02d:%02d:%02d.%03ld", timeTm.tm_hour, timeTm.tm_min, timeTm.tm_sec, (markerTime.tv_nsec / 1000000));
	  QString returnBuffer(timeBuffer);
	  delete[] timeBuffer;
	  return returnBuffer;
	}
      return QString("");
    }

    //! Returns a new heap based object utilized from qwt3d_ptr
    Scale* clone() const {return new TimeScale(*this);}
  };

  class FrequencyScale: public Qwt3D::LinearScale{

    friend class Qwt3D::Axis;
    friend class Qwt3D::qwt3d_ptr<Qwt3D::Scale>;
  private:
    double _centerFrequency;
    bool _useCenterFrequencyFlag;
  public:
    FrequencyScale(bool useCenterFrequencyFlag, double centerFrequency):_centerFrequency(centerFrequency),_useCenterFrequencyFlag(useCenterFrequencyFlag){}
    virtual ~FrequencyScale(){}

    virtual QString ticLabel(unsigned int idx) const{
      if (idx<majors_p.size())
	{
	  if(!_useCenterFrequencyFlag){
	    return QString("%1").arg(  majors_p[idx], 0, 'f', 0 );

	  }
	  else{
	    return QString("%1").arg(  (majors_p[idx] + _centerFrequency)/1000.0, 0, 'f', 3 );
	  }
	}
      return QString("");
    }

    //! Returns a new heap based object utilized from qwt3d_ptr
    Scale* clone() const {return new FrequencyScale(*this);}
  };

public:
  Waterfall3DDisplayPlot(QWidget*);
  virtual ~Waterfall3DDisplayPlot();
						
  void Init();
  void Reset();

  bool loadFromData(double** data, unsigned int columns, unsigned int rows
		    ,double minx, double maxx, double miny, double maxy);

  void SetFrequencyRange(const double, const double, const double, const bool);
  double GetStartFrequency()const;
  double GetStopFrequency()const;

  void PlotNewData(const double* dataPoints, const int64_t numDataPoints, const double timePerFFT, const timespec timestamp, const int droppedFrames);

  void SetIntensityRange(const double minIntensity, const double maxIntensity);

  virtual void replot(void);

  int GetIntensityColorMapType()const;
  void SetIntensityColorMapType( const int, const QColor, const QColor, const bool forceFlag = false, const bool noReplotFlag = false );
  const QColor GetUserDefinedLowIntensityColor()const;
  const QColor GetUserDefinedHighIntensityColor()const;

  static const int INTENSITY_COLOR_MAP_TYPE_MULTI_COLOR = 0;
  static const int INTENSITY_COLOR_MAP_TYPE_WHITE_HOT = 1;
  static const int INTENSITY_COLOR_MAP_TYPE_BLACK_HOT = 2;
  static const int INTENSITY_COLOR_MAP_TYPE_INCANDESCENT = 3;
  static const int INTENSITY_COLOR_MAP_TYPE_USER_DEFINED = 4;

signals:
  void UpdatedLowerIntensityLevel(const double);
  void UpdatedUpperIntensityLevel(const double);

protected:

  double _startFrequency;
  double _stopFrequency;

  Waterfall3DData* _waterfallData;

  timespec _lastReplot;

  int64_t _numPoints;

  double _displayIntervalTime;

  int _intensityColorMapType;
  QColor _userDefinedLowIntensityColor;
  QColor _userDefinedHighIntensityColor;

  bool _useCenterFrequencyFlag;
  double _centerFrequency;

  timespec _dataTimestamp;
  double _timePerFFT;
  bool _initialized;

  bool _createCoordinateSystemFlag;

private:

};

#endif /* WATERFALL_3D_DISPLAY_PLOT_HPP */

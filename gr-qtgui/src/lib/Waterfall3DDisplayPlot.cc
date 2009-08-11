#ifndef WATERFALL_3D_DISPLAY_PLOT_C
#define WATERFALL_3D_DISPLAY_PLOT_C

#include <Waterfall3DDisplayPlot.h>

#include <qwt3d_helper.h>
#include <qapplication.h>

Waterfall3DColorMap::Waterfall3DColorMap(): Qwt3D::Color(), QwtLinearColorMap(){
  _interval.setInterval(0, 1.0);

}

Waterfall3DColorMap::~Waterfall3DColorMap(){

}

Qwt3D::RGBA
Waterfall3DColorMap::operator()(double, double, double z) const
{
  return Qwt3D::RGBA(Qwt3D::Qt2GL(color(_interval, z)));
}

void
Waterfall3DColorMap::SetInterval(const double minValue, const double maxValue)
{
  _interval.setInterval(minValue, maxValue);
}

Qwt3D::ColorVector&
Waterfall3DColorMap::createVector(Qwt3D::ColorVector& vec)
{
  // Generate 100 interval values and then return those
  Qwt3D::ColorVector colorVec;
  for(unsigned int number = 0; number < 100; number++){
    double value = (_interval.width() * (static_cast<double>(number) / 100.0)) + _interval.minValue();
    colorVec.push_back(operator()(0,0,value));
  }
  vec = colorVec;
  return vec; 
}


const int Waterfall3DDisplayPlot::INTENSITY_COLOR_MAP_TYPE_MULTI_COLOR;
const int Waterfall3DDisplayPlot::INTENSITY_COLOR_MAP_TYPE_WHITE_HOT;
const int Waterfall3DDisplayPlot::INTENSITY_COLOR_MAP_TYPE_BLACK_HOT;
const int Waterfall3DDisplayPlot::INTENSITY_COLOR_MAP_TYPE_INCANDESCENT;
const int Waterfall3DDisplayPlot::INTENSITY_COLOR_MAP_TYPE_USER_DEFINED;

Waterfall3DDisplayPlot::Waterfall3DDisplayPlot(QWidget* parent):Qwt3D::SurfacePlot(parent)
{
  _startFrequency = 0;
  _stopFrequency = 4000;

  _createCoordinateSystemFlag = true;

  _initialized = false;

  _numPoints = 1024;

  _displayIntervalTime = (1.0/5.0); // 1/5 of a second between updates

  timespec_reset(&_lastReplot);

  _useCenterFrequencyFlag = false;
  _centerFrequency = 0.0;

  _timePerFFT = 1.0;
  timespec_reset(&_dataTimestamp);

  coordinates()->setAutoScale(false);

  _waterfallData = new Waterfall3DData(_startFrequency, _stopFrequency, _numPoints, 200);
  _waterfallData->assign(this);
  _waterfallData->create();

  _intensityColorMapType = -1;
  SetIntensityColorMapType(INTENSITY_COLOR_MAP_TYPE_MULTI_COLOR, Qt::white, Qt::black, true, true);

  legend()->setScale(Qwt3D::LINEARSCALE);
  legend()->setTitleString("Intensity");

  enableMouse(true);  
}

Waterfall3DDisplayPlot::~Waterfall3DDisplayPlot()
{
  delete _waterfallData;
}

void
Waterfall3DDisplayPlot::Init()
{
  if(!_initialized && initializedGL()){
    resize(parentWidget()->width(), parentWidget()->height());

    // Attempting to prevent valgrind uninitialized variable errors in QwtPlot3d::Drawable class
    glDisable(GL_POLYGON_OFFSET_FILL);
    
    setPlotStyle(Qwt3D::FILLED);
    
    setCoordinateStyle(Qwt3D::FRAME);
    setFloorStyle(Qwt3D::FLOORDATA);
    setOrtho(true);  

    _initialized = true;
  }
}

void
Waterfall3DDisplayPlot::Reset()
{
  _waterfallData->ResizeData(_startFrequency, _stopFrequency, _numPoints);
  _waterfallData->Reset();

  if(initializedGL()){
    setScale(1, static_cast<int>(((_stopFrequency - _startFrequency) / 200) ), 10);
  }

  _createCoordinateSystemFlag = true;

  timespec_reset(&_dataTimestamp);
  _timePerFFT = 1.0;
}

void
Waterfall3DDisplayPlot::SetFrequencyRange(const double constStartFreq,
					  const double constStopFreq,
					  const double constCenterFreq,
					  const bool useCenterFrequencyFlag,
					  const double units,
					  const std::string &strunits)
{
  double startFreq = constStartFreq / units;
  double stopFreq = constStopFreq / units;
  double centerFreq = constCenterFreq / units;

  if(stopFreq > startFreq) {
    _startFrequency = startFreq;
    _stopFrequency = stopFreq;

    _useCenterFrequencyFlag = useCenterFrequencyFlag;
    _centerFrequency = centerFreq;

    Reset();

    // Only replot if screen is visible
    if(isVisible()){
      replot();
    }
  }
}

bool
Waterfall3DDisplayPlot::loadFromData(double** data, unsigned int columns, unsigned int rows
				     ,double minx, double maxx, double miny, double maxy)
{

  Qwt3D::GridData* gridPtr = (Qwt3D::GridData*)actualData_p;
  
  gridPtr->setPeriodic(false,false);
  gridPtr->setSize(columns,rows);
  readIn(*gridPtr,data,columns,rows,minx,maxx,miny,maxy);
  calcNormals(*gridPtr);  
  
  updateData();
  updateNormals();
  
  if( _createCoordinateSystemFlag ){
    createCoordinateSystem();
    
    for (unsigned i=0; i!=coordinates()->axes.size(); ++i)
      {
	coordinates()->axes[i].setMajors(5);
	coordinates()->axes[i].setMinors(3);
      }
    
    coordinates()->axes[Qwt3D::Y1].setLabelString("Time");
    coordinates()->axes[Qwt3D::Y2].setLabelString("Time");
    coordinates()->axes[Qwt3D::Y3].setLabelString("Time");
    coordinates()->axes[Qwt3D::Y4].setLabelString("Time");
    coordinates()->axes[Qwt3D::Z1].setLabelString("Intensity (dB)");
    coordinates()->axes[Qwt3D::Z2].setLabelString("Intensity (dB)");
    coordinates()->axes[Qwt3D::Z3].setLabelString("Intensity (dB)");
    coordinates()->axes[Qwt3D::Z4].setLabelString("Intensity (dB)");

    coordinates()->axes[Qwt3D::X1].setLabelString((!_useCenterFrequencyFlag ? "Frequency (Hz)" : "Frequency (kHz)"));
    coordinates()->axes[Qwt3D::X2].setLabelString((!_useCenterFrequencyFlag ? "Frequency (Hz)" : "Frequency (kHz)"));
    coordinates()->axes[Qwt3D::X3].setLabelString((!_useCenterFrequencyFlag ? "Frequency (Hz)" : "Frequency (kHz)"));
    coordinates()->axes[Qwt3D::X4].setLabelString((!_useCenterFrequencyFlag ? "Frequency (Hz)" : "Frequency (kHz)"));

    // The QwtPlot3D Interface takes ownership of these items, so there is no need to delete them...
    coordinates()->axes[Qwt3D::X1].setScale(new FrequencyScale(_useCenterFrequencyFlag, _centerFrequency));
    coordinates()->axes[Qwt3D::X2].setScale(new FrequencyScale(_useCenterFrequencyFlag, _centerFrequency));
    coordinates()->axes[Qwt3D::X3].setScale(new FrequencyScale(_useCenterFrequencyFlag, _centerFrequency));
    coordinates()->axes[Qwt3D::X4].setScale(new FrequencyScale(_useCenterFrequencyFlag, _centerFrequency));

    coordinates()->axes[Qwt3D::Y1].setScale(new TimeScale(this));
    coordinates()->axes[Qwt3D::Y2].setScale(new TimeScale(this));
    coordinates()->axes[Qwt3D::Y3].setScale(new TimeScale(this));
    coordinates()->axes[Qwt3D::Y4].setScale(new TimeScale(this));

    coordinates()->axes[Qwt3D::Z1].setScale(new IntensityScale(_waterfallData->GetFloorValue()));
    coordinates()->axes[Qwt3D::Z2].setScale(new IntensityScale(_waterfallData->GetFloorValue()));
    coordinates()->axes[Qwt3D::Z3].setScale(new IntensityScale(_waterfallData->GetFloorValue()));
    coordinates()->axes[Qwt3D::Z4].setScale(new IntensityScale(_waterfallData->GetFloorValue()));

    _createCoordinateSystemFlag = false;
  }
  
  return true;
}

double
Waterfall3DDisplayPlot::GetStartFrequency() const
{
  return _startFrequency;
}

double
Waterfall3DDisplayPlot::GetStopFrequency() const
{
  return _stopFrequency;
}

void
Waterfall3DDisplayPlot::PlotNewData(const double* dataPoints,
				    const int64_t numDataPoints,
				    const double timePerFFT,
				    const timespec timestamp,
				    const int droppedFrames)
{
  if(numDataPoints > 0){
    if(numDataPoints != _numPoints){
      _numPoints = numDataPoints;

      Reset();

      if(isVisible()){
	replot();
      }

      _createCoordinateSystemFlag = true;

      _lastReplot = get_highres_clock();
    }

    _dataTimestamp = timestamp;
    _timePerFFT = timePerFFT;

    _waterfallData->addFFTData(dataPoints, numDataPoints, droppedFrames);
    _waterfallData->IncrementNumLinesToUpdate();
  }

  // Allow at least a 50% duty cycle
  if(diff_timespec(get_highres_clock(), _lastReplot) > _displayIntervalTime){
    // Only update when window is visible
    if(isVisible()){
      replot();
    }

    _lastReplot = get_highres_clock();
  }
}

void
Waterfall3DDisplayPlot::SetIntensityRange(const double minIntensity,
					  const double maxIntensity)
{
  _waterfallData->SetFloorValue(minIntensity);
  _waterfallData->setMinZ(0);
  _waterfallData->setMaxZ(maxIntensity-minIntensity);

  _createCoordinateSystemFlag = true;

  emit UpdatedLowerIntensityLevel(minIntensity);
  emit UpdatedUpperIntensityLevel(maxIntensity);

  SetIntensityColorMapType(_intensityColorMapType,
			   _userDefinedLowIntensityColor,
			   _userDefinedLowIntensityColor, true);
}

void
Waterfall3DDisplayPlot::replot()
{
  if(!_initialized){
    Init();
  }
  if(initializedGL()){
    const timespec startTime = get_highres_clock();
    
    _waterfallData->create();
    
    legend()->setMajors(4);
    legend()->setMinors(5);
    double start, stop;
    coordinates()->axes[Qwt3D::Z1].limits(start,stop);
    legend()->setLimits( _waterfallData->GetFloorValue(), _waterfallData->GetFloorValue() + stop - start );

    coordinates()->axes[Qwt3D::X1].limits(start,stop);

    showColorLegend(true);

    updateGL();

    double differenceTime = (diff_timespec(get_highres_clock(), startTime));
  
    // Require at least a 20% duty cycle
    differenceTime *= 4.0;
    if(differenceTime > (1.0/5.0)){
      _displayIntervalTime = differenceTime;
    }
  }
}

void
Waterfall3DDisplayPlot::resizeSlot( QSize *s )
{
  resize(s->width(), s->height());
}

int
Waterfall3DDisplayPlot::GetIntensityColorMapType() const
{
  return _intensityColorMapType;
}

void
Waterfall3DDisplayPlot::SetIntensityColorMapType(const int newType,
						 const QColor lowColor,
						 const QColor highColor,
						 const bool forceFlag,
						 const bool noReplotFlag)
{
  if(((_intensityColorMapType != newType) || forceFlag) || 
     ((newType == INTENSITY_COLOR_MAP_TYPE_USER_DEFINED) &&
      (lowColor.isValid() && highColor.isValid()))){

    Waterfall3DColorMap* colorMap = new Waterfall3DColorMap();
    colorMap->SetInterval(_waterfallData->minZ(), _waterfallData->maxZ());

    switch(newType){
    case INTENSITY_COLOR_MAP_TYPE_MULTI_COLOR:{
      _intensityColorMapType = newType;
      colorMap->setColorInterval(Qt::darkCyan, Qt::white);
      colorMap->addColorStop(0.25, Qt::cyan);
      colorMap->addColorStop(0.5, Qt::yellow);
      colorMap->addColorStop(0.75, Qt::red);
      setBackgroundColor(Qwt3D::Qt2GL(Qt::gray));
      break;
    }
    case INTENSITY_COLOR_MAP_TYPE_WHITE_HOT:{
      _intensityColorMapType = newType;
      colorMap->setColorInterval(Qt::black, Qt::white);
      setBackgroundColor(Qwt3D::Qt2GL(Qt::blue));
      break;
    }
    case INTENSITY_COLOR_MAP_TYPE_BLACK_HOT:{
      _intensityColorMapType = newType;
      colorMap->setColorInterval(Qt::white, Qt::black);
      setBackgroundColor(Qwt3D::Qt2GL(Qt::blue));
      break;
    }
    case INTENSITY_COLOR_MAP_TYPE_INCANDESCENT:{
      _intensityColorMapType = newType;
      colorMap->setColorInterval(Qt::black, Qt::white);
      colorMap->addColorStop(0.5, Qt::darkRed);
      setBackgroundColor(Qwt3D::Qt2GL(Qt::gray));
      break;
    }
    case INTENSITY_COLOR_MAP_TYPE_USER_DEFINED:{
      _userDefinedLowIntensityColor = lowColor;
      _userDefinedHighIntensityColor = highColor;
      _intensityColorMapType = newType;
      colorMap->setColorInterval(_userDefinedLowIntensityColor, _userDefinedHighIntensityColor);
      setBackgroundColor(Qwt3D::Qt2GL(Qt::white));
      break;
    }
    default:
      colorMap->setColorInterval(Qt::black, Qt::white);
      break;
    }

    // Qwt3D takes over destruction of this object...
    setDataColor(colorMap);

    if(!noReplotFlag){
      // Draw again
      replot();
      
      // Update the last replot timer
      _lastReplot = get_highres_clock();
    }
  }
}

const QColor
Waterfall3DDisplayPlot::GetUserDefinedLowIntensityColor() const
{
  return _userDefinedLowIntensityColor;
}

const QColor
Waterfall3DDisplayPlot::GetUserDefinedHighIntensityColor() const
{
  return _userDefinedHighIntensityColor;
}

#endif /* WATERFALL_3D_DISPLAY_PLOT_C */

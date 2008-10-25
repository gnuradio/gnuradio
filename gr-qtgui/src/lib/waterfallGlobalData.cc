#ifndef WATERFALL_GLOBAL_DATA_CPP
#define WATERFALL_GLOBAL_DATA_CPP

#include <waterfallGlobalData.h>

#include <Waterfall3DDisplayPlot.h>

WaterfallData::WaterfallData(const double minimumFrequency, const double maximumFrequency, const uint64_t fftPoints, const unsigned int historyExtent):
  QwtRasterData(QwtDoubleRect(minimumFrequency /* X START */,  0 /* Y START */, maximumFrequency - minimumFrequency /* WIDTH */,  static_cast<double>(historyExtent)/* HEIGHT */))

{
  _intensityRange = QwtDoubleInterval(-200.0, 0.0);
  
  _fftPoints = fftPoints;
  _historyLength = historyExtent;

  _spectrumData = new double[_fftPoints * _historyLength];

  Reset();
}

WaterfallData::~WaterfallData(){
  delete[] _spectrumData;
}

void WaterfallData::Reset(){
  memset(_spectrumData, 0x0, _fftPoints*_historyLength*sizeof(double));

  _numLinesToUpdate = -1;
}

void WaterfallData::Copy(const WaterfallData* rhs){
  if((_fftPoints != rhs->GetNumFFTPoints()) ||
     (boundingRect() != rhs->boundingRect()) ){
    _fftPoints = rhs->GetNumFFTPoints();
    setBoundingRect(rhs->boundingRect());
    delete[] _spectrumData;
    _spectrumData = new double[_fftPoints * _historyLength];
  }
  Reset();
  SetSpectrumDataBuffer(rhs->GetSpectrumDataBuffer());
  SetNumLinesToUpdate(rhs->GetNumLinesToUpdate());
  setRange(rhs->range());
}

void WaterfallData::ResizeData(const double startFreq, const double stopFreq, const uint64_t fftPoints){
  if((fftPoints != GetNumFFTPoints()) ||
     (boundingRect().width() != (stopFreq - startFreq)) ||
     (boundingRect().left() != startFreq)){

    setBoundingRect(QwtDoubleRect(startFreq, 0, stopFreq-startFreq, boundingRect().height()));
    _fftPoints = fftPoints;
    delete[] _spectrumData;
    _spectrumData = new double[_fftPoints * _historyLength];
  }
   
  Reset();
}

QwtRasterData *WaterfallData::copy() const{
  WaterfallData* returnData =  new WaterfallData(boundingRect().left(), boundingRect().right(), _fftPoints, _historyLength);
  returnData->Copy(this);
  return returnData;
}

QwtDoubleInterval WaterfallData::range() const{
  return _intensityRange;
}

void WaterfallData::setRange(const QwtDoubleInterval& newRange){
  _intensityRange = newRange;
}

double WaterfallData::value(double x, double y) const{
  double returnValue = 0.0;

  const unsigned int intY = static_cast<unsigned int>((1.0 - (y/boundingRect().height())) * 
						      static_cast<double>(_historyLength - 1));
  const unsigned int intX = static_cast<unsigned int>((((x - boundingRect().left()) / boundingRect().width()) * 
						       static_cast<double>(_fftPoints-1)) + 0.5);

  const int location = (intY * _fftPoints) + intX;
  if((location > -1) && (location < static_cast<int64_t>(_fftPoints * _historyLength))){
    returnValue = _spectrumData[location];
  }

  return returnValue;
}

uint64_t WaterfallData::GetNumFFTPoints()const{
  return _fftPoints;
}

void WaterfallData::addFFTData(const double* fftData, const uint64_t fftDataSize, const int droppedFrames){
  if(fftDataSize == _fftPoints){
    int64_t heightOffset = _historyLength - 1 - droppedFrames;
    uint64_t drawingDroppedFrames = droppedFrames;

    // Any valid data rolled off the display so just fill in zeros and write new data
    if(heightOffset < 0){
      heightOffset = 0;
      drawingDroppedFrames = static_cast<uint64_t>(_historyLength-1);
    }
    
    // Copy the old data over if any available
    if(heightOffset > 0){
      memmove( _spectrumData, &_spectrumData[(drawingDroppedFrames+1) * _fftPoints], heightOffset * _fftPoints * sizeof(double)) ;
    }

    if(drawingDroppedFrames > 0){
      // Fill in zeros data for dropped data
      memset(&_spectrumData[heightOffset * _fftPoints], 0x00, static_cast<int64_t>(drawingDroppedFrames) * _fftPoints * sizeof(double));
    }

    // add the new buffer
    memcpy(&_spectrumData[(_historyLength - 1) * _fftPoints], fftData, _fftPoints*sizeof(double));

  }
}

double* WaterfallData::GetSpectrumDataBuffer()const{
  return _spectrumData;
}

void WaterfallData::SetSpectrumDataBuffer(const double* newData){
  memcpy(_spectrumData, newData, _fftPoints * _historyLength * sizeof(double));
}

int WaterfallData::GetNumLinesToUpdate()const{
  return _numLinesToUpdate;
}

void WaterfallData::SetNumLinesToUpdate(const int newNum){
  _numLinesToUpdate = newNum;
}

void WaterfallData::IncrementNumLinesToUpdate(){
  _numLinesToUpdate++;
}

Waterfall3DData::Waterfall3DData(const double minimumFrequency, const double maximumFrequency, const uint64_t fftPoints, const unsigned int historyExtent):
  WaterfallData(minimumFrequency,  maximumFrequency, fftPoints, historyExtent), Qwt3D::Function(){

  _floorValue = 0.0;
  setMinZ(0.0);
  setMaxZ(200.0);

  // Create the dummy mesh data until _ResizeMesh is called
  data = new double*[1];
  data[0] = new double[1];
  Qwt3D::Function::setMesh(1,1);

  _ResizeMesh();
}

Waterfall3DData::~Waterfall3DData(){
  for ( unsigned i = 0; i < umesh_p; i++){
    delete[] data[i];
  }
  delete[] data;

}

void Waterfall3DData::ResizeData(const double startFreq, const double stopFreq, const uint64_t fftPoints){
  if((fftPoints != GetNumFFTPoints()) ||
     (boundingRect().width() != (stopFreq - startFreq)) ||
     (boundingRect().left() != startFreq)){
    WaterfallData::ResizeData(startFreq, stopFreq, fftPoints);
    _ResizeMesh();
  }
   
  Reset();
}

bool Waterfall3DData::create()
{
  if ((umesh_p<=2) || (vmesh_p<=2) || !plotwidget_p)
    return false;

  // Almost the same as the old create, except that here we store our own data buffer in the class rather than re-creating it each time...

  unsigned i,j;

  /* get the data */
  double dx = (maxu_p - minu_p) / (umesh_p - 1);
  double dy = (maxv_p - minv_p) / (vmesh_p - 1);
  
  for (i = 0; i < umesh_p; ++i) 
    {
      for (j = 0; j < vmesh_p; ++j) 
	{
	  data[i][j] = operator()(minu_p + i*dx, minv_p + j*dy);
	  
	  if (data[i][j] > range_p.maxVertex.z)
	    data[i][j] = range_p.maxVertex.z;
	  else if (data[i][j] < range_p.minVertex.z)
	    data[i][j] = range_p.minVertex.z;
	}
    }
  
  Q_ASSERT(plotwidget_p);
  if (!plotwidget_p)
    {
      fprintf(stderr,"Function: no valid Plot3D Widget assigned");
    }
  else
    {
      ((Waterfall3DDisplayPlot*)plotwidget_p)->loadFromData(data, umesh_p, vmesh_p, minu_p, maxu_p, minv_p, maxv_p);
    }
  
  return true;
}

double Waterfall3DData::operator()(double x, double y){
  return value(x,y) - _floorValue;
}

double Waterfall3DData::GetFloorValue()const{
  return _floorValue;
}

void Waterfall3DData::SetFloorValue(const double newValue){
  _floorValue = newValue;
}

double Waterfall3DData::minZ()const{
  return range_p.minVertex.z;
}

double Waterfall3DData::maxZ()const{
  return range_p.maxVertex.z;
}

void Waterfall3DData::setMesh(unsigned int, unsigned int){
  // Do Nothing
  printf("Should Not Reach this Function\n");
}

void Waterfall3DData::_ResizeMesh(){
  // Clear out the old mesh
  for ( unsigned i = 0; i < umesh_p; i++){
    delete[] data[i];
  }
  delete[] data;
  
  Qwt3D::Function::setMesh(static_cast<int>(boundingRect().width()/20.0), _historyLength);
  setDomain( boundingRect().left(), static_cast<int>(boundingRect().right()), 0, _historyLength);

  /* allocate some space for the mesh */
  unsigned i;
  data         = new double* [umesh_p] ;
  for ( i = 0; i < umesh_p; i++) 
    {
      data[i]         = new double [vmesh_p];
    }
}

#endif /* WATERFALL_GLOBAL_DATA_CPP */

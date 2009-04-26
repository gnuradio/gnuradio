#include <cmath>
#include <QColorDialog>
#include <QMessageBox>
#include <spectrumdisplayform.h>

int SpectrumDisplayForm::_openGLWaterfall3DFlag = -1;

SpectrumDisplayForm::SpectrumDisplayForm(QWidget* parent)
  : QWidget(parent)
{
  setupUi(this);

  _systemSpecifiedFlag = false;
  _intValidator = new QIntValidator(this);
  _intValidator->setBottom(0);
  _frequencyDisplayPlot = new FrequencyDisplayPlot(FrequencyPlotDisplayFrame);
  _waterfallDisplayPlot = new WaterfallDisplayPlot(WaterfallPlotDisplayFrame);
  _waterfall3DDisplayPlot = new Waterfall3DDisplayPlot(Waterfall3DPlotDisplayFrame);
  _timeDomainDisplayPlot = new TimeDomainDisplayPlot(TimeDomainDisplayFrame);
  _constellationDisplayPlot = new ConstellationDisplayPlot(ConstellationDisplayFrame);
  _numRealDataPoints = 1024;
  _realFFTDataPoints = new double[_numRealDataPoints];
  _averagedValues = new double[_numRealDataPoints];
  _historyVector = new std::vector<double*>;
  
  AvgLineEdit->setValidator(_intValidator);
  PowerLineEdit->setValidator(_intValidator);
  MinHoldCheckBox_toggled( false );
  MaxHoldCheckBox_toggled( false );
  
  WaterfallMaximumIntensityWheel->setRange(-200, 0);
  WaterfallMaximumIntensityWheel->setTickCnt(50);
  WaterfallMinimumIntensityWheel->setRange(-200, 0);
  WaterfallMinimumIntensityWheel->setTickCnt(50);
  WaterfallMinimumIntensityWheel->setValue(-200);
  
  Waterfall3DMaximumIntensityWheel->setRange(-200, 0);
  Waterfall3DMaximumIntensityWheel->setTickCnt(50);
  Waterfall3DMinimumIntensityWheel->setRange(-200, 0);
  Waterfall3DMinimumIntensityWheel->setTickCnt(50);
  Waterfall3DMinimumIntensityWheel->setValue(-200);
  
  _peakFrequency = 0;
  _peakAmplitude = -HUGE_VAL;
  
  _noiseFloorAmplitude = -HUGE_VAL;

  connect(_waterfallDisplayPlot, SIGNAL(UpdatedLowerIntensityLevel(const double)), 
	  _frequencyDisplayPlot, SLOT(SetLowerIntensityLevel(const double)));
  connect(_waterfallDisplayPlot, SIGNAL(UpdatedUpperIntensityLevel(const double)), 
	  _frequencyDisplayPlot, SLOT(SetUpperIntensityLevel(const double)));
  
  _frequencyDisplayPlot->SetLowerIntensityLevel(-200);
  _frequencyDisplayPlot->SetUpperIntensityLevel(-200);

  // Load up the acceptable FFT sizes...
  FFTSizeComboBox->clear();
  for(long fftSize = SpectrumGUIClass::MIN_FFT_SIZE; fftSize <= SpectrumGUIClass::MAX_FFT_SIZE; fftSize *= 2){
    FFTSizeComboBox->insertItem(FFTSizeComboBox->count(), QString("%1").arg(fftSize));
  }
  Reset();

  ToggleTabFrequency(false);
  ToggleTabWaterfall(false);
  ToggleTabWaterfall3D(false);
  ToggleTabTime(false);
  ToggleTabConstellation(false);
}

SpectrumDisplayForm::~SpectrumDisplayForm()
{
  // Qt deletes children when parent is deleted

  // Don't worry about deleting Display Plots - they are deleted when parents are deleted
  /*   delete _intValidator; */

  delete[] _realFFTDataPoints;
  delete[] _averagedValues;

  for(unsigned int count = 0; count < _historyVector->size(); count++){
    delete[] _historyVector->operator[](count);
  }

  delete _historyVector;
}

void
SpectrumDisplayForm::setSystem( SpectrumGUIClass * newSystem, 
				const uint64_t numFFTDataPoints, 
				const uint64_t numTimeDomainDataPoints )
{
  ResizeBuffers(numFFTDataPoints, numTimeDomainDataPoints);
  
  if(newSystem != NULL){
    _system = newSystem;
    _systemSpecifiedFlag = true;
  }
  else{
    _systemSpecifiedFlag = false;
  }
}

void
SpectrumDisplayForm::newFrequencyData( const SpectrumUpdateEvent* spectrumUpdateEvent)
{
  const std::complex<float>* complexDataPoints = spectrumUpdateEvent->getFFTPoints();
  const uint64_t numFFTDataPoints = spectrumUpdateEvent->getNumFFTDataPoints();
  const double* realTimeDomainDataPoints = spectrumUpdateEvent->getRealTimeDomainPoints();
  const double* imagTimeDomainDataPoints = spectrumUpdateEvent->getImagTimeDomainPoints();
  const uint64_t numTimeDomainDataPoints = spectrumUpdateEvent->getNumTimeDomainDataPoints();
  const double timePerFFT = spectrumUpdateEvent->getTimePerFFT();
  const timespec dataTimestamp = spectrumUpdateEvent->getDataTimestamp();;
  const bool repeatDataFlag = spectrumUpdateEvent->getRepeatDataFlag();
  const bool lastOfMultipleUpdatesFlag = spectrumUpdateEvent->getLastOfMultipleUpdateFlag();
  const timespec generatedTimestamp = spectrumUpdateEvent->getEventGeneratedTimestamp();

  // REMEMBER: The dataTimestamp is NOT valid when the repeat data flag is true...
  ResizeBuffers(numFFTDataPoints, numTimeDomainDataPoints);

  // Calculate the Magnitude of the complex point
  const std::complex<float>* complexDataPointsPtr = complexDataPoints;
  double* realFFTDataPointsPtr = _realFFTDataPoints;
  for(uint64_t point = 0; point < numFFTDataPoints; point++){
    // Calculate dBm
    // 50 ohm load assumption
    // 10 * log10 (v^2 / (2 * 50.0 * .001)) = 10 * log10( v^2 * 10)
    // 75 ohm load assumption
    // 10 * log10 (v^2 / (2 * 75.0 * .001)) = 10 * log10( v^2 * 15)
    
    *realFFTDataPointsPtr = 10.0*log10((((*complexDataPointsPtr).real() * (*complexDataPointsPtr).real()) +
					((*complexDataPointsPtr).imag()*(*complexDataPointsPtr).imag())) + 1e-20);

    complexDataPointsPtr++;
    realFFTDataPointsPtr++;
  }
 
  int tabindex = SpectrumTypeTab->currentIndex();

  // Don't update the averaging history if this is repeated data
  if(!repeatDataFlag){
    _AverageHistory(_realFFTDataPoints);

    double sumMean;
    const double fft_bin_size = (_stopFrequency-_startFrequency) /
      static_cast<double>(numFFTDataPoints);

    // find the peak, sum (for mean), etc
    _peakAmplitude = -HUGE_VAL;
    sumMean = 0.0;
    for(uint64_t number = 0; number < numFFTDataPoints; number++){
      // find peak
      if(_realFFTDataPoints[number] > _peakAmplitude){
        _peakFrequency = (static_cast<float>(number) * fft_bin_size);  // Calculate the frequency relative to the local bw, adjust for _startFrequency later
        _peakAmplitude = _realFFTDataPoints[number];
        // _peakBin = number;
      }
      // sum (for mean)
      sumMean += _realFFTDataPoints[number];
    }

    // calculate the spectral mean
    // +20 because for the comparison below we only want to throw out bins
    // that are significantly higher (and would, thus, affect the mean more)
    const double meanAmplitude = (sumMean / numFFTDataPoints) + 20.0;

    // now throw out any bins higher than the mean
    sumMean = 0.0;
    uint64_t newNumDataPoints = numFFTDataPoints;
    for(uint64_t number = 0; number < numFFTDataPoints; number++){
      if (_realFFTDataPoints[number] <= meanAmplitude)
        sumMean += _realFFTDataPoints[number];
      else
        newNumDataPoints--;
    }

    if (newNumDataPoints == 0)             // in the odd case that all
      _noiseFloorAmplitude = meanAmplitude; // amplitudes are equal!
    else
      _noiseFloorAmplitude = sumMean / newNumDataPoints;
  }

  if(lastOfMultipleUpdatesFlag){
    if(tabindex == d_plot_fft) {
      _frequencyDisplayPlot->PlotNewData(_averagedValues, numFFTDataPoints, 
					 _noiseFloorAmplitude, _peakFrequency, 
					 _peakAmplitude);
    }
    if(tabindex == d_plot_time) {
      _timeDomainDisplayPlot->PlotNewData(realTimeDomainDataPoints, 
					  imagTimeDomainDataPoints, 
					  numTimeDomainDataPoints);
    }
    if(tabindex == d_plot_constellation) {
      _constellationDisplayPlot->PlotNewData(realTimeDomainDataPoints, 
					     imagTimeDomainDataPoints, 
					     numTimeDomainDataPoints);
    }

    // Don't update the repeated data for the waterfall
    if(!repeatDataFlag){
      if(tabindex == d_plot_waterfall) {
	_waterfallDisplayPlot->PlotNewData(_realFFTDataPoints, numFFTDataPoints, 
					   timePerFFT, dataTimestamp, 
					   spectrumUpdateEvent->getDroppedFFTFrames());
      }
      if( _openGLWaterfall3DFlag == 1 && (tabindex == d_plot_waterfall3d)) {
	_waterfall3DDisplayPlot->PlotNewData(_realFFTDataPoints, numFFTDataPoints, 
					     timePerFFT, dataTimestamp, 
					     spectrumUpdateEvent->getDroppedFFTFrames());
      }
    }

    // Tell the system the GUI has been updated
    if(_systemSpecifiedFlag){
      _system->SetLastGUIUpdateTime(generatedTimestamp);
      _system->DecrementPendingGUIUpdateEvents();
    }
  }
}

void
SpectrumDisplayForm::resizeEvent( QResizeEvent *e )
{
  // Let the actual window resize its width, but not its height
  QSize newSize(e->size().width(), e->oldSize().height());
  QResizeEvent et(newSize, e->oldSize());
  QWidget::resizeEvent(&et);

  // Tell the Tab Window to Resize
  SpectrumTypeTab->resize( e->size().width(), e->size().height()-60);

  // Tell the TabXFreqDisplay to resize
  FrequencyPlotDisplayFrame->resize(e->size().width()-4,
				    e->size().height()-140);
  _frequencyDisplayPlot->resize( FrequencyPlotDisplayFrame->width()-4,
				 e->size().height()-140);
  
  // Move the Power Lbl and Line Edit
  PowerLabel->move(e->size().width()-(415-324) - PowerLabel->width(),
		   e->size().height()-135);
  PowerLineEdit->move(e->size().width()-(415-318) - PowerLineEdit->width(),
		      e->size().height()-115);
  
  // Move the Avg Lbl and Line Edit
  AvgLabel->move(e->size().width()-(415-406) - AvgLabel->width(),
		 e->size().height()-135);
  AvgLineEdit->move(e->size().width()-(415-400) - AvgLineEdit->width(),
		    e->size().height()-115);
  
  // Move Max and Min check boxes
  MaxHoldCheckBox->move(MaxHoldCheckBox->x(),
			e->size().height()-135);
  MaxHoldResetBtn->move(MaxHoldResetBtn->x(),
			e->size().height()-135);
  MinHoldCheckBox->move(MinHoldCheckBox->x(),
			e->size().height()-115);
  MinHoldResetBtn->move(MinHoldResetBtn->x(),
			e->size().height()-115);

  WaterfallPlotDisplayFrame->resize(e->size().width()-4,
				    e->size().height()-140);
  _waterfallDisplayPlot->resize( WaterfallPlotDisplayFrame->width()-4,
				 e->size().height()-140);
  
  // Move the IntensityWheels and Labels
  WaterfallMaximumIntensityLabel->move(width() - 5 -
				       WaterfallMaximumIntensityLabel->width(),
				       WaterfallMaximumIntensityLabel->y());
  WaterfallMaximumIntensityWheel->resize(WaterfallMaximumIntensityLabel->x() - 5 -
					 WaterfallMaximumIntensityWheel->x(),
					 WaterfallMaximumIntensityWheel->height());
  
  WaterfallMinimumIntensityLabel->move(width() - 5 -
				       WaterfallMinimumIntensityLabel->width(),
				       height() - 115);
  WaterfallMinimumIntensityWheel->resize(WaterfallMinimumIntensityLabel->x() - 5 -
					 WaterfallMinimumIntensityWheel->x(),
					 WaterfallMaximumIntensityWheel->height());
  WaterfallMinimumIntensityWheel->move(WaterfallMinimumIntensityWheel->x(),
				       height() - 115);
  WaterfallAutoScaleBtn->move(WaterfallAutoScaleBtn->x(),
			      e->size().height()-115);
  
  Waterfall3DPlotDisplayFrame->resize(e->size().width()-4,
				      e->size().height()-140);
  _waterfall3DDisplayPlot->resize( Waterfall3DPlotDisplayFrame->width()-4,
				   e->size().height()-140);
  
  Waterfall3DMaximumIntensityLabel->move(width() - 5 -
					 Waterfall3DMaximumIntensityLabel->width(),
					 Waterfall3DMaximumIntensityLabel->y());
  Waterfall3DMaximumIntensityWheel->resize(Waterfall3DMaximumIntensityLabel->x() - 5 -
					   Waterfall3DMaximumIntensityWheel->x(),
					   Waterfall3DMaximumIntensityWheel->height());
  Waterfall3DMinimumIntensityLabel->move(width() - 5 -
					 Waterfall3DMinimumIntensityLabel->width(),
					 height() - 115);
  Waterfall3DMinimumIntensityWheel->resize(Waterfall3DMinimumIntensityLabel->x() - 5 -
					   Waterfall3DMinimumIntensityWheel->x(),
					   Waterfall3DMaximumIntensityWheel->height());
  Waterfall3DMinimumIntensityWheel->move(Waterfall3DMinimumIntensityWheel->x(),
					 height() - 115);
  Waterfall3DAutoScaleBtn->move(WaterfallAutoScaleBtn->x(),
				e->size().height()-115);
  
  TimeDomainDisplayFrame->resize(e->size().width()-4,
				 e->size().height()-140);
  _timeDomainDisplayPlot->resize( TimeDomainDisplayFrame->width()-4,
				  e->size().height()-140);
  
  ConstellationDisplayFrame->resize(e->size().width()-4,
				    e->size().height()-140);
  _constellationDisplayPlot->resize( TimeDomainDisplayFrame->width()-4,
				     e->size().height()-140);
  
  // Move the FFT Size Combobox and label
  FFTSizeComboBox->move(width() - 5 - FFTSizeComboBox->width(),
			height()-50);
  FFTSizeLabel->move(width() - 10 - FFTSizeComboBox->width() - FFTSizeLabel->width(),
		     height()-50);
  
  // Move the lower check and combo boxes
  UseRFFrequenciesCheckBox->move(UseRFFrequenciesCheckBox->x(), height()-50);
  WindowLbl->move(WindowLbl->x(), height()-25);
  WindowComboBox->move(WindowComboBox->x(), height()-25);
}


void
SpectrumDisplayForm::customEvent( QEvent * e)
{
  if(e->type() == QEvent::User+3){
    if(_systemSpecifiedFlag){
      WindowComboBox->setCurrentIndex(_system->GetWindowType());
      FFTSizeComboBox->setCurrentIndex(_system->GetFFTSizeIndex());
      //FFTSizeComboBox->setCurrentIndex(1);
      PowerLineEdit_textChanged(PowerLineEdit->text());
    }

    waterfallMinimumIntensityChangedCB(WaterfallMinimumIntensityWheel->value());
    waterfallMaximumIntensityChangedCB(WaterfallMaximumIntensityWheel->value());

    waterfall3DMinimumIntensityChangedCB(Waterfall3DMinimumIntensityWheel->value());
    waterfall3DMaximumIntensityChangedCB(Waterfall3DMaximumIntensityWheel->value());

    // If the video card doesn't support OpenGL then don't display the 3D Waterfall
    if(QGLFormat::hasOpenGL()){
      // Check for Hardware Acceleration of the OpenGL
      if(!_waterfall3DDisplayPlot->format().directRendering()){
	// Only ask this once while the program is running...
	if(_openGLWaterfall3DFlag == -1){
	  _openGLWaterfall3DFlag = 0;
	  if(QMessageBox::warning(this, "OpenGL Direct Rendering NOT Supported", "<center>The system's video card hardware or current drivers do not support direct hardware rendering of the OpenGL modules.</center><br><center>Software rendering is VERY processor intensive.</center><br><center>Do you want to use software rendering?</center>", QMessageBox::Yes, QMessageBox::No | QMessageBox::Default | QMessageBox::Escape) == QMessageBox::Yes){
	    _openGLWaterfall3DFlag = 1;
	  }
	}
      }
      else{
	_openGLWaterfall3DFlag = 1;
      }
    }
    
    if(_openGLWaterfall3DFlag != 1){
      ToggleTabWaterfall3D(false);
    }

    // Clear any previous display
    Reset();
  }
  else if(e->type() == 10005){
    SpectrumUpdateEvent* spectrumUpdateEvent = (SpectrumUpdateEvent*)e;
    newFrequencyData(spectrumUpdateEvent);
  }
  else if(e->type() == 10008){
    setWindowTitle(((SpectrumWindowCaptionEvent*)e)->getLabel());
  }
  else if(e->type() == 10009){
    Reset();
    if(_systemSpecifiedFlag){
      _system->ResetPendingGUIUpdateEvents();
    }
  }
  else if(e->type() == 10010){
    _startFrequency = ((SpectrumFrequencyRangeEvent*)e)->GetStartFrequency();
    _stopFrequency = ((SpectrumFrequencyRangeEvent*)e)->GetStopFrequency();
    _centerFrequency  = ((SpectrumFrequencyRangeEvent*)e)->GetCenterFrequency();

    UseRFFrequenciesCB(UseRFFrequenciesCheckBox->isChecked());
  }
}

void
SpectrumDisplayForm::AvgLineEdit_textChanged( const QString &valueString )
{
  if(!valueString.isEmpty()){
    int value = valueString.toInt();
    if(value > 500){
      value = 500;
      AvgLineEdit->setText("500");
    }
    SetAverageCount(value);
  }
}


void
SpectrumDisplayForm::MaxHoldCheckBox_toggled( bool newState )
{
  MaxHoldResetBtn->setEnabled(newState);
  _frequencyDisplayPlot->SetMaxFFTVisible(newState);
  MaxHoldResetBtn_clicked();
}


void
SpectrumDisplayForm::MinHoldCheckBox_toggled( bool newState )
{
  MinHoldResetBtn->setEnabled(newState);
  _frequencyDisplayPlot->SetMinFFTVisible(newState);
  MinHoldResetBtn_clicked();
}


void
SpectrumDisplayForm::MinHoldResetBtn_clicked()
{
  _frequencyDisplayPlot->ClearMinData();
  _frequencyDisplayPlot->replot();
}


void
SpectrumDisplayForm::MaxHoldResetBtn_clicked()
{
  _frequencyDisplayPlot->ClearMaxData();
  _frequencyDisplayPlot->replot();
}


void
SpectrumDisplayForm::PowerLineEdit_textChanged( const QString &valueString )
{
  if(_systemSpecifiedFlag){
    if(!valueString.isEmpty()){
      double value = valueString.toDouble();
      if(value < 1.0){
	value = 1.0;
	PowerLineEdit->setText("1");
      }
      _system->SetPowerValue(value);
    }

    if(_system->GetPowerValue() > 1){
      UseRFFrequenciesCheckBox->setChecked(false);
      UseRFFrequenciesCheckBox->setEnabled(false);
      UseRFFrequenciesCB(false);
    }
    else{
      UseRFFrequenciesCheckBox->setEnabled(true);
    }
  }
}

void
SpectrumDisplayForm::SetFrequencyRange(const double newStartFrequency, 
				       const double newStopFrequency,
				       const double newCenterFrequency)
{
  double fdiff = abs(newStartFrequency - newStopFrequency);

  if(fdiff > 0) {
    std::string strunits[4] = {"Hz", "kHz", "MHz", "GHz"};
    double units10 = floor(log10(fdiff));
    double units3  = floor(units10 / 3.0);
    double units = pow(10, units10);
    int iunit = static_cast<int>(units3);
    
    _frequencyDisplayPlot->SetFrequencyRange(newStartFrequency,
					     newStopFrequency,
					     newCenterFrequency,
					     UseRFFrequenciesCheckBox->isChecked(),
					     units, strunits[iunit]);
    _waterfallDisplayPlot->SetFrequencyRange(newStartFrequency,
					     newStopFrequency,
					     newCenterFrequency,
					     UseRFFrequenciesCheckBox->isChecked(),
					     units, strunits[iunit]);
    _waterfall3DDisplayPlot->SetFrequencyRange(newStartFrequency,
					       newStopFrequency,
					       newCenterFrequency,
					       UseRFFrequenciesCheckBox->isChecked(),
					       units, strunits[iunit]);
  }
}

int
SpectrumDisplayForm::GetAverageCount()
{
  return _historyVector->size();
}

void
SpectrumDisplayForm::SetAverageCount(const int newCount)
{
  if(newCount > -1){
    if(newCount != static_cast<int>(_historyVector->size())){
      std::vector<double*>::iterator pos;
      while(newCount < static_cast<int>(_historyVector->size())){
	pos = _historyVector->begin();
	delete[] (*pos);
	_historyVector->erase(pos);
      }

      while(newCount > static_cast<int>(_historyVector->size())){
	_historyVector->push_back(new double[_numRealDataPoints]);
      }
      AverageDataReset();
    }
  }
}

void
SpectrumDisplayForm::_AverageHistory(const double* newBuffer)
{
  if(_numRealDataPoints > 0){
    if(_historyVector->size() > 0){
      memcpy(_historyVector->operator[](_historyEntry), newBuffer,
	     _numRealDataPoints*sizeof(double));

      // Increment the next location to store data
      _historyEntryCount++;
      if(_historyEntryCount > static_cast<int>(_historyVector->size())){
	_historyEntryCount = _historyVector->size();
      }
      _historyEntry = (++_historyEntry)%_historyVector->size();

      // Total up and then average the values
      double sum;
      for(uint64_t location = 0; location < _numRealDataPoints; location++){
	sum = 0;
	for(int number = 0; number < _historyEntryCount; number++){
	  sum += _historyVector->operator[](number)[location];
	}
 	_averagedValues[location] = sum/static_cast<double>(_historyEntryCount);
      }
    }
    else{
      memcpy(_averagedValues, newBuffer, _numRealDataPoints*sizeof(double));
    }
  }
}

void
SpectrumDisplayForm::ResizeBuffers( const uint64_t numFFTDataPoints,
				    const uint64_t /*numTimeDomainDataPoints*/ )
{
  // Convert from Complex to Real for certain Displays
  if(_numRealDataPoints != numFFTDataPoints){
    _numRealDataPoints = numFFTDataPoints;
    delete[] _realFFTDataPoints;
    delete[] _averagedValues;
    
    _realFFTDataPoints = new double[_numRealDataPoints];
    _averagedValues = new double[_numRealDataPoints];
    memset(_realFFTDataPoints, 0x0, _numRealDataPoints*sizeof(double));
    
    const int historySize = _historyVector->size();
    SetAverageCount(0); // Clear the existing history
    SetAverageCount(historySize);
    
    Reset();
  }
}

void
SpectrumDisplayForm::Reset()
{
  AverageDataReset();

  _waterfallDisplayPlot->Reset();
  _waterfall3DDisplayPlot->Reset();
}


void
SpectrumDisplayForm::AverageDataReset()
{
  _historyEntry = 0;
  _historyEntryCount = 0;

  memset(_averagedValues, 0x0, _numRealDataPoints*sizeof(double));

  MaxHoldResetBtn_clicked();
  MinHoldResetBtn_clicked();
}


void
SpectrumDisplayForm::closeEvent( QCloseEvent *e )
{
  if(_systemSpecifiedFlag){
    _system->SetWindowOpenFlag(false);
  }

  qApp->processEvents();

  QWidget::closeEvent(e);
}


void
SpectrumDisplayForm::WindowTypeChanged( int newItem )
{
  if(_systemSpecifiedFlag){
   _system->SetWindowType(newItem);
  }
}


void
SpectrumDisplayForm::UseRFFrequenciesCB( bool useRFFlag )
{
  if(useRFFlag){
    SetFrequencyRange(_startFrequency, _stopFrequency, _centerFrequency);
  }
  else{
    SetFrequencyRange(_startFrequency, _stopFrequency, 0.0 );
  }
}


void
SpectrumDisplayForm::waterfallMaximumIntensityChangedCB( double newValue )
{
  if(newValue > WaterfallMinimumIntensityWheel->value()){
    WaterfallMaximumIntensityLabel->setText(QString("%1 dB").arg(newValue, 0, 'f', 0));
  }
  else{
    WaterfallMaximumIntensityWheel->setValue(WaterfallMinimumIntensityWheel->value());
  }

  _waterfallDisplayPlot->SetIntensityRange(WaterfallMinimumIntensityWheel->value(),
					   WaterfallMaximumIntensityWheel->value());
}


void
SpectrumDisplayForm::waterfallMinimumIntensityChangedCB( double newValue )
{
  if(newValue < WaterfallMaximumIntensityWheel->value()){
    WaterfallMinimumIntensityLabel->setText(QString("%1 dB").arg(newValue, 0, 'f', 0));
  }
  else{
    WaterfallMinimumIntensityWheel->setValue(WaterfallMaximumIntensityWheel->value());
  }

  _waterfallDisplayPlot->SetIntensityRange(WaterfallMinimumIntensityWheel->value(),
					   WaterfallMaximumIntensityWheel->value());
}

void
SpectrumDisplayForm::waterfall3DMaximumIntensityChangedCB( double newValue )
{
  if(newValue > Waterfall3DMinimumIntensityWheel->value()){
    Waterfall3DMaximumIntensityLabel->setText(QString("%1 dB").arg(newValue, 0, 'f', 0));
  }
  else{
    Waterfall3DMaximumIntensityWheel->setValue(Waterfall3DMinimumIntensityWheel->value());
  }

  _waterfall3DDisplayPlot->SetIntensityRange(Waterfall3DMinimumIntensityWheel->value(),
					     Waterfall3DMaximumIntensityWheel->value());
}


void
SpectrumDisplayForm::waterfall3DMinimumIntensityChangedCB( double newValue )
{
  if(newValue < Waterfall3DMaximumIntensityWheel->value()){
    Waterfall3DMinimumIntensityLabel->setText(QString("%1 dB").arg(newValue, 0, 'f', 0));
  }
  else{
    Waterfall3DMinimumIntensityWheel->setValue(Waterfall3DMaximumIntensityWheel->value());
  }

  _waterfall3DDisplayPlot->SetIntensityRange(Waterfall3DMinimumIntensityWheel->value(),
					     Waterfall3DMaximumIntensityWheel->value());
}


void
SpectrumDisplayForm::FFTComboBoxSelectedCB( const QString &fftSizeString )
{
  if(_systemSpecifiedFlag){
    _system->SetFFTSize(fftSizeString.toLong());
  }
}


void
SpectrumDisplayForm::WaterfallAutoScaleBtnCB()
{
  double minimumIntensity = _noiseFloorAmplitude - 5;
  if(minimumIntensity < WaterfallMinimumIntensityWheel->minValue()){
    minimumIntensity = WaterfallMinimumIntensityWheel->minValue();
  }
  WaterfallMinimumIntensityWheel->setValue(minimumIntensity);
  double maximumIntensity = _peakAmplitude + 10;
  if(maximumIntensity > WaterfallMaximumIntensityWheel->maxValue()){
    maximumIntensity = WaterfallMaximumIntensityWheel->maxValue();
  }
  WaterfallMaximumIntensityWheel->setValue(maximumIntensity);
  waterfallMaximumIntensityChangedCB(maximumIntensity);
}

void
SpectrumDisplayForm::Waterfall3DAutoScaleBtnCB()
{
  double minimumIntensity = _noiseFloorAmplitude - 5;
  if(minimumIntensity < Waterfall3DMinimumIntensityWheel->minValue()){
    minimumIntensity = Waterfall3DMinimumIntensityWheel->minValue();
  }
  Waterfall3DMinimumIntensityWheel->setValue(minimumIntensity);
  double maximumIntensity = _peakAmplitude + 10;
  if(maximumIntensity > Waterfall3DMaximumIntensityWheel->maxValue()){
    maximumIntensity = Waterfall3DMaximumIntensityWheel->maxValue();
  }
  Waterfall3DMaximumIntensityWheel->setValue(maximumIntensity);
  waterfallMaximumIntensityChangedCB(maximumIntensity);
}

void
SpectrumDisplayForm::WaterfallIntensityColorTypeChanged( int newType )
{
  QColor lowIntensityColor;
  QColor highIntensityColor;
  if(newType == WaterfallDisplayPlot::INTENSITY_COLOR_MAP_TYPE_USER_DEFINED){
    // Select the Low Intensity Color
    lowIntensityColor = _waterfallDisplayPlot->GetUserDefinedLowIntensityColor();
    if(!lowIntensityColor.isValid()){
      lowIntensityColor = Qt::black;
    }
    QMessageBox::information(this, "Low Intensity Color Selection", "In the next window, select the low intensity color for the waterfall display",  QMessageBox::Ok);
    lowIntensityColor = QColorDialog::getColor(lowIntensityColor, this);
    
    // Select the High Intensity Color
    highIntensityColor = _waterfallDisplayPlot->GetUserDefinedHighIntensityColor();
    if(!highIntensityColor.isValid()){
      highIntensityColor = Qt::white;
    }
    QMessageBox::information(this, "High Intensity Color Selection", "In the next window, select the high intensity color for the waterfall display",  QMessageBox::Ok);
    highIntensityColor = QColorDialog::getColor(highIntensityColor, this);
  }
  
  _waterfallDisplayPlot->SetIntensityColorMapType(newType, lowIntensityColor, highIntensityColor);
}

void
SpectrumDisplayForm::Waterfall3DIntensityColorTypeChanged( int newType )
{
  QColor lowIntensityColor;
  QColor highIntensityColor;
  if(newType == Waterfall3DDisplayPlot::INTENSITY_COLOR_MAP_TYPE_USER_DEFINED){
    // Select the Low Intensity Color
    lowIntensityColor = _waterfallDisplayPlot->GetUserDefinedLowIntensityColor();
    if(!lowIntensityColor.isValid()){
      lowIntensityColor = Qt::black;
    }
    QMessageBox::information(this, "Low Intensity Color Selection", "In the next window, select the low intensity color for the waterfall display",  QMessageBox::Ok);
    lowIntensityColor = QColorDialog::getColor(lowIntensityColor, this);
    
    // Select the High Intensity Color
    highIntensityColor = _waterfallDisplayPlot->GetUserDefinedHighIntensityColor();
    if(!highIntensityColor.isValid()){
      highIntensityColor = Qt::white;
    }
    QMessageBox::information(this, "High Intensity Color Selection", "In the next window, select the high intensity color for the waterfall display",  QMessageBox::Ok);
    highIntensityColor = QColorDialog::getColor(highIntensityColor, this);
  }
  _waterfall3DDisplayPlot->SetIntensityColorMapType(newType, lowIntensityColor,
						    highIntensityColor);
}


void
SpectrumDisplayForm::ToggleTabFrequency(const bool state)
{
  if(state == true) {
    if(d_plot_fft == -1) {
      SpectrumTypeTab->addTab(FrequencyPage, "Frequency Display");
      d_plot_fft = SpectrumTypeTab->count()-1;
    }
  }
  else {
    SpectrumTypeTab->removeTab(SpectrumTypeTab->indexOf(FrequencyPage));
    d_plot_fft = -1;
  }
}

void
SpectrumDisplayForm::ToggleTabWaterfall(const bool state)
{
  if(state == true) {
    if(d_plot_waterfall == -1) {
      SpectrumTypeTab->addTab(WaterfallPage, "Waterfall Display");
      d_plot_waterfall = SpectrumTypeTab->count()-1;
    }
  }
  else {
    SpectrumTypeTab->removeTab(SpectrumTypeTab->indexOf(WaterfallPage));
    d_plot_waterfall = -1;
  }
}

void
SpectrumDisplayForm::ToggleTabWaterfall3D(const bool state)
{
  if(state == true) {
    if(d_plot_waterfall3d == -1) {
      SpectrumTypeTab->addTab(Waterfall3DPage, "3D Waterfall Display");
      d_plot_waterfall3d = SpectrumTypeTab->count()-1;
    }
  }
  else {
    SpectrumTypeTab->removeTab(SpectrumTypeTab->indexOf(Waterfall3DPage));
    d_plot_waterfall3d = -1;
  }
}

void
SpectrumDisplayForm::ToggleTabTime(const bool state)
{
  if(state == true) {
    if(d_plot_time == -1) {
      SpectrumTypeTab->addTab(TimeDomainPage, "Time Domain Display");
      d_plot_time = SpectrumTypeTab->count()-1;
    }
  }
  else {
    SpectrumTypeTab->removeTab(SpectrumTypeTab->indexOf(TimeDomainPage));
    d_plot_time = -1;
  }
}

void
SpectrumDisplayForm::ToggleTabConstellation(const bool state)
{
  if(state == true) {
    if(d_plot_constellation == -1) {
      SpectrumTypeTab->addTab(ConstellationPage, "Constellation Display");
      d_plot_constellation = SpectrumTypeTab->count()-1;
    }
  }
  else {
    SpectrumTypeTab->removeTab(SpectrumTypeTab->indexOf(ConstellationPage));
    d_plot_constellation = -1;
  }
}


void
SpectrumDisplayForm::SetTimeDomainAxis(double min, double max)
{
  _timeDomainDisplayPlot->set_yaxis(min, max);
}

void
SpectrumDisplayForm::SetConstellationAxis(double xmin, double xmax,
						double ymin, double ymax)
{
  _constellationDisplayPlot->set_axis(xmin, xmax, ymin, ymax);
}

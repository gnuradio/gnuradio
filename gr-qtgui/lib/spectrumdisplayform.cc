/* -*- c++ -*- */
/*
 * Copyright 2008-2011 Free Software Foundation, Inc.
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

#include <cmath>
#include <QColorDialog>
#include <QMessageBox>
#include <gnuradio/qtgui/spectrumdisplayform.h>
#include <gnuradio/qtgui/qtgui_types.h>

SpectrumDisplayForm::SpectrumDisplayForm(QWidget* parent)
  : QWidget(parent)
{
  setupUi(this);

  _systemSpecifiedFlag = false;
  _intValidator = new QIntValidator(this);
  _intValidator->setBottom(0);
  _frequencyDisplayPlot = new FrequencyDisplayPlot(1, FrequencyPlotDisplayFrame);
  _waterfallDisplayPlot = new WaterfallDisplayPlot(1, WaterfallPlotDisplayFrame);
  _timeDomainDisplayPlot = new TimeDomainDisplayPlot(2, TimeDomainDisplayFrame);
  _constellationDisplayPlot = new ConstellationDisplayPlot(1, ConstellationDisplayFrame);
  _numRealDataPoints = 1024;
  _realFFTDataPoints = new double[_numRealDataPoints];
  _averagedValues = new double[_numRealDataPoints];
  _historyVector = new std::vector<double*>;

  _timeDomainDisplayPlot->setLineLabel(0, "real");
  _timeDomainDisplayPlot->setLineLabel(1, "imag");

  AvgLineEdit->setRange(0, 500);                 // Set range of Average box value from 0 to 500
  minHoldCheckBox_toggled( false );
  maxHoldCheckBox_toggled( false );

#if QWT_VERSION < 0x060100
  WaterfallMaximumIntensitySlider->setRange(-200, 0);
  WaterfallMinimumIntensitySlider->setRange(-200, 0);
  WaterfallMinimumIntensitySlider->setValue(-200);
#else /* QWT_VERSION < 0x060100 */
  WaterfallMaximumIntensitySlider->setScale(-200, 0);
  WaterfallMinimumIntensitySlider->setScale(-200, 0);
  WaterfallMinimumIntensitySlider->setValue(-200);
#endif /* QWT_VERSION < 0x060100 */

  WaterfallMaximumIntensitySlider->setOrientation(Qt::Horizontal);
  WaterfallMinimumIntensitySlider->setOrientation(Qt::Horizontal);

  _peakFrequency = 0;
  _peakAmplitude = -HUGE_VAL;

  _noiseFloorAmplitude = -HUGE_VAL;

  connect(_waterfallDisplayPlot, SIGNAL(updatedLowerIntensityLevel(const double)),
  	  _frequencyDisplayPlot, SLOT(setLowerIntensityLevel(const double)));
  connect(_waterfallDisplayPlot, SIGNAL(updatedUpperIntensityLevel(const double)),
   	  _frequencyDisplayPlot, SLOT(setUpperIntensityLevel(const double)));

  _frequencyDisplayPlot->setLowerIntensityLevel(-200);
  _frequencyDisplayPlot->setUpperIntensityLevel(-200);

  // Load up the acceptable FFT sizes...
  FFTSizeComboBox->clear();
  for(long fftSize = SpectrumGUIClass::MIN_FFT_SIZE; fftSize <= SpectrumGUIClass::MAX_FFT_SIZE; fftSize *= 2){
    FFTSizeComboBox->insertItem(FFTSizeComboBox->count(), QString("%1").arg(fftSize));
  }
  reset();

  toggleTabFrequency(false);
  toggleTabWaterfall(false);
  toggleTabTime(false);
  toggleTabConstellation(false);

  _historyEntry = 0;
  _historyEntryCount = 0;

  // Create a timer to update plots at the specified rate
  displayTimer = new QTimer(this);
  connect(displayTimer, SIGNAL(timeout()), this, SLOT(updateGuiTimer()));

  // Connect double click signals up
  connect(_frequencyDisplayPlot, SIGNAL(plotPointSelected(const QPointF)),
  	  this, SLOT(onFFTPlotPointSelected(const QPointF)));

  connect(_waterfallDisplayPlot, SIGNAL(plotPointSelected(const QPointF)),
  	  this, SLOT(onWFallPlotPointSelected(const QPointF)));

  connect(_timeDomainDisplayPlot, SIGNAL(plotPointSelected(const QPointF)),
  	  this, SLOT(onTimePlotPointSelected(const QPointF)));

  connect(_constellationDisplayPlot, SIGNAL(plotPointSelected(const QPointF)),
  	  this, SLOT(onConstPlotPointSelected(const QPointF)));
}

SpectrumDisplayForm::~SpectrumDisplayForm()
{
  // Qt deletes children when parent is deleted

  // Don't worry about deleting Display Plots - they are deleted when parents are deleted
  delete _intValidator;

  delete[] _realFFTDataPoints;
  delete[] _averagedValues;

  for(unsigned int count = 0; count < _historyVector->size(); count++){
    delete[] _historyVector->operator[](count);
  }

  delete _historyVector;

  displayTimer->stop();
  delete displayTimer;
}

void
SpectrumDisplayForm::setSystem(SpectrumGUIClass * newSystem,
			       const uint64_t numFFTDataPoints,
			       const uint64_t numTimeDomainDataPoints)
{
  resizeBuffers(numFFTDataPoints, numTimeDomainDataPoints);

  if(newSystem != NULL) {
    _system = newSystem;
    _systemSpecifiedFlag = true;
  }
  else {
    _systemSpecifiedFlag = false;
  }
}

/***********************************************************************
 * This is kind of gross because we're combining three operations:
 * Conversion from float to double (which is what the plotter wants)
 * Finding the peak and mean
 * Doing the "FFT shift" to put 0Hz at the center of the plot
 * I feel like this might want to be part of the sink block
 **********************************************************************/
static void fftshift_and_sum(double *outFFT, const float *inFFT,
			     uint64_t num_points, double &sum_mean,
			     double &peak_ampl, int &peak_bin)
{
  const float* inptr = inFFT+num_points/2;
  double* outptr = outFFT;

  sum_mean = 0;
  peak_ampl = -HUGE_VAL;
  peak_bin = 0;

  // Run this twice to perform the fftshift operation on the data here as well
  for(uint64_t point = 0; point < num_points/2; point++) {
    float pt = (*inptr);
    *outptr = pt;
    if(*outptr > peak_ampl) {
      peak_bin = point;
      peak_ampl = *outptr;
    }
    sum_mean += *outptr;

    inptr++;
    outptr++;
  }

  // This loop takes the first half of the input data and puts it in the
  // second half of the plotted data
  inptr = inFFT;
  for(uint64_t point = 0; point < num_points/2; point++) {
    float pt = (*inptr);
    *outptr = pt;
    if(*outptr > peak_ampl) {
      peak_bin = point;
      peak_ampl = *outptr;
    }
    sum_mean += *outptr;

    inptr++;
    outptr++;
  }
}

void
SpectrumDisplayForm::newFrequencyData( const SpectrumUpdateEvent* spectrumUpdateEvent)
{
  //_lastSpectrumEvent = (SpectrumUpdateEvent)(*spectrumUpdateEvent);
  const float* fftMagDataPoints = spectrumUpdateEvent->getFFTPoints();
  const uint64_t numFFTDataPoints = spectrumUpdateEvent->getNumFFTDataPoints();
  const uint64_t numTimeDomainDataPoints = spectrumUpdateEvent->getNumTimeDomainDataPoints();
  const gr::high_res_timer_type dataTimestamp = spectrumUpdateEvent->getDataTimestamp();
  const bool repeatDataFlag = spectrumUpdateEvent->getRepeatDataFlag();
  const bool lastOfMultipleUpdatesFlag = spectrumUpdateEvent->getLastOfMultipleUpdateFlag();
  const gr::high_res_timer_type generatedTimestamp = spectrumUpdateEvent->getEventGeneratedTimestamp();
  double* realTimeDomainDataPoints = (double*)spectrumUpdateEvent->getRealTimeDomainPoints();
  double* imagTimeDomainDataPoints = (double*)spectrumUpdateEvent->getImagTimeDomainPoints();

  std::vector<double*> timeDomainDataPoints;
  timeDomainDataPoints.push_back(realTimeDomainDataPoints);
  timeDomainDataPoints.push_back(imagTimeDomainDataPoints);

  // REMEMBER: The dataTimestamp is NOT valid when the repeat data flag is true...
  resizeBuffers(numFFTDataPoints, numTimeDomainDataPoints);

  const double fftBinSize = (_stopFrequency-_startFrequency) /
    static_cast<double>(numFFTDataPoints);

  //this does the fftshift, conversion to double, and calculation of sum, peak amplitude, peak freq.
  double sum_mean, peak_ampl;
  int peak_bin;
  fftshift_and_sum(_realFFTDataPoints, fftMagDataPoints, numFFTDataPoints, sum_mean, peak_ampl, peak_bin);
  double peak_freq = peak_bin * fftBinSize;

  // Don't update the averaging history if this is repeated data
  if(!repeatDataFlag){
    _averageHistory(_realFFTDataPoints);

    // Only use the local info if we are not repeating data
    _peakAmplitude = peak_ampl;
    _peakFrequency = peak_freq;

    // calculate the spectral mean
    // +20 because for the comparison below we only want to throw out bins
    // that are significantly higher (and would, thus, affect the mean more)
    const double meanAmplitude = (sum_mean / numFFTDataPoints) + 20.0;

    // now throw out any bins higher than the mean
    sum_mean = 0.0;
    uint64_t newNumDataPoints = numFFTDataPoints;
    for(uint64_t number = 0; number < numFFTDataPoints; number++){
      if (_realFFTDataPoints[number] <= meanAmplitude)
        sum_mean += _realFFTDataPoints[number];
      else
        newNumDataPoints--;
    }

    if (newNumDataPoints == 0)             // in the odd case that all
      _noiseFloorAmplitude = meanAmplitude; // amplitudes are equal!
    else
      _noiseFloorAmplitude = sum_mean / newNumDataPoints;
  }

  if(lastOfMultipleUpdatesFlag){
    int tabindex = SpectrumTypeTab->currentIndex();
    if(tabindex == d_plot_fft) {
      _frequencyDisplayPlot->plotNewData(_averagedValues, numFFTDataPoints,
					 _noiseFloorAmplitude, _peakFrequency,
					 _peakAmplitude, d_update_time);
    }
    if(tabindex == d_plot_time) {
      _timeDomainDisplayPlot->plotNewData(timeDomainDataPoints,
					  numTimeDomainDataPoints,
					  d_update_time);
    }
    if(tabindex == d_plot_constellation) {
      _constellationDisplayPlot->plotNewData(realTimeDomainDataPoints,
					     imagTimeDomainDataPoints,
					     numTimeDomainDataPoints,
					     d_update_time);
    }

    // Don't update the repeated data for the waterfall
    if(!repeatDataFlag){
      if(tabindex == d_plot_waterfall) {
	_waterfallDisplayPlot->plotNewData(_realFFTDataPoints, numFFTDataPoints,
					   d_update_time, dataTimestamp,
					   spectrumUpdateEvent->getDroppedFFTFrames());
      }
    }

    // Tell the system the GUI has been updated
    if(_systemSpecifiedFlag){
      _system->setLastGUIUpdateTime(generatedTimestamp);
      _system->decrementPendingGUIUpdateEvents();
    }
  }
}

void
SpectrumDisplayForm::resizeEvent( QResizeEvent *e )
{
  QSize s;
  s.setWidth(FrequencyPlotDisplayFrame->width());
  s.setHeight(FrequencyPlotDisplayFrame->height());
  emit _frequencyDisplayPlot->resizeSlot(&s);

  s.setWidth(TimeDomainDisplayFrame->width());
  s.setHeight(TimeDomainDisplayFrame->height());
  emit _timeDomainDisplayPlot->resizeSlot(&s);

  s.setWidth(WaterfallPlotDisplayFrame->width());
  s.setHeight(WaterfallPlotDisplayFrame->height());
  emit _waterfallDisplayPlot->resizeSlot(&s);

  s.setWidth(ConstellationDisplayFrame->width());
  s.setHeight(ConstellationDisplayFrame->height());
  emit _constellationDisplayPlot->resizeSlot(&s);
}

void
SpectrumDisplayForm::customEvent( QEvent * e)
{
  if(e->type() == QEvent::User+3){
    if(_systemSpecifiedFlag){
      WindowComboBox->setCurrentIndex(_system->getWindowType());
      FFTSizeComboBox->setCurrentIndex(_system->getFFTSizeIndex());
    }

    waterfallMinimumIntensityChangedCB(WaterfallMinimumIntensitySlider->value());
    waterfallMaximumIntensityChangedCB(WaterfallMaximumIntensitySlider->value());

    // Clear any previous display
    reset();
  }
  else if(e->type() == SpectrumUpdateEventType) {
    SpectrumUpdateEvent* spectrumUpdateEvent = (SpectrumUpdateEvent*)e;
    newFrequencyData(spectrumUpdateEvent);
  }
  else if(e->type() == SpectrumWindowCaptionEventType) {
    setWindowTitle(((SpectrumWindowCaptionEvent*)e)->getLabel());
  }
  else if(e->type() == SpectrumWindowResetEventType) {
    reset();
    if(_systemSpecifiedFlag){
      _system->resetPendingGUIUpdateEvents();
    }
  }
  else if(e->type() == SpectrumFrequencyRangeEventType) {
    _startFrequency = ((SpectrumFrequencyRangeEvent*)e)->GetStartFrequency();
    _stopFrequency = ((SpectrumFrequencyRangeEvent*)e)->GetStopFrequency();
    _centerFrequency  = ((SpectrumFrequencyRangeEvent*)e)->GetCenterFrequency();

    useRFFrequenciesCB(UseRFFrequenciesCheckBox->isChecked());
  }
}

void
SpectrumDisplayForm::updateGuiTimer()
{
  // This is called by the displayTimer and redraws the canvases of
  // all of the plots.
  _frequencyDisplayPlot->canvas()->update();
  _waterfallDisplayPlot->canvas()->update();
  _timeDomainDisplayPlot->canvas()->update();
  _constellationDisplayPlot->canvas()->update();
}


void
SpectrumDisplayForm::avgLineEdit_valueChanged( int value )
{
  setAverageCount(value);
}


void
SpectrumDisplayForm::maxHoldCheckBox_toggled( bool newState )
{
  MaxHoldResetBtn->setEnabled(newState);
  _frequencyDisplayPlot->setMaxFFTVisible(newState);
  maxHoldResetBtn_clicked();
}


void
SpectrumDisplayForm::minHoldCheckBox_toggled( bool newState )
{
  MinHoldResetBtn->setEnabled(newState);
  _frequencyDisplayPlot->setMinFFTVisible(newState);
  minHoldResetBtn_clicked();
}


void
SpectrumDisplayForm::minHoldResetBtn_clicked()
{
  _frequencyDisplayPlot->clearMinData();
  _frequencyDisplayPlot->replot();
}


void
SpectrumDisplayForm::maxHoldResetBtn_clicked()
{
  _frequencyDisplayPlot->clearMaxData();
  _frequencyDisplayPlot->replot();
}


void
SpectrumDisplayForm::tabChanged(int index)
{
  // This might be dangerous to call this with NULL
  resizeEvent(NULL);
}

void
SpectrumDisplayForm::setFrequencyRange(const double newCenterFrequency,
				       const double newStartFrequency,
				       const double newStopFrequency)
{
  double fcenter;
  if(UseRFFrequenciesCheckBox->isChecked()) {
    fcenter = newCenterFrequency;
  }
  else {
    fcenter = 0;
  }

  double fdiff = std::max(fabs(newStartFrequency), fabs(newStopFrequency));
  if(fdiff > 0) {
    std::string strunits[4] = {"Hz", "kHz", "MHz", "GHz"};
    std::string strtime[4] = {"sec", "ms", "us", "ns"};
    double units10 = floor(log10(fdiff));
    double units3  = std::max(floor(units10 / 3.0), 0.0);
    double units = pow(10, (units10-fmod(units10, 3.0)));
    int iunit = static_cast<int>(units3);

    _startFrequency = newStartFrequency;
    _stopFrequency = newStopFrequency;
    _centerFrequency = newCenterFrequency;

    _frequencyDisplayPlot->setFrequencyRange(fcenter, fdiff,
					     units, strunits[iunit]);
    _waterfallDisplayPlot->setFrequencyRange(fcenter, fdiff,
					     units, strunits[iunit]);
    _timeDomainDisplayPlot->setSampleRate((_stopFrequency - _startFrequency)/2.0,
					  units, strtime[iunit]);
  }
}

int
SpectrumDisplayForm::getAverageCount()
{
  return _historyVector->size();
}

void
SpectrumDisplayForm::setAverageCount(const int newCount)
{
  if(newCount > -1) {
    if(newCount != static_cast<int>(_historyVector->size())) {
      std::vector<double*>::iterator pos;
      while(newCount < static_cast<int>(_historyVector->size())) {
	pos = _historyVector->begin();
	delete[] (*pos);
	_historyVector->erase(pos);
      }

      while(newCount > static_cast<int>(_historyVector->size())) {
	_historyVector->push_back(new double[_numRealDataPoints]);
      }
      averageDataReset();
    }
  }
}

void
SpectrumDisplayForm::_averageHistory(const double* newBuffer)
{
  if(_numRealDataPoints > 0) {
    if(_historyVector->size() > 0) {
      memcpy(_historyVector->operator[](_historyEntry), newBuffer,
	     _numRealDataPoints*sizeof(double));

      // Increment the next location to store data
      _historyEntryCount++;
      if(_historyEntryCount > static_cast<int>(_historyVector->size())) {
	_historyEntryCount = _historyVector->size();
      }
      _historyEntry += 1;
      _historyEntry = _historyEntry % _historyVector->size();

      // Total up and then average the values
      double sum;
      for(uint64_t location = 0; location < _numRealDataPoints; location++) {
	sum = 0;
	for(int number = 0; number < _historyEntryCount; number++) {
	  sum += _historyVector->operator[](number)[location];
	}
 	_averagedValues[location] = sum/static_cast<double>(_historyEntryCount);
      }
    }
    else {
      memcpy(_averagedValues, newBuffer, _numRealDataPoints*sizeof(double));
    }
  }
}

void
SpectrumDisplayForm::resizeBuffers(const uint64_t numFFTDataPoints,
				   const uint64_t /*numTimeDomainDataPoints*/)
{
  // Convert from Complex to Real for certain Displays
  if(_numRealDataPoints != numFFTDataPoints) {
    _numRealDataPoints = numFFTDataPoints;
    delete[] _realFFTDataPoints;
    delete[] _averagedValues;

    _realFFTDataPoints = new double[_numRealDataPoints];
    _averagedValues = new double[_numRealDataPoints];
    memset(_realFFTDataPoints, 0x0, _numRealDataPoints*sizeof(double));

    const int historySize = _historyVector->size();
    setAverageCount(0); // Clear the existing history
    setAverageCount(historySize);

    reset();
  }
}

void
SpectrumDisplayForm::reset()
{
  averageDataReset();

  _waterfallDisplayPlot->resetAxis();
}


void
SpectrumDisplayForm::averageDataReset()
{
  _historyEntry = 0;
  _historyEntryCount = 0;

  memset(_averagedValues, 0x0, _numRealDataPoints*sizeof(double));

  maxHoldResetBtn_clicked();
  minHoldResetBtn_clicked();
}


void
SpectrumDisplayForm::closeEvent(QCloseEvent *e)
{
  if(_systemSpecifiedFlag){
    _system->setWindowOpenFlag(false);
  }

  qApp->processEvents();

  QWidget::closeEvent(e); //equivalent to e->accept()
}


void
SpectrumDisplayForm::windowTypeChanged(int newItem)
{
  if(_systemSpecifiedFlag) {
   _system->setWindowType(newItem);
  }
}


void
SpectrumDisplayForm::useRFFrequenciesCB(bool useRFFlag)
{
  setFrequencyRange(_centerFrequency, _startFrequency, _stopFrequency);
}


void
SpectrumDisplayForm::waterfallMaximumIntensityChangedCB(double newValue)
{
  if(newValue > WaterfallMinimumIntensitySlider->value()) {
    WaterfallMaximumIntensityLabel->setText(QString("%1 dB").arg(newValue, 0, 'f', 0));
  }
  else{
    WaterfallMinimumIntensitySlider->setValue(newValue - 2);
  }

  _waterfallDisplayPlot->setIntensityRange(WaterfallMinimumIntensitySlider->value(),
					   WaterfallMaximumIntensitySlider->value());
}


void
SpectrumDisplayForm::waterfallMinimumIntensityChangedCB( double newValue )
{
  if(newValue < WaterfallMaximumIntensitySlider->value()){
    WaterfallMinimumIntensityLabel->setText(QString("%1 dB").arg(newValue, 0, 'f', 0));
  }
  else{
    WaterfallMaximumIntensitySlider->setValue(newValue + 2);
  }

  _waterfallDisplayPlot->setIntensityRange(WaterfallMinimumIntensitySlider->value(),
					   WaterfallMaximumIntensitySlider->value());
}

void
SpectrumDisplayForm::fftComboBoxSelectedCB(const QString &fftSizeString)
{
  if(_systemSpecifiedFlag){
    _system->setFFTSize(fftSizeString.toLong());
  }
}


void
SpectrumDisplayForm::waterfallAutoScaleBtnCB()
{
  double minimumIntensity = _noiseFloorAmplitude - 5;
  double maximumIntensity = _peakAmplitude + 10;

#if QWT_VERSION < 0x060100
  if(minimumIntensity < WaterfallMinimumIntensitySlider->minValue()) {
    minimumIntensity = WaterfallMinimumIntensitySlider->minValue();
  }
  WaterfallMinimumIntensitySlider->setValue(minimumIntensity);
  if(maximumIntensity > WaterfallMaximumIntensitySlider->maxValue()) {
    maximumIntensity = WaterfallMaximumIntensitySlider->maxValue();
  }
#else /* QWT_VERSION < 0x060100 */
  if(minimumIntensity < WaterfallMinimumIntensitySlider->lowerBound()) {
    minimumIntensity = WaterfallMinimumIntensitySlider->lowerBound();
  }
  WaterfallMinimumIntensitySlider->setValue(minimumIntensity);
  if(maximumIntensity > WaterfallMaximumIntensitySlider->upperBound()) {
    maximumIntensity = WaterfallMaximumIntensitySlider->upperBound();
  }
#endif /* QWT_VERSION < 0x060100 */

  WaterfallMaximumIntensitySlider->setValue(maximumIntensity);
  waterfallMaximumIntensityChangedCB(maximumIntensity);
}

void
SpectrumDisplayForm::waterfallIntensityColorTypeChanged( int newType )
{
  QColor lowIntensityColor;
  QColor highIntensityColor;
  if(newType == INTENSITY_COLOR_MAP_TYPE_USER_DEFINED){
    // Select the Low Intensity Color
    lowIntensityColor = _waterfallDisplayPlot->getUserDefinedLowIntensityColor();
    if(!lowIntensityColor.isValid()){
      lowIntensityColor = Qt::black;
    }
    QMessageBox::information(this, "Low Intensity Color Selection", "In the next window, select the low intensity color for the waterfall display",  QMessageBox::Ok);
    lowIntensityColor = QColorDialog::getColor(lowIntensityColor, this);

    // Select the High Intensity Color
    highIntensityColor = _waterfallDisplayPlot->getUserDefinedHighIntensityColor();
    if(!highIntensityColor.isValid()){
      highIntensityColor = Qt::white;
    }
    QMessageBox::information(this, "High Intensity Color Selection", "In the next window, select the high intensity color for the waterfall display",  QMessageBox::Ok);
    highIntensityColor = QColorDialog::getColor(highIntensityColor, this);
  }

  _waterfallDisplayPlot->setIntensityColorMapType(0, newType, lowIntensityColor, highIntensityColor);
}

void
SpectrumDisplayForm::toggleTabFrequency(const bool state)
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
SpectrumDisplayForm::toggleTabWaterfall(const bool state)
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
SpectrumDisplayForm::toggleTabTime(const bool state)
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
SpectrumDisplayForm::toggleTabConstellation(const bool state)
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
SpectrumDisplayForm::setTimeDomainAxis(double min, double max)
{
  _timeDomainDisplayPlot->setYaxis(min, max);
}

void
SpectrumDisplayForm::setConstellationAxis(double xmin, double xmax,
						double ymin, double ymax)
{
  _constellationDisplayPlot->set_axis(xmin, xmax, ymin, ymax);
}

void
SpectrumDisplayForm::setConstellationPenSize(int size)
{
  _constellationDisplayPlot->set_pen_size( size );
}

void
SpectrumDisplayForm::setFrequencyAxis(double min, double max)
{
  _frequencyDisplayPlot->setYaxis(min, max);
}

void
SpectrumDisplayForm::setUpdateTime(double t)
{
  d_update_time = t;
  // QTimer class takes millisecond input
  displayTimer->start(d_update_time*1000);
}

void
SpectrumDisplayForm::onFFTPlotPointSelected(const QPointF p)
{
  emit plotPointSelected(p, 1);
}

void
SpectrumDisplayForm::onWFallPlotPointSelected(const QPointF p)
{
  emit plotPointSelected(p, 2);
}

void
SpectrumDisplayForm::onTimePlotPointSelected(const QPointF p)
{
  emit plotPointSelected(p, 3);
}

void
SpectrumDisplayForm::onConstPlotPointSelected(const QPointF p)
{
  emit plotPointSelected(p, 4);
}

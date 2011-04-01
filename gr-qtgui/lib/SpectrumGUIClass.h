#ifndef SPECTRUM_GUI_CLASS_HPP
#define SPECTRUM_GUI_CLASS_HPP

#include <gruel/thread.h>
#include <qwidget.h>
#include <qapplication.h>
#include <qlabel.h>
#include <qslider.h>
#include <spectrumUpdateEvents.h>

class SpectrumDisplayForm;
#include <spectrumdisplayform.h>

#include <cmath>

#include <complex>
#include <vector>
#include <string>

class SpectrumGUIClass
{
public:
  SpectrumGUIClass(const uint64_t maxDataSize, const uint64_t fftSize,
		   const double newCenterFrequency,
		   const double newStartFrequency, 
		   const double newStopFrequency);
  ~SpectrumGUIClass();
  void Reset();

  void OpenSpectrumWindow(QWidget*,
			  const bool frequency=true, const bool waterfall=true,
			  const bool time=true, const bool constellation=true,
			  const bool use_openGL=true);
  void SetDisplayTitle(const std::string);

  bool GetWindowOpenFlag();
  void SetWindowOpenFlag(const bool);

  void SetFrequencyRange(const double, const double, const double);
  double GetStartFrequency();
  double GetStopFrequency();
  double GetCenterFrequency();

  void UpdateWindow(const bool, const std::complex<float>*,
		    const uint64_t, const float*,
		    const uint64_t, const float*,
		    const uint64_t,
		    const timespec, const bool);

  float GetPowerValue();
  void SetPowerValue(const float);

  int GetWindowType();
  void SetWindowType(const int);

  int GetFFTSize();
  int GetFFTSizeIndex();
  void SetFFTSize(const int);

  timespec GetLastGUIUpdateTime();
  void SetLastGUIUpdateTime(const timespec);

  unsigned int GetPendingGUIUpdateEvents();
  void IncrementPendingGUIUpdateEvents();
  void DecrementPendingGUIUpdateEvents();
  void ResetPendingGUIUpdateEvents();

  static const long MAX_FFT_SIZE;
  static const long MIN_FFT_SIZE;

  QWidget* qwidget();

  void SetTimeDomainAxis(double min, double max);
  void SetConstellationAxis(double xmin, double xmax,
			    double ymin, double ymax);
  void SetConstellationPenSize(int size);
  void SetFrequencyAxis(double min, double max);

  void SetUpdateTime(double t);

protected:

private:

  gruel::mutex d_mutex;
  int64_t _dataPoints;
  std::string _title;
  double _centerFrequency;
  double _startFrequency;
  double _stopFrequency;
  float _powerValue;
  bool _windowOpennedFlag;
  int _windowType;
  int64_t _lastDataPointCount;
  int _fftSize;
  timespec _lastGUIUpdateTime;
  unsigned int _pendingGUIUpdateEventsCount;
  int _droppedEntriesCount;
  bool _fftBuffersCreatedFlag;
  double _updateTime;

  SpectrumDisplayForm* _spectrumDisplayForm;

  std::complex<float>* _fftPoints;
  double* _realTimeDomainPoints;
  double* _imagTimeDomainPoints;
};

#endif /* SPECTRUM_GUI_CLASS_HPP */

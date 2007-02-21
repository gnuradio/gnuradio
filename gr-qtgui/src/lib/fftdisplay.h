#ifndef FFT_DISPLAY_H
#define FFT_DISPLAY_H

#include <gr_complex.h>

#include <vector>

#include <qwidget.h>
#include <qwt_plot.h>
#include <qevent.h>

class fft_display_event:public QCustomEvent{
public:
	fft_display_event(std::vector<gr_complex>*, const float, const float);
	~fft_display_event();

	const std::vector<gr_complex>& get_fft_data()const;
	float get_start_frequency()const;
	float get_stop_frequency()const;

	static const int EVENT_TYPE_ID;
protected:

private:
	std::vector<gr_complex> d_fft_data;
	float d_start_frequency;
	float d_stop_frequency;
};

class fft_display:public QwtPlot{
	Q_OBJECT
public:	
	fft_display(const unsigned int, QWidget* = ((QWidget*)0));
	virtual ~fft_display();

	virtual void customEvent(QCustomEvent*);

	void set_start_frequency(const float);
	float get_start_frequency()const;

	void set_stop_frequency(const float);
	float get_stop_frequency()const;

	unsigned int get_fft_bin_size()const;

public slots:
	virtual void set_data( const std::vector<gr_complex>& );
	virtual void update_display();

protected:

private:
	std::vector<gr_complex>* d_fft_data;
	double* d_plot_data;
	double* d_x_data;
	unsigned int d_fft_bin_size;
	float d_start_frequency;
	float d_stop_frequency;
};

#endif /* FFT_DISPLAY_H */

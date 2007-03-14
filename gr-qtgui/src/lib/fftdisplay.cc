#ifndef FFT_DISPLAY_CC
#define FFT_DISPLAY_CC

#include <qwt/qwt_painter.h>
#include <qwt/qwt_plot_canvas.h>
#include <qwt/qwt_plot_curve.h>
#include <qwt/qwt_scale_engine.h>
#include <qapplication.h>
#include <fftdisplay.h>

const int fft_display_event::EVENT_TYPE_ID = QEvent::User+100;

fft_display_event::fft_display_event( std::vector<gr_complex>* fft_data, const float start_frequency, const float stop_frequency):QCustomEvent(fft_display_event::EVENT_TYPE_ID){
	d_fft_data.resize(fft_data->size());
	for(unsigned int i = 0; i < fft_data->size(); i++){
		d_fft_data[i] = fft_data->operator[](i);
	}
	d_start_frequency = start_frequency;
	d_stop_frequency = stop_frequency;
}

fft_display_event::~fft_display_event(){

}

const std::vector<gr_complex>& fft_display_event::get_fft_data() const{
	return d_fft_data;
}

float fft_display_event::get_start_frequency()const{
	return d_start_frequency;
}

float fft_display_event::get_stop_frequency()const{
	return d_stop_frequency;
}

fft_display::fft_display(const unsigned int fft_size, QWidget* parent):
	QwtPlot(parent)
{
    	// Disable polygon clipping
    	QwtPainter::setDeviceClipping(false);

    	// We don't need the cache here
    	canvas()->setPaintAttribute(QwtPlotCanvas::PaintCached, false);
    	canvas()->setPaintAttribute(QwtPlotCanvas::PaintPacked, false);

	canvas()->setPaletteBackgroundColor(QColor("white"));

	d_fft_bin_size = fft_size;
	if(d_fft_bin_size < 1){
		d_fft_bin_size = 1;
	}

	d_start_frequency = 0.0;
	d_stop_frequency = 4000.0;

	d_fft_data = new std::vector<gr_complex>(d_fft_bin_size);
	d_plot_data = new double[d_fft_bin_size];
	d_x_data = new double[d_fft_bin_size];
	for( unsigned int i = 0; i < d_fft_bin_size; i++){
		d_fft_data->operator[](i) = gr_complex(static_cast<float>(i), 0.0);
		d_x_data[i] = d_start_frequency + ((d_stop_frequency-d_start_frequency) / static_cast<float>(d_fft_bin_size)*static_cast<float>(i));
		d_plot_data[i] = 1.0;
	}
	
	// Set the Appropriate Axis Scale Engine
#warning Pass the axis info as necessary...
	if(true){
		setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);
	}
	else{
		setAxisScaleEngine(QwtPlot::yLeft, new QwtLog10ScaleEngine);
	}

	QwtPlotCurve* fft_plot_curve = new QwtPlotCurve("FFT Spectrum");
	fft_plot_curve->attach(this);
	fft_plot_curve->setPen(QPen(Qt::blue));
	fft_plot_curve->setRawData(d_x_data, d_plot_data, d_fft_bin_size);

	setTitle("Spectral Display");
}

fft_display::~fft_display(){
	/* The Qwt objects are destroyed by Qt when their parent is destroyed */

	delete[] d_plot_data;
	delete[] d_x_data;
	delete d_fft_data;
}


void fft_display::set_data(const std::vector<gr_complex>& input_data){
	unsigned int min_points = d_fft_data->size();
	if(min_points < input_data.size()){
		min_points = input_data.size();
	}
	if(min_points > d_fft_bin_size){
		min_points = d_fft_bin_size;
	}
	for(unsigned int point = 0; point < min_points; point++){
		d_fft_data->operator[](point) = input_data[point];
	}
}

void fft_display::update_display(){

	// Tell the event loop to display the new data - the event loop handles deleting this object
	qApp->postEvent(this, new fft_display_event(d_fft_data, d_start_frequency, d_stop_frequency));

}

void fft_display::customEvent(QCustomEvent* e){
	if(e->type() == fft_display_event::EVENT_TYPE_ID){
		fft_display_event* fft_display_event_ptr = (fft_display_event*)e;
		// Write out the FFT data to the display here

		gr_complex data_value;
		for(unsigned int number = 0; number < fft_display_event_ptr->get_fft_data().size(); number++){
			data_value = fft_display_event_ptr->get_fft_data()[number];
			d_plot_data[number] = abs(data_value);
#warning Add code here for handling magnitude, scaling, etc...

			d_x_data[number] = d_start_frequency + ((d_stop_frequency-d_start_frequency) / static_cast<float>(d_fft_bin_size)*static_cast<float>(number));
		}

    		// Axis 
    		setAxisTitle(QwtPlot::xBottom, "Frequency (Hz)");
    		setAxisScale(QwtPlot::xBottom, fft_display_event_ptr->get_start_frequency(), fft_display_event_ptr->get_stop_frequency());

    		setAxisTitle(QwtPlot::yLeft, "Values");
		replot();
	}
}

void fft_display::set_start_frequency(const float new_freq){
	d_start_frequency = new_freq;
}

float fft_display::get_start_frequency()const{
	return d_start_frequency;
}

void fft_display::set_stop_frequency(const float new_freq){
	d_stop_frequency = new_freq;
}

float fft_display::get_stop_frequency()const{
	return d_stop_frequency;
}

unsigned int fft_display::get_fft_bin_size()const{
	return d_fft_bin_size;
}


#endif /* FFT_DISPLAY_CC */

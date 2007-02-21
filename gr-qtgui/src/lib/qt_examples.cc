#include <stdio.h>
#include <unistd.h>

#include <iostream>
#include <fstream>

#include <qapplication.h>
#include <omnithread.h>
#include <vector>

#include <fftdisplay.h>

#warning Must make this threadsafe
static bool g_exit_flag = false;

void read_function(void* ptr){

	fft_display* fftDisplay = (fft_display*)ptr;

	std::vector<gr_complex> fftData(fftDisplay->get_fft_bin_size());
	for(unsigned int number = 0; number < fftData.size(); number++){
		fftData[number] = gr_complex(static_cast<float>(number), static_cast<float>(number));
	}

	float* readBuffer = new float[fftDisplay->get_fft_bin_size()*2];
	int amntRead = 0;

	fftDisplay->set_data(fftData);

	while(!g_exit_flag){
	        // Read in the data here
		sched_yield();
		std::cin.read((char*)readBuffer, fftDisplay->get_fft_bin_size()*sizeof(gr_complex));
		amntRead = std::cin.gcount();

		if(amntRead != static_cast<int>(fftDisplay->get_fft_bin_size()*sizeof(gr_complex))){
			fprintf(stderr, "Invalid Read Amount from stdin - closing program\n");
			qApp->quit();
			g_exit_flag = true;
		}
		else{
			for(unsigned int number = 0; number < fftData.size(); number++){
				fftData[number] = gr_complex(readBuffer[2*number], readBuffer[(2*number)+1]);
			}

			fftDisplay->set_data(fftData);

			fftDisplay->update_display();

			qApp->wakeUpGuiThread();
		}
	}	

	delete[] readBuffer;
}

int main (int argc, char* argv[]){
	extern char* optarg;
	extern int optind, optopt;
	float start_frequency = 0.0;
	float stop_frequency = 4000.0;
	int c;
	unsigned int fft_bin_size = 1024;

	while ((c = getopt(argc, argv, "s:p:b:")) != -1){
		switch(c){
		case 's': start_frequency = strtod(optarg, NULL); break;
		case 'p': stop_frequency = strtod(optarg, NULL); break;
		case 'b': fft_bin_size = (unsigned int)(atoi(optarg)); break;
		case ':': /* -s or -p w/o operand */
		fprintf(stderr, "Option -%c requires an arguement\n", optopt); break;
		case '?': fprintf(stderr, "Unrecognized option: -%c\n", optopt);
			  fprintf(stderr, "Valid Arguements\n -s <start frequency>\n -p <stop frequency> -b <fft_bin_size> - number of fft samples per display\n");
			  exit(-1); break;
		}
	}

	// Verify the stop frequency is greater than the stop frequency
	if(stop_frequency < start_frequency){
		fprintf(stderr, "Stop Frequency (%0.0f Hz) was less than the Start Frequency (%0.0f Hz)\n", stop_frequency, start_frequency);
		exit(-1);
	}

	// Create the QApplication - this MUST be done before ANY QObjects are created
	QApplication* qApp = new QApplication(argc, argv);

	fft_display* fftDisplay = new fft_display(fft_bin_size); // No Parent Specified

	// Resize the Display
	fftDisplay->resize(640,240);

	// Set the start and stop frequency
	fftDisplay->set_start_frequency(start_frequency);
	fftDisplay->set_stop_frequency(stop_frequency);

	g_exit_flag = false;
	omni_thread* thread_ptr = new omni_thread(&read_function, (void*)fftDisplay);
	
	// Set up the thread to read the data from stdin
	thread_ptr->start();
	sched_yield();

	// Make the closing of the last window call the quit()
	QObject::connect(qApp, SIGNAL(lastWindowClosed()), qApp, SLOT(quit()));

	// finally, refresh the plot
	fftDisplay->update_display();
	fftDisplay->show();

	// Start the Event Thread
	qApp->exec();

	// No QObjects should be deleted once the event thread exits...
	g_exit_flag = true;

	delete fftDisplay;

	// Destroy the Event Thread
	delete qApp;
	return 0;
}


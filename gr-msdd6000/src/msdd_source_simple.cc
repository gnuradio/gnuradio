#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <vector>
#include <msdd_source_simple.h>
#include <gr_io_signature.h>
#include <gr_sync_block.h>

#ifndef FALSE
#define FALSE 	(0==1)
#define TRUE	(0==0)
#endif


msdd_source_simple_sptr
msdd_make_source_simple ( const char *src, unsigned short port_src) 
{
  return msdd_source_simple_sptr (new msdd_source_simple ( src, port_src)); 
}


msdd_source_simple::msdd_source_simple (
	            const char *src, 
	            unsigned short port_src) 
		: gr_sync_block("MSDD_SOURCE_SIMPLE",
			gr_make_io_signature (0,0,0),
			gr_make_io_signature (1, 1, sizeof (short)))
{
	rcv = new MSDD6000((char*)src);
}

int
msdd_source_simple::work (int noutput_items,
                         gr_vector_const_void_star &input_items,
                         gr_vector_void_star &output_items)
{
	
#define BUF_LEN	(366*sizeof(short)*2 + 6)

	float* out1 =(float*) output_items[0];

	char buffer[BUF_LEN];
	rcv->read( &buffer[0], BUF_LEN );

	int seq = *((int*) &buffer[2]);
	
	if(d_lastseq == -366){
		// not started case
		if(seq == 0){
			d_lastseq = 0;
			} else {
			// THROW AWAY SAMPLES WE ARE NOT STARTED YET!
			return 0;
			}
		
		} else {
		// started case
		int samples_missed = seq - d_lastseq - 366;
		if(samples_missed > 0){
			printf("dropped %d samples.\n", samples_missed);
			}
		d_lastseq = seq;
		}
	
	if(noutput_items< 366*2){
		printf("NOT ENOUGH SPACE IN OUTPUT BUFFER!!! >:-(\n");
		}
	
	memcpy(&out1[0], &buffer[6], BUF_LEN - 6);
	
//	for(int i = 0; i < 366*2; i++){
//		out1[i] = (float)  (*((short*) &buffer[6+2*i]) );
//	}
	
	return 366*2;
}

bool msdd_source_simple::set_decim_rate(unsigned int rate)
{
	rcv->set_decim(log2(rate));
	return TRUE;
}


bool msdd_source_simple::set_rx_freq(int channel, double freq)
{
	long new_fc = (long)freq;
	rcv->set_fc( new_fc/1000000, new_fc%1000000);
	return TRUE;
}


bool msdd_source_simple::set_pga(int which, double gain)
{
	if(gain < 0 || gain > 10){
		printf("GAIN IS OUTSIDE ACCEPTABLE RANGE!\n");
		return FALSE;
	}
	// ok i lied this is not really a pga, its decimation gain
	rcv->set_ddc_gain((int)gain);
	return TRUE;
}


msdd_source_simple::~msdd_source_simple ()
{
	delete rcv;
}


bool msdd_source_simple::start()
{
	rcv->start();
}


bool msdd_source_simple::stop()
{
	rcv->stop();
}

int msdd_source_simple::ninput_bytes_reqd_for_noutput_items(int out){
	return 0;
}

long msdd_source_simple::adc_freq(){
	return 102400000;
}

int msdd_source_simple::decim_rate(){
	return pow(2, rcv->d_decim);
}


std::vector<int> msdd_source_simple::gain_range(){
	static std::vector<int> r;
	r.push_back(0);
	r.push_back(12);
	return r;
}

std::vector<float> msdd_source_simple::freq_range(){
	std::vector<float> r;
	r.push_back(30.0*1000*1000);
	r.push_back(6.0*1000*1000*1000);
	return r;
}


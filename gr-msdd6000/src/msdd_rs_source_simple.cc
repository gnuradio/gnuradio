/* -*- c++ -*- */
/*
 * Copyright 2008,2010 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <msdd_rs_source_simple.h>
#include <gr_io_signature.h>
#include <string.h>
#include <cstdio>


msdd_rs_source_simple_sptr
msdd_rs_make_source_simple ( const char *src, unsigned short port_src) 
{
  return gnuradio::get_initial_sptr(new msdd_rs_source_simple ( src, port_src)); 
}


msdd_rs_source_simple::msdd_rs_source_simple (
	            const char *src, 
	            unsigned short port_src) 
		: gr_sync_block("MSDD_RS_SOURCE_SIMPLE",
				gr_make_io_signature (0,0,0),
				gr_make_io_signature (1, 1, sizeof (short))),
                  rcv(new MSDD6000_RS((char*) src)), d_lastseq(0)
{
}

msdd_rs_source_simple::~msdd_rs_source_simple ()
{
}


int
msdd_rs_source_simple::work (int noutput_items,
                         gr_vector_const_void_star &input_items,
                         gr_vector_void_star &output_items)
{
	
#define BUF_LEN	(366*sizeof(short)*2 + 6)

	float* out1 =(float*) output_items[0];

	char buffer[BUF_LEN];
	/* Read a buffer out -- looking at UDP payload at this point.*/
	rcv->read( &buffer[0], BUF_LEN );
	
	//int seq = *((int*) &buffer[2]);
	int seq;
	memcpy(&seq, &buffer[2], 4*sizeof(char));

	char type = buffer[0];
	//printf("Sequence %d\n",seq);
	
	// FIXME get rid of these magic 366's!
	if(d_lastseq == -366)
	{
        if (type != 0){
            /* Received control packet -- parse and update locally stored parameters */
            printf("Parsing control Packet\n");
            rcv->parse_control(&buffer[0], seq);       
        }
        else{   
            // not started case
            if(seq == 0){
                d_lastseq = 0;
            } 
            else 
            {
                // THROW AWAY SAMPLES WE ARE NOT STARTED YET!
                return 0;
            }
        }
	} 
	// Started case
	else 
	{
        if (type != 0){
                        /* Received control packet -- parse and update locally stored parameters */
            printf("Parsing control Packet\n");
            rcv->parse_control(&buffer[0], seq);       
        }
            
        else {
            int samples_missed = seq - d_lastseq - 366;
            if(samples_missed > 0)
            {
                printf("dropped %d samples.\n", samples_missed);
            }
            d_lastseq = seq;
        }
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

//bool msdd_rs_source_simple::set_decim_rate(unsigned int rate)
//{
//	// FIXME seems buggy.  How about a floor or ceil?
//        rcv->set_decim((int) log2(rate));
//	return true;
//}

bool msdd_rs_source_simple::set_rx_freq(double freq)
{
	long new_fc = (long)freq;
	rcv->set_fc( new_fc/1000000, new_fc%1000000);
	return true;
}


bool msdd_rs_source_simple::set_ddc_gain(double gain)
{
	if(gain < 0 || gain > 7){ // only 3 bits available.
		printf("GAIN IS OUTSIDE ACCEPTABLE RANGE!\n");
		return false;
	}
	//decimation gain
	rcv->set_ddc_gain((int)gain);
	return true;
}

// Set desired sample rate of MSDD6000 -- Note bounds checking is 
// done by the module and it will return the value actually used in the hardware.
bool msdd_rs_source_simple::set_ddc_samp_rate(double rate)
{
	rcv->set_ddc_samp_rate((float) rate);
	return true;		
}

// Set desired input BW of MSDD6000 -- Note bounds checking is 
// done by the module and it will return the value actually used in the hardware.
bool msdd_rs_source_simple::set_ddc_bw(double bw)
{
	rcv->set_ddc_bw((float) bw);
	return true;		
}

bool msdd_rs_source_simple::set_rf_atten(double rf_atten)
{
        rcv->set_rf_attn((int) rf_atten);
        return true;
}

bool msdd_rs_source_simple::start()
{
	rcv->start();
    rcv->stop_data();
	return true;
}

bool msdd_rs_source_simple::stop()
{
	rcv->stop();
	return true;
}

int msdd_rs_source_simple::start_data()
{
	return rcv->start_data();
}

int msdd_rs_source_simple::stop_data()
{
	return rcv->stop_data();
}

/* Query functions */
long msdd_rs_source_simple::pull_adc_freq(){
	return 102400000;
}

/* Request the current ddc sample rate */
float msdd_rs_source_simple::pull_ddc_samp_rate(){
	return(rcv->pull_ddc_samp_rate());
}

/* Request the current ddc bandwidth */
float msdd_rs_source_simple::pull_ddc_bw(){
	return(rcv->pull_ddc_bw());
	
}

/* Request the current rx freq */
float msdd_rs_source_simple::pull_rx_freq(){
	return(rcv->pull_rx_freq());
}

/* Request current ddc gain */
int msdd_rs_source_simple::pull_ddc_gain(){
	return(rcv->pull_ddc_gain());
}

/* Request current RF attenuation */
int msdd_rs_source_simple::pull_rf_atten(){
	return(rcv->pull_rf_atten());
}

std::vector<int> msdd_rs_source_simple::gain_range(){
	static std::vector<int> r;
	r.push_back(0);
	r.push_back(12);
	return r;
}

std::vector<float> msdd_rs_source_simple::freq_range(){
	std::vector<float> r;
	r.push_back(30.0*1000*1000);
	r.push_back(6.0*1000*1000*1000);
	return r;
}

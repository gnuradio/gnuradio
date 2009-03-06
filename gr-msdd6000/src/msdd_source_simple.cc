/* -*- c++ -*- */
/*
 * Copyright 2008,2009 Free Software Foundation, Inc.
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

#include <msdd_source_simple.h>
#include <gr_io_signature.h>
#include <string.h>


msdd_source_simple_sptr
msdd_make_source_simple (const char *src, unsigned short port_src) 
{
  return msdd_source_simple_sptr (new msdd_source_simple ( src, port_src)); 
}


msdd_source_simple::msdd_source_simple (const char *src, 
					unsigned short port_src) 
  : gr_sync_block("MSDD_SOURCE_SIMPLE",
		  gr_make_io_signature (0,0,0),
		  gr_make_io_signature (1, 1, sizeof (short))),
    rcv(new MSDD6000((char*) src)), d_lastseq(0), d_firstrun(true)
{
  set_output_multiple(MSDD_COMPLEX_SAMPLES_PER_PACKET*2);
}

msdd_source_simple::~msdd_source_simple ()
{
}


int
msdd_source_simple::work (int noutput_items,
                         gr_vector_const_void_star &input_items,
                         gr_vector_void_star &output_items)
{
	
#define BUF_LEN        (MSDD_COMPLEX_SAMPLES_PER_PACKET*sizeof(short)*2 + 6)

  signed short* out1 =(signed short*) output_items[0];

  for(int i=0; i<floor(noutput_items*1.0/(2*MSDD_COMPLEX_SAMPLES_PER_PACKET));i++){
    char buffer[BUF_LEN];
    rcv->read( &buffer[0], BUF_LEN );

    int seq = *((int*) &buffer[2]);

    if(d_lastseq == -MSDD_COMPLEX_SAMPLES_PER_PACKET){
      // not started case
      if(seq == 0){
	d_lastseq = 0;
      } else {
	// THROW AWAY SAMPLES WE ARE NOT STARTED YET!
	return 0;
      }

    } else {
      // started case
      int samples_missed = seq - d_lastseq - MSDD_COMPLEX_SAMPLES_PER_PACKET;
      if(samples_missed > 0){
	if(d_firstrun == true){
	  // we may have missed some initial samples off the beginning of
	  // a stream but there are no drop outs in the middle of what we have
	} else {
	  printf("dropped %d samples.\n", samples_missed);
	}
      }
      d_lastseq = seq;
    }

    int out_idx = i*MSDD_COMPLEX_SAMPLES_PER_PACKET*2;
    memcpy(&out1[out_idx], &buffer[6], BUF_LEN - 6);
    d_firstrun = false;
  }

  return noutput_items;

}

bool msdd_source_simple::set_decim_rate(unsigned int rate)
{
  rcv->set_decim((int) floor(log2(rate)));
  return true;
}


bool msdd_source_simple::set_rx_freq(int channel, double freq)
{
  long new_fc = (long)freq;
  rcv->set_fc( new_fc/1000000, new_fc%1000000);
  return true;
}


bool msdd_source_simple::set_pga(int which, double gain)
{
  if(gain < 0 || gain > 10){
    printf("GAIN IS OUTSIDE ACCEPTABLE RANGE!\n");
    return false;
  }
  // ok i lied this is not really a pga, its decimation gain
  rcv->set_ddc_gain((int)gain);
  return true;
}


bool msdd_source_simple::start()
{
  rcv->start();
  return true;
}


bool msdd_source_simple::stop()
{
  rcv->stop();
  return true;
}

long msdd_source_simple::adc_freq()
{
  return 102400000;
}

int msdd_source_simple::decim_rate()
{
  return 1 << rcv->d_decim;
}


std::vector<int> msdd_source_simple::gain_range()
{
  static std::vector<int> r;
  r.push_back(0);
  r.push_back(12);
  return r;
}

std::vector<float> msdd_source_simple::freq_range()
{
  std::vector<float> r;
  r.push_back(30.0*1000*1000);
  r.push_back(6.0*1000*1000*1000);
  return r;
}

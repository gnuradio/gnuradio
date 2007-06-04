/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_ofdm_bpsk_mapper.h>
#include <gr_io_signature.h>
#include <vector>

gr_ofdm_bpsk_mapper_sptr
gr_make_ofdm_bpsk_mapper (unsigned int msgq_limit, 
			  unsigned int occupied_carriers, unsigned int fft_length,
			  const std::vector<gr_complex> &known_symbol1, 
			  const std::vector<gr_complex> &known_symbol2)
{
  return gr_ofdm_bpsk_mapper_sptr (new gr_ofdm_bpsk_mapper (msgq_limit, occupied_carriers, fft_length, 
							    known_symbol1, known_symbol2));
}

// Consumes 1 packet and produces as many OFDM symbols of fft_length to hold the full packet
gr_ofdm_bpsk_mapper::gr_ofdm_bpsk_mapper (unsigned int msgq_limit, 
					  unsigned int occupied_carriers, unsigned int fft_length,
					  const std::vector<gr_complex> &known_symbol1, 
					  const std::vector<gr_complex> &known_symbol2)
  : gr_sync_block ("ofdm_bpsk_mapper",
		   gr_make_io_signature (0, 0, 0),
		   gr_make_io_signature2 (1, 2, sizeof(gr_complex)*fft_length, sizeof(char))),
    d_msgq(gr_make_msg_queue(msgq_limit)), d_msg_offset(0), d_eof(false),
    d_occupied_carriers(occupied_carriers),
    d_fft_length(fft_length),
    d_bit_offset(0),
    d_header_sent(0),
    d_known_symbol1(known_symbol1),
    d_known_symbol2(known_symbol2)
{
  assert(d_occupied_carriers <= d_fft_length);
  assert(d_occupied_carriers == d_known_symbol1.size());
  assert(d_occupied_carriers == d_known_symbol2.size());
}

gr_ofdm_bpsk_mapper::~gr_ofdm_bpsk_mapper(void)
{
}

float randombit()
{
  int r = rand()&1;
  return (float)(-1 + 2*r);
}

int
gr_ofdm_bpsk_mapper::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
{
  gr_complex *out = (gr_complex *)output_items[0];
  
  unsigned int i=0;
  unsigned int unoccupied_carriers = d_fft_length - d_occupied_carriers;
  unsigned int zeros_on_left = (unsigned)ceil(unoccupied_carriers/2.0);

  //printf("OFDM BPSK Mapper:  ninput_items: %d   noutput_items: %d\n", ninput_items[0], noutput_items);

  if(d_eof) {
    return -1;
  }
  
  if(!d_msg) {
    d_msg = d_msgq->delete_head();	   // block, waiting for a message
    d_msg_offset = 0;
    d_bit_offset = 0;
    d_header_sent = 0;
    
    if((d_msg->length() == 0) && (d_msg->type() == 1)) {
      d_msg.reset();
      return -1;
    }
  }

  if(output_items.size() == 2) {
    char *sig = (char *)output_items[1];
    if(d_header_sent == 1) {
      sig[0] = 1;
    }
    else {
      sig[0] = 0;
    }
  }
  
  // Build a single symbol:

  // Initialize all bins to 0 to set unused carriers
  memset(out, 0, d_fft_length*sizeof(gr_complex));
  
  if(d_header_sent == 0) {
     for(i=0; i < d_occupied_carriers; i++) {
       out[i+zeros_on_left] = d_known_symbol1[i];
     }
    d_header_sent++;

    return 1;
  }
  
  if(d_header_sent == 1) {
    for(i=0; i < d_occupied_carriers; i++) {
      out[i+zeros_on_left] = d_known_symbol2[i];
    }
    d_header_sent++;

    return 1;
  }

  i = 0;
  while((d_msg_offset < d_msg->length()) && (i < d_occupied_carriers)) {
    unsigned char bit = (d_msg->msg()[d_msg_offset] >> (d_bit_offset)) & 0x01;
    out[i + zeros_on_left] = gr_complex(-1+2*(bit));
    i++;
    d_bit_offset++;
    if(d_bit_offset == 8) {
      d_bit_offset = 0;
      d_msg_offset++;
    }
  }

  // Ran out of data to put in symbol
  if (d_msg_offset == d_msg->length()) {
    while(i < d_occupied_carriers) {   // finish filling out the symbol
      out[i + zeros_on_left] = gr_complex(randombit(),0);
      i++;
    }

    if (d_msg->type() == 1)	           // type == 1 sets EOF
      d_eof = true;
    d_msg.reset();   // finished packet, free message
 
    assert(d_bit_offset == 0);
    return 1;          // produced one symbol
  }
  
  return 1;  // produced symbol
}


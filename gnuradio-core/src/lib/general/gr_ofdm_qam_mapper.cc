/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_ofdm_qam_mapper.h>
#include <gr_io_signature.h>
#include <stdexcept>
#include <string.h>

gr_ofdm_qam_mapper_sptr
gr_make_ofdm_qam_mapper (unsigned int msgq_limit, 
			 unsigned int occupied_carriers, unsigned int fft_length,
			 int m)
{
  return gr_ofdm_qam_mapper_sptr (new gr_ofdm_qam_mapper (msgq_limit, occupied_carriers, fft_length, m));
}

// Consumes 1 packet and produces as many OFDM symbols of fft_length to hold the full packet
gr_ofdm_qam_mapper::gr_ofdm_qam_mapper (unsigned int msgq_limit, 
					unsigned int occupied_carriers, unsigned int fft_length,
					int m)
  : gr_sync_block ("ofdm_qam_mapper",
		   gr_make_io_signature (0, 0, 0),
		   gr_make_io_signature2 (1, 2, sizeof(gr_complex)*fft_length, sizeof(char))),
    d_msgq(gr_make_msg_queue(msgq_limit)), d_msg_offset(0), d_eof(false),
    d_occupied_carriers(occupied_carriers),
    d_fft_length(fft_length),
    d_bit_offset(0),
    d_pending_flag(0),
    d_mod_order(m)
{
  if (!(d_occupied_carriers <= d_fft_length))
    throw std::invalid_argument("gr_ofdm_qam_mapper: occupied carriers must be <= fft_length");

  bool ok = false;
  if(m == 2) {
    ok = true;
  }
  if(m == 4) {
    ok = true;
  }
  if(m == 16) {
    ok = true;
  }
  if(m == 64) {
    ok = true;
  }
  if(m == 256) {
    ok = true;
  }
  
  if(!ok)
    throw std::invalid_argument("Order M must be [2, 4, 16, 64, 256]");  

  make_constellation();
}

gr_ofdm_qam_mapper::~gr_ofdm_qam_mapper(void)
{
}

void gr_ofdm_qam_mapper::make_constellation()
{
  int i = 0, j = 0, ii = 0, jj = 0;
  // number of bits/symbol (log2(M))
  int k = (int)(log10(d_mod_order) / log10(2.0));

  int a, b;
  float re, im;
  float coeff = 1;
  std::vector<int> bits_i, bits_q;

  int rr = 0;
  int ss = 0;
  int ll = bits_i.size();

  for(i=0; i < d_mod_order; i++) {
    a = (i & (0x01 << (k-1))) >> (k-1);
    b = (i & (0x01 << (k-2))) >> (k-2);
    for(j=2; j < k; j+=2) {
      bits_i.push_back( (i & (0x01 << (k-j-1))) >> (k-j-1));
      bits_q.push_back( (i & (0x01 << (k-(j+1)-1))) >> (k-(j+1)-1));
    }
    
    ss = 0;
    ll = bits_i.size();
    for(ii = 0; ii < ll; ii++) {
      rr = 0;
      for(jj = 0; jj < (ll-ii); jj++) {
	rr = abs(bits_i[jj] - rr);
      }
      ss += rr * pow(2.0, ii+1.0);
    }
    re = (2.0*a-1.0)*(ss+1.0);
    
    ss = 0;
    ll = bits_q.size();
    for(ii = 0; ii < ll; ii++) {
      rr = 0;
      for(jj = 0; jj < (ll-ii); jj++) {
	rr = abs(bits_q[jj] - rr);
      }
      ss += rr*pow(2.0, ii+1.0);
    }
    im = (2.0*b-1.0)*(ss+1.0);
      
    a = std::max(re, im);
    if(a > coeff) {
      coeff = a;
    }
    d_constellation_map.push_back(gr_complex(re, im));
  }
  
  d_constellation_map[0] = gr_complex(-3, -3);
  d_constellation_map[1] = gr_complex(-3, -1);
  d_constellation_map[2] = gr_complex(-3, 1);
  d_constellation_map[3] = gr_complex(-3, 3);
  d_constellation_map[4] = gr_complex(-1, -3);
  d_constellation_map[5] = gr_complex(-1, -1);
  d_constellation_map[6] = gr_complex(-1, 1);
  d_constellation_map[7] = gr_complex(-1, 3);
  d_constellation_map[8] = gr_complex(1, -3);
  d_constellation_map[9] = gr_complex(1, -1);
  d_constellation_map[10] = gr_complex(1, 1);
  d_constellation_map[11] = gr_complex(1, 3);
  d_constellation_map[12] = gr_complex(3, -3);
  d_constellation_map[13] = gr_complex(3, -1);
  d_constellation_map[14] = gr_complex(3, 1);
  d_constellation_map[15] = gr_complex(3, 3);

  coeff = sqrt(31.0)/2.0;
  for(i = 0; i < d_constellation_map.size(); i++) {
    d_constellation_map[i] /= coeff;
    printf("const[%d]: %f + j%f\n", i, d_constellation_map[i].real(), d_constellation_map[i].imag());
  }
}

static float
randombit()
{
  int r = rand()&1;
  return (float)(-1 + 2*r);
}

int
gr_ofdm_qam_mapper::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
{
  gr_complex *out = (gr_complex *)output_items[0];
  
  unsigned int i=0;
  unsigned int unoccupied_carriers = d_fft_length - d_occupied_carriers;
  unsigned int zeros_on_left = (unsigned)ceil(unoccupied_carriers/2.0);

  //printf("OFDM QAM Mapper:  ninput_items: %d   noutput_items: %d\n", ninput_items[0], noutput_items);

  if(d_eof) {
    return -1;
  }
  
  if(!d_msg) {
    d_msg = d_msgq->delete_head();	   // block, waiting for a message
    d_msg_offset = 0;
    d_bit_offset = 0;
    d_pending_flag = 1;			   // new packet, write start of packet flag
    
    if((d_msg->length() == 0) && (d_msg->type() == 1)) {
      d_msg.reset();
      return -1;		// We're done; no more messages coming.
    }
  }

  char *out_flag = 0;
  if(output_items.size() == 2)
    out_flag = (char *) output_items[1];
  

  // Build a single symbol:
  

  // Initialize all bins to 0 to set unused carriers
  memset(out, 0, d_fft_length*sizeof(gr_complex));
  
  i = 0;
  while((d_msg_offset < d_msg->length()) && (i < d_occupied_carriers)) {
    unsigned char bit0 = (d_msg->msg()[d_msg_offset] >> (d_bit_offset)) & 0x01;
    d_bit_offset++;
    
    unsigned char bit1 = (d_msg->msg()[d_msg_offset] >> (d_bit_offset)) & 0x01;
    d_bit_offset++;
    
    unsigned char bit2 = (d_msg->msg()[d_msg_offset] >> (d_bit_offset)) & 0x01;
    d_bit_offset++;
    
    unsigned char bit3 = (d_msg->msg()[d_msg_offset] >> (d_bit_offset)) & 0x01;
    d_bit_offset++;

    unsigned char bit = (bit0 << 3) | (bit1 << 2) | (bit2 << 1) | (bit3 << 0);
    
    out[i + zeros_on_left] = d_constellation_map[bit];
    i++;
    if(d_bit_offset == 8) {
      d_bit_offset = 0;
      d_msg_offset++;
    }
  }

  // Ran out of data to put in symbol
  if (d_msg_offset == d_msg->length()) {
    while(i < d_occupied_carriers) {   // finish filling out the symbol
      out[i + zeros_on_left] = d_constellation_map[rand() & 0x0F];
      i++;
    }

    if (d_msg->type() == 1)	        // type == 1 sets EOF
      d_eof = true;
    d_msg.reset();   			// finished packet, free message
    assert(d_bit_offset == 0);
  }

  if (out_flag)
    out_flag[0] = d_pending_flag;
  d_pending_flag = 0;

  return 1;  // produced symbol
}


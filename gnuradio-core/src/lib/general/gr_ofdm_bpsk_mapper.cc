/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
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
gr_make_ofdm_bpsk_mapper (unsigned int mtu, unsigned int occupied_carriers, unsigned int vlen,
			  std::vector<gr_complex> known_symbol1, std::vector<gr_complex> known_symbol2)
{
  return gr_ofdm_bpsk_mapper_sptr (new gr_ofdm_bpsk_mapper (mtu, occupied_carriers, vlen, 
							    known_symbol1, known_symbol2));
}

gr_ofdm_bpsk_mapper::gr_ofdm_bpsk_mapper (unsigned int mtu, unsigned int occupied_carriers, unsigned int vlen,
					  std::vector<gr_complex> known_symbol1, 
					  std::vector<gr_complex> known_symbol2)
  : gr_block ("ofdm_bpsk_mapper",
	      gr_make_io_signature (1, 1, 2*sizeof(int) + sizeof(unsigned char)*mtu),
	      gr_make_io_signature (1, 1, sizeof(gr_complex)*vlen)),
    d_mtu(mtu),
    d_occupied_carriers(occupied_carriers),
    d_vlen(vlen),
    d_packet_offset(0),
    d_bit_offset(0),
    d_header_sent(0),
    d_known_symbol1(known_symbol1),
    d_known_symbol2(known_symbol2)

{
  assert(d_occupied_carriers < d_vlen);
  assert(d_occupied_carriers == d_known_symbol1.size());
  assert(d_occupied_carriers == d_known_symbol2.size());
}

gr_ofdm_bpsk_mapper::~gr_ofdm_bpsk_mapper(void)
{
}

void
gr_ofdm_bpsk_mapper::forecast (int noutput_items, gr_vector_int &ninput_items_required)
{
  unsigned ninputs = ninput_items_required.size ();
  for (unsigned i = 0; i < ninputs; i++)
    ninput_items_required[i] = 1;
}

int
gr_ofdm_bpsk_mapper::general_work(int noutput_items,
				  gr_vector_int &ninput_items,
				  gr_vector_const_void_star &input_items,
				  gr_vector_void_star &output_items)
{
  const gr_frame *in = (const gr_frame *) input_items[0];
  gr_complex *out = (gr_complex *)output_items[0];
  
  unsigned int i=0;
  unsigned int num_symbols = 0, pkt_length;

  //printf("OFDM BPSK Mapper:  ninput_items: %d   noutput_items: %d\n", ninput_items[0], noutput_items);

  pkt_length = in[0].length;

  std::vector<gr_complex>::iterator ks_itr;
  if(d_header_sent == 0) {
     ks_itr = d_known_symbol1.begin();
  }
  else if(d_header_sent == 1) {
    ks_itr = d_known_symbol2.begin();
  }
  
  if(d_header_sent < 2) {
    //  Add training symbols here
    for(i=0; i < (ceil((d_vlen - d_occupied_carriers)/2.0)); i++) {
      out[i] = gr_complex(0,0);
    }
    for(;i<d_vlen - ceil((d_vlen-d_occupied_carriers)/2.0);i++) {
      //out[i] = gr_complex(1,0);
      out[i] = *(ks_itr++);
    }
    for(; i < d_vlen; i++) {
      out[i] = gr_complex(0,0);
    }

    num_symbols = 1;
    out += d_vlen;
    d_header_sent++;
  }
  
  unsigned int unoccupied_carriers = d_vlen - d_occupied_carriers;
  unsigned int zeros_on_left = (unsigned)ceil(unoccupied_carriers/2.0);
  unsigned int zeros_on_right = unoccupied_carriers - zeros_on_left;

  while(num_symbols < (unsigned)noutput_items) {

    // stick in unused carriers
    for(i = d_vlen-zeros_on_right; i < d_vlen; i++) {
      out[i] = gr_complex(0,0);
    }

    for(i=0; i < zeros_on_left; i++) {
      out[i] = gr_complex(0,0);
    }

    while((d_packet_offset < pkt_length) && (i < d_vlen-zeros_on_right)) {
      unsigned char bit = (in[0].data[d_packet_offset] >> (d_bit_offset++)) & 0x01;
      out[i++] = gr_complex(-1+2*(bit));
      if(d_bit_offset == 8) {
	d_bit_offset = 0;
	d_packet_offset++;
      }
    }

    // Ran out of data to put in symbols
    if(d_packet_offset == pkt_length) {
      while(i < d_vlen-zeros_on_right) {
	out[i++] = gr_complex(0,0);
      }

      d_packet_offset = 0;
      assert(d_bit_offset == 0);
      num_symbols++;
      d_header_sent = 0;
      consume_each(1);
      return num_symbols;
    }
    
    // Ran out of space in symbol
    out += d_vlen;
    num_symbols++;
  }
  consume_each(0);
  return num_symbols;
}


/* -*- c++ -*- */
/*
 * Copyright 2004,2006 Free Software Foundation, Inc.
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

// WARNING: this file is machine generated.  Edits will be over written

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_unpacked_to_packed_bb.h>
#include <gr_io_signature.h>
#include <assert.h>

static const unsigned int BITS_PER_TYPE = sizeof(unsigned char) * 8;


gr_unpacked_to_packed_bb_sptr 
gr_make_unpacked_to_packed_bb (unsigned int bits_per_chunk, gr_endianness_t endianness)
{
  return gr_unpacked_to_packed_bb_sptr 
    (new gr_unpacked_to_packed_bb (bits_per_chunk,endianness));
}

gr_unpacked_to_packed_bb::gr_unpacked_to_packed_bb (unsigned int bits_per_chunk, 
						    gr_endianness_t endianness)
  : gr_block ("unpacked_to_packed_bb",
	      gr_make_io_signature (1, -1, sizeof (unsigned char)),
	      gr_make_io_signature (1, -1, sizeof (unsigned char))),
    d_bits_per_chunk(bits_per_chunk),d_endianness(endianness),d_index(0)
{
  assert (bits_per_chunk <= BITS_PER_TYPE);
  assert (bits_per_chunk > 0);

  set_relative_rate (bits_per_chunk/(1.0 * BITS_PER_TYPE));
}

void
gr_unpacked_to_packed_bb::forecast(int noutput_items, gr_vector_int &ninput_items_required)
{
  int input_required = (int) ceil( (d_index+noutput_items * 1.0 * BITS_PER_TYPE)/d_bits_per_chunk);
  unsigned ninputs = ninput_items_required.size();
  for (unsigned int i = 0; i < ninputs; i++) {
    ninput_items_required[i] = input_required;
  }
}

unsigned int
get_bit_be1 (const unsigned char *in_vector,unsigned int bit_addr, unsigned int bits_per_chunk) {
  unsigned int byte_addr = (int)bit_addr/bits_per_chunk;
  unsigned char x = in_vector[byte_addr];
  unsigned int residue = bit_addr - byte_addr * bits_per_chunk;
  //printf("Bit addr %d  byte addr %d  residue %d  val  %d\n",bit_addr,byte_addr,residue,(x>>(bits_per_chunk-1-residue))&1);
  return (x >> (bits_per_chunk-1-residue))&1;
}

int
gr_unpacked_to_packed_bb::general_work (int noutput_items,
					gr_vector_int &ninput_items,
					gr_vector_const_void_star &input_items,
					gr_vector_void_star &output_items)
{
  unsigned int index_tmp = d_index;

  assert (input_items.size() == output_items.size());
  int nstreams = input_items.size();

  for (int m=0; m< nstreams; m++) {
    const unsigned char *in = (unsigned char *) input_items[m];
    unsigned char *out = (unsigned char *) output_items[m];
    index_tmp=d_index;

    // per stream processing

    //assert((ninput_items[m]-d_index)*d_bits_per_chunk >= noutput_items*BITS_PER_TYPE);
  
    switch(d_endianness){

    case GR_MSB_FIRST:
      for(int i=0;i<noutput_items;i++) {
	unsigned char tmp=0;
	for(unsigned int j=0; j<BITS_PER_TYPE; j++) {
	  tmp = (tmp<<1) | get_bit_be1(in,index_tmp,d_bits_per_chunk);
	  index_tmp++;
	}
	out[i] = tmp;
      }
      break;

    case GR_LSB_FIRST:
      for(int i=0;i<noutput_items;i++) {
	unsigned long tmp=0;
	for(unsigned int j=0; j<BITS_PER_TYPE; j++) {
	  tmp = (tmp>>1)| (get_bit_be1(in,index_tmp,d_bits_per_chunk)<<(BITS_PER_TYPE-1));
	  index_tmp++;
	}
	out[i] = tmp;
      }
      break;
      
    default:
      assert(0);
    }
  }

  d_index = index_tmp;
  consume_each ((int)(d_index/d_bits_per_chunk));
  d_index = d_index%d_bits_per_chunk;

  return noutput_items;
}

/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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

// @WARNING@

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <@NAME@.h>
#include <gr_io_signature.h>
#include <assert.h>
#include <gr_log2_const.h>

static const unsigned int BITS_PER_TYPE = sizeof(@I_TYPE@) * 8;
static const unsigned int LOG2_L_TYPE = gr_log2_const<sizeof(@I_TYPE@) * 8>();


@SPTR_NAME@ 
gr_make_@BASE_NAME@ (unsigned int bits_per_chunk, gr_endianness_t endianness)
{
  return @SPTR_NAME@ 
    (new @NAME@ (bits_per_chunk,endianness));
}

@NAME@::@NAME@ (unsigned int bits_per_chunk, 
						    gr_endianness_t endianness)
  : gr_block ("@BASE_NAME@",
	      gr_make_io_signature (1, -1, sizeof (@I_TYPE@)),
	      gr_make_io_signature (1, -1, sizeof (@O_TYPE@))),
    d_bits_per_chunk(bits_per_chunk),d_endianness(endianness),d_index(0)
{
  assert (bits_per_chunk <= BITS_PER_TYPE);
  assert (bits_per_chunk > 0);

  set_relative_rate ((1.0 * BITS_PER_TYPE) / bits_per_chunk);
}

void
@NAME@::forecast(int noutput_items, gr_vector_int &ninput_items_required)
{

  int input_required = (int) ceil((d_index + noutput_items * d_bits_per_chunk) / (1.0 * BITS_PER_TYPE));
  unsigned ninputs = ninput_items_required.size();
  for (unsigned int i = 0; i < ninputs; i++) {
    ninput_items_required[i] = input_required;
    //printf("Forecast wants %d needs %d\n",noutput_items,ninput_items_required[i]);
  }
}

unsigned int
get_bit_le (const @I_TYPE@ *in_vector,unsigned int bit_addr)
{
  @I_TYPE@ x = in_vector[bit_addr>>LOG2_L_TYPE];
  return (x>>(bit_addr&(BITS_PER_TYPE-1)))&1;
}

unsigned int
get_bit_be (const @I_TYPE@ *in_vector,unsigned int bit_addr)
{
  @I_TYPE@ x = in_vector[bit_addr>>LOG2_L_TYPE];
  return (x>>((BITS_PER_TYPE-1)-(bit_addr&(BITS_PER_TYPE-1))))&1;
}

int
@NAME@::general_work (int noutput_items,
					gr_vector_int &ninput_items,
					gr_vector_const_void_star &input_items,
					gr_vector_void_star &output_items)
{
  unsigned int index_tmp = d_index;

  assert (input_items.size() == output_items.size());
  int nstreams = input_items.size();

  for (int m=0; m < nstreams; m++){
    const @I_TYPE@ *in = (@I_TYPE@ *) input_items[m];
    @O_TYPE@ *out = (@O_TYPE@ *) output_items[m];
    index_tmp = d_index;

    // per stream processing

    switch (d_endianness){

    case GR_MSB_FIRST:
      for (int i = 0; i < noutput_items; i++){
	//printf("here msb %d\n",i);
	@O_TYPE@ x = 0;
	for(unsigned int j=0; j<d_bits_per_chunk; j++, index_tmp++)
	  x = (x<<1) | get_bit_be(in, index_tmp);
	out[i] = x;
      }
      break;

    case GR_LSB_FIRST:
      for (int i = 0; i < noutput_items; i++){
	//printf("here lsb %d\n",i);
	@O_TYPE@ x = 0;
	for(unsigned int j=0; j<d_bits_per_chunk; j++, index_tmp++)
	  x = (x<<1) | get_bit_le(in, index_tmp);
	out[i] = x;
      }
      break;

    default:
      assert(0);
    }

    //printf("almost got to end\n");
    assert(ninput_items[m] >= (int) ((d_index+(BITS_PER_TYPE-1))>>LOG2_L_TYPE));
  }

  d_index = index_tmp;
  consume_each (d_index >> LOG2_L_TYPE);
  d_index = d_index & (BITS_PER_TYPE-1);
  //printf("got to end\n");
  return noutput_items;
}

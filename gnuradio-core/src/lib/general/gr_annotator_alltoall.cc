/* -*- c++ -*- */
/*
 * Copyright 2010 Free Software Foundation, Inc.
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

#include <gr_annotator_alltoall.h>
#include <gr_io_signature.h>
#include <string.h>
#include <iostream>
#include <iomanip>

gr_annotator_alltoall_sptr
gr_make_annotator_alltoall (int when, size_t sizeof_stream_item,
			    float rel_rate)
{
  return gnuradio::get_initial_sptr (new gr_annotator_alltoall
				     (when, sizeof_stream_item, rel_rate));
}

gr_annotator_alltoall::gr_annotator_alltoall (int when,
					      size_t sizeof_stream_item,
					      float rel_rate)
  : gr_block ("annotator_alltoall",
	      gr_make_io_signature (1, -1, sizeof_stream_item),
	      gr_make_io_signature (1, -1, sizeof_stream_item)),
    d_itemsize(sizeof_stream_item), d_rel_rate(rel_rate), d_when((uint64_t)when)
{
  set_tag_propagation_policy(TPP_ALL_TO_ALL);

  d_tag_counter = 0;
  set_relative_rate(d_rel_rate);
}

gr_annotator_alltoall::~gr_annotator_alltoall ()
{
}

int
gr_annotator_alltoall::general_work (int noutput_items,
				     gr_vector_int &ninput_items,
				     gr_vector_const_void_star &input_items,
				     gr_vector_void_star &output_items)
{
  const float *in = (const float *) input_items[0];
  float *out = (float *) output_items[0];

  std::stringstream str;
  str << name() << unique_id();

  uint64_t abs_N = 0;
  int ninputs = input_items.size();
  for(int i = 0; i < ninputs; i++) {
    abs_N = nitems_read(i);
    std::vector<pmt::pmt_t> all_tags = get_tags_in_range(i, abs_N, abs_N + noutput_items);

    std::vector<pmt::pmt_t>::iterator itr;
    for(itr = all_tags.begin(); itr != all_tags.end(); itr++) {
      d_stored_tags.push_back(*itr);
    }
  }

  // Source ID and key for any tag that might get applied from this block
  pmt::pmt_t srcid = pmt::pmt_string_to_symbol(str.str());
  pmt::pmt_t key = pmt::pmt_string_to_symbol("seq");

  // Work does nothing to the data stream; just copy all inputs to outputs
  // Adds a new tag when the number of items read is a multiple of d_when
  int noutputs = output_items.size();
  for(int j = 0; j < noutput_items; j++) {
    for(int i = 0; i < noutputs; i++) {
      if(abs_N % d_when == 0) {
	pmt::pmt_t value = pmt::pmt_from_uint64(d_tag_counter++);
	add_item_tag(i, abs_N, key, value, srcid);
      }

      // Sum all of the inputs together for each output. Just 'cause.
      out = (float*)output_items[i];
      out[j] = 0;
      for(int ins = 0; ins < ninputs; ins++) {
	in = (const float*)input_items[ins];
	out[j] += in[j];
      }
    }
    abs_N++;    
  }

  consume_each(noutput_items);
  return noutput_items;
}

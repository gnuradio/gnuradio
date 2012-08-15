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

#include <gr_annotator_raw.h>
#include <gr_io_signature.h>
#include <string.h>
#include <iostream>
#include <iomanip>
#include <stdexcept>

using namespace pmt;

gr_annotator_raw_sptr
gr_make_annotator_raw(size_t sizeof_stream_item)
{
  return gnuradio::get_initial_sptr(new gr_annotator_raw
				    (sizeof_stream_item));
}

gr_annotator_raw::gr_annotator_raw(size_t sizeof_stream_item)
  : gr_sync_block("annotator_raw",
		  gr_make_io_signature(1, 1, sizeof_stream_item),
		  gr_make_io_signature(1, 1, sizeof_stream_item)),
    d_itemsize(sizeof_stream_item)
{
  set_tag_propagation_policy(TPP_ONE_TO_ONE);
  set_relative_rate(1.0);
}

void gr_annotator_raw::add_tag(uint64_t offset, pmt_t key, pmt_t val)
{
  gruel::scoped_lock l(d_mutex);

  gr_tag_t tag;
  tag.srcid = pmt::pmt_intern(name());
  tag.key = key;
  tag.value = val;
  tag.offset = offset;

  // add our new tag
  d_queued_tags.push_back(tag);
  // make sure our tags are in offset order
  std::sort(d_queued_tags.begin(), d_queued_tags.end(),
	    gr_tag_t::offset_compare);
  // make sure we are not adding an item in the past!
  if(tag.offset > nitems_read(0)) {
    throw std::runtime_error("gr_annotator_raw::add_tag: item added too far in the past\n.");
  }
}

gr_annotator_raw::~gr_annotator_raw()
{
}

int
gr_annotator_raw::work(int noutput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items)
{
  gruel::scoped_lock l(d_mutex);

  const char *in = (const char*)input_items[0];
  char *out = (char*)output_items[0];

  uint64_t start_N = nitems_read(0);
  uint64_t end_N = start_N + (uint64_t)(noutput_items);
   
  // locate queued tags that fall in this range and insert them when appropriate
  std::vector<gr_tag_t>::iterator i = d_queued_tags.begin();
  while( i != d_queued_tags.end() ) {
    if( (*i).offset >= start_N && (*i).offset < end_N) {
      add_item_tag(0, (*i).offset,(*i).key, (*i).value, (*i).srcid);
      i = d_queued_tags.erase(i);
    } 
    else {
      break;
    }
  }

  // copy data across
  memcpy(out, in, noutput_items*d_itemsize);
  return noutput_items;
}

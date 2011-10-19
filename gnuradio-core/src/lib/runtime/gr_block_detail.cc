/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2010 Free Software Foundation, Inc.
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

#include <gr_block_detail.h>
#include <gr_buffer.h>

using namespace pmt;

static long s_ncurrently_allocated = 0;

long
gr_block_detail_ncurrently_allocated ()
{
  return s_ncurrently_allocated;
}

gr_block_detail::gr_block_detail (unsigned int ninputs, unsigned int noutputs)
  : d_produce_or(0),
    d_ninputs (ninputs), d_noutputs (noutputs),
    d_input (ninputs), d_output (noutputs),
    d_done (false)
{
  s_ncurrently_allocated++;
}

gr_block_detail::~gr_block_detail ()
{
  // should take care of itself
  s_ncurrently_allocated--;
}

void
gr_block_detail::set_input (unsigned int which, gr_buffer_reader_sptr reader)
{
  if (which >= d_ninputs)
    throw std::invalid_argument ("gr_block_detail::set_input");

  d_input[which] = reader;
}

void
gr_block_detail::set_output (unsigned int which, gr_buffer_sptr buffer)
{
  if (which >= d_noutputs)
    throw std::invalid_argument ("gr_block_detail::set_output");

  d_output[which] = buffer;
}

gr_block_detail_sptr
gr_make_block_detail (unsigned int ninputs, unsigned int noutputs)
{
  return gr_block_detail_sptr (new gr_block_detail (ninputs, noutputs));
}

void
gr_block_detail::set_done (bool done)
{
  d_done = done;
  for (unsigned int i = 0; i < d_noutputs; i++)
    d_output[i]->set_done (done);

  for (unsigned int i = 0; i < d_ninputs; i++)
    d_input[i]->set_done (done);
}

void 
gr_block_detail::consume (int which_input, int how_many_items)
{
  if (how_many_items > 0) {
    input (which_input)->update_read_pointer (how_many_items);
  }
}


void
gr_block_detail::consume_each (int how_many_items)
{
  if (how_many_items > 0) {
    for (int i = 0; i < ninputs (); i++) {
      d_input[i]->update_read_pointer (how_many_items);
    }
  }
}

void
gr_block_detail::produce (int which_output, int how_many_items)
{
  if (how_many_items > 0){
    d_output[which_output]->update_write_pointer (how_many_items);
    d_produce_or |= how_many_items;
  }
}

void
gr_block_detail::produce_each (int how_many_items)
{
  if (how_many_items > 0) {
    for (int i = 0; i < noutputs (); i++) {
      d_output[i]->update_write_pointer (how_many_items);
    }
    d_produce_or |= how_many_items;
  }
}


void
gr_block_detail::_post(pmt_t msg)
{
  d_tpb.insert_tail(msg);
}

uint64_t
gr_block_detail::nitems_read(unsigned int which_input) 
{
  if(which_input >= d_ninputs)
    throw std::invalid_argument ("gr_block_detail::n_input_items");
  return d_input[which_input]->nitems_read();
}

uint64_t
gr_block_detail::nitems_written(unsigned int which_output) 
{
  if(which_output >= d_noutputs)
    throw std::invalid_argument ("gr_block_detail::n_output_items");
  return d_output[which_output]->nitems_written();
}

void
gr_block_detail::add_item_tag(unsigned int which_output, const gr_tag_t &tag)
{
  if(!pmt_is_symbol(tag.key)) {
    throw pmt_wrong_type("gr_block_detail::add_item_tag key", tag.key);
  }
  else {
    // Add tag to gr_buffer's deque tags
    d_output[which_output]->add_item_tag(tag);
  }
}

void
gr_block_detail::get_tags_in_range(std::vector<gr_tag_t> &v,
				   unsigned int which_input,
				   uint64_t abs_start,
				   uint64_t abs_end)
{
  // get from gr_buffer_reader's deque of tags
  d_input[which_input]->get_tags_in_range(v, abs_start, abs_end);
}

void
gr_block_detail::get_tags_in_range(std::vector<gr_tag_t> &v,
				   unsigned int which_input,
				   uint64_t abs_start,
				   uint64_t abs_end,
				   const pmt_t &key)
{
  std::vector<gr_tag_t> found_items;
  
  v.resize(0);

  // get from gr_buffer_reader's deque of tags
  d_input[which_input]->get_tags_in_range(found_items, abs_start, abs_end);

  // Filter further by key name
  pmt_t itemkey;
  std::vector<gr_tag_t>::iterator itr;
  for(itr = found_items.begin(); itr != found_items.end(); itr++) {
    itemkey = (*itr).key;
    if(pmt_eqv(key, itemkey)) {
      v.push_back(*itr);
    }
  }
}

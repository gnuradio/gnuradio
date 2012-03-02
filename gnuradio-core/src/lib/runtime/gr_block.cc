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

#include <gr_block.h>
#include <gr_block_detail.h>
#include <stdexcept>
#include <iostream>

gr_block::gr_block (const std::string &name,
		    gr_io_signature_sptr input_signature,
		    gr_io_signature_sptr output_signature)
  : gr_basic_block(name, input_signature, output_signature),
    d_output_multiple (1),
    d_output_multiple_set(false),
    d_unaligned(0),
    d_is_unaligned(false),
    d_relative_rate (1.0),
    d_history(1),
    d_fixed_rate(false),
    d_tag_propagation_policy(TPP_ALL_TO_ALL)
{
}
  
gr_block::~gr_block ()
{
}

// stub implementation:  1:1

void
gr_block::forecast (int noutput_items, gr_vector_int &ninput_items_required)
{
  unsigned ninputs = ninput_items_required.size ();
  for (unsigned i = 0; i < ninputs; i++)
    ninput_items_required[i] = noutput_items + history() - 1;
}

// default implementation

bool
gr_block::start()
{
  return true;
}

bool
gr_block::stop()
{
  return true;
}

void
gr_block::set_output_multiple (int multiple)
{
  if (multiple < 1)
    throw std::invalid_argument ("gr_block::set_output_multiple");

  d_output_multiple_set = true;
  d_output_multiple = multiple;
}

void
gr_block::set_alignment (int multiple)
{
  if (multiple < 1)
    throw std::invalid_argument ("gr_block::set_alignment_multiple");

  d_output_multiple = multiple;
}

void
gr_block::set_unaligned (int na)
{
  // unaligned value must be less than 0 and it doesn't make sense
  // that it's larger than the alignment value.
  if ((na < 0) || (na > d_output_multiple))
    throw std::invalid_argument ("gr_block::set_unaligned");

  d_unaligned = na;
}

void 
gr_block::set_is_unaligned (bool u)
{
  d_is_unaligned = u;
}

void
gr_block::set_relative_rate (double relative_rate)
{
  if (relative_rate < 0.0)
    throw std::invalid_argument ("gr_block::set_relative_rate");
  
  d_relative_rate = relative_rate;
}


void
gr_block::consume (int which_input, int how_many_items)
{
  d_detail->consume (which_input, how_many_items);
}

void
gr_block::consume_each (int how_many_items)
{
  d_detail->consume_each (how_many_items);
}

void
gr_block::produce (int which_output, int how_many_items)
{
  d_detail->produce (which_output, how_many_items);
}

int
gr_block::fixed_rate_ninput_to_noutput(int ninput)
{
  throw std::runtime_error("Unimplemented");
}

int
gr_block::fixed_rate_noutput_to_ninput(int noutput)
{
  throw std::runtime_error("Unimplemented");
}

uint64_t
gr_block::nitems_read(unsigned int which_input) 
{
  if(d_detail) {
    return d_detail->nitems_read(which_input);
  }
  else {
    //throw std::runtime_error("No block_detail associated with block yet");
    return 0;
  }
}

uint64_t
gr_block::nitems_written(unsigned int which_output) 
{
  if(d_detail) {
    return d_detail->nitems_written(which_output);
  }
  else {
    //throw std::runtime_error("No block_detail associated with block yet");
    return 0;
  }
}

void
gr_block::add_item_tag(unsigned int which_output,
		       const gr_tag_t &tag)
{
  d_detail->add_item_tag(which_output, tag);
}

void
gr_block::get_tags_in_range(std::vector<gr_tag_t> &v,
			    unsigned int which_output,
			    uint64_t start, uint64_t end)
{
  d_detail->get_tags_in_range(v, which_output, start, end);
}
  
void
gr_block::get_tags_in_range(std::vector<gr_tag_t> &v,
			    unsigned int which_output,
			    uint64_t start, uint64_t end,
			    const pmt::pmt_t &key)
{
  d_detail->get_tags_in_range(v, which_output, start, end, key);
}

gr_block::tag_propagation_policy_t
gr_block::tag_propagation_policy()
{
  return d_tag_propagation_policy;
}

void
gr_block::set_tag_propagation_policy(tag_propagation_policy_t p)
{
  d_tag_propagation_policy = p;
}

std::ostream&
operator << (std::ostream& os, const gr_block *m)
{
  os << "<gr_block " << m->name() << " (" << m->unique_id() << ")>";
  return os;
}


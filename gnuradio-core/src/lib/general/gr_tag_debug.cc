/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#include <gr_tag_debug.h>
#include <gr_io_signature.h>
#include <iostream>
#include <iomanip>

gr_tag_debug_sptr
gr_make_tag_debug(size_t sizeof_stream_item, const std::string &name)
{
  return gnuradio::get_initial_sptr
    (new gr_tag_debug(sizeof_stream_item, name));
}

gr_tag_debug::gr_tag_debug(size_t sizeof_stream_item, const std::string &name)
  : gr_sync_block("tag_debug",
		  gr_make_io_signature(1, -1, sizeof_stream_item),
		  gr_make_io_signature(0, 0, 0)),
    d_name(name), d_display(true)
{
}

std::vector<gr_tag_t>
gr_tag_debug::current_tags()
{
  gruel::scoped_lock l(d_mutex);
  return d_tags;
}

void
gr_tag_debug::set_display(bool d)
{
  gruel::scoped_lock l(d_mutex);
  d_display = d;
}

int
gr_tag_debug::work(int noutput_items,
		   gr_vector_const_void_star &input_items,
		   gr_vector_void_star &output_items)
{
  gruel::scoped_lock l(d_mutex);

  if(d_display) {
    std::cout << std::endl
	      << "----------------------------------------------------------------------";
    std::cout << std::endl << "Tag Debug: " << d_name << std::endl;
  }

  uint64_t abs_N, end_N;
  for(size_t i = 0; i < input_items.size(); i++) {
    abs_N = nitems_read(i);
    end_N = abs_N + (uint64_t)(noutput_items);

    d_tags.clear();
    get_tags_in_range(d_tags, i, abs_N, end_N);

    if(d_display) {
      std::cout << "Input Stream: " << i << std::endl;
      for(d_tags_itr = d_tags.begin(); d_tags_itr != d_tags.end(); d_tags_itr++) {
	std::cout << std::setw(10) << "Offset: " << d_tags_itr->offset
		  << std::setw(10) << "Source: " << pmt::pmt_symbol_to_string(d_tags_itr->srcid)
		  << std::setw(10) << "Key: " << pmt::pmt_symbol_to_string(d_tags_itr->key)
		  << std::setw(10) << "Value: ";
	pmt::pmt_print(d_tags_itr->value);
      }
    }
  }

  if(d_display) {
    std::cout << "----------------------------------------------------------------------";
    std::cout << std::endl;
  }

  return noutput_items;
}

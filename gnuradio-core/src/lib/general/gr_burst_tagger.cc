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

#include <gr_burst_tagger.h>
#include <gr_io_signature.h>
#include <string.h>

gr_burst_tagger_sptr
gr_make_burst_tagger(size_t itemsize)
{
  return gnuradio::get_initial_sptr(new gr_burst_tagger(itemsize));
}

gr_burst_tagger::gr_burst_tagger(size_t itemsize)
  : gr_sync_block ("burst_tagger",
		   gr_make_io_signature2 (2, 2, itemsize, sizeof(short)),
		   gr_make_io_signature (1, 1, itemsize)),
    d_itemsize(itemsize), d_state(false)
{
  std::stringstream str;
  str << name() << unique_id();

  d_true_key = pmt::pmt_string_to_symbol("burst");
  d_true_value = pmt::PMT_T;
  
  d_false_key = pmt::pmt_string_to_symbol("burst");
  d_false_value = pmt::PMT_F;
  
  d_id  = pmt::pmt_string_to_symbol(str.str());
}

void
gr_burst_tagger::set_true_tag (const std::string &key, bool value)
{
  d_true_key = pmt::pmt_string_to_symbol(key);
  if(value == true) {
    d_true_value = pmt::PMT_T;
  }
  else {
    d_true_value = pmt::PMT_F;
  }
}

void
gr_burst_tagger::set_false_tag (const std::string &key, bool value)
{
  d_false_key = pmt::pmt_string_to_symbol(key);
  if(value == true) {
    d_false_value = pmt::PMT_T;
  }
  else {
    d_false_value = pmt::PMT_F;
  }
}

gr_burst_tagger::~gr_burst_tagger()
{
}

int
gr_burst_tagger::work(int noutput_items,
		      gr_vector_const_void_star &input_items,
		      gr_vector_void_star &output_items)
{
  const char *signal  = (const char*)input_items[0];
  const short *trigger = (const short*)input_items[1];
  char *out = (char*)output_items[0];

  memcpy(out, signal, noutput_items * d_itemsize);

  for(int i = 0; i < noutput_items; i++) {
    if(trigger[i] > 0) {
      if(d_state == false) {
	d_state = true;
	add_item_tag(0, nitems_written(0)+i, d_true_key, d_true_value, d_id);
      }
    }
    else {
      if(d_state == true) {
	d_state = false;
	add_item_tag(0, nitems_written(0)+i, d_false_key, d_false_value, d_id);
      }
    }
  }
  return noutput_items;
}

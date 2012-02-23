/* -*- c++ -*- */
/*
 * Copyright 2007,2010,2012 Free Software Foundation, Inc.
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

#include <gr_delay.h>
#include <gr_io_signature.h>
#include <string.h>

gr_delay_sptr
gr_make_delay (size_t itemsize, int delay)
{
  return gnuradio::get_initial_sptr(new gr_delay (itemsize, delay));
}

gr_delay::gr_delay (size_t itemsize, int delay)
  : gr_block ("delay",
	      gr_make_io_signature (1, -1, itemsize),
	      gr_make_io_signature (1, -1, itemsize)),
    d_itemsize(itemsize)
{
  set_delay(delay);
  d_delta = 0;
}

void 
gr_delay::forecast (int noutput_items, gr_vector_int &ninput_items_required)
{
  // make sure all inputs have noutput_items available
  unsigned ninputs = ninput_items_required.size ();
  for (unsigned i = 0; i < ninputs; i++)
    ninput_items_required[i] = noutput_items;
}

void
gr_delay::set_delay (int d)
{
  // only set a new delta if there is a change in the delay; this
  // protects from quickly-repeated calls to this function that would
  // end with d_delta=0.
  if(d != delay()) {
    gruel::scoped_lock l(d_mutex_delay);
    int old = delay();
    set_history(d+1);
    d_delta += delay() - old;
  }
}

int
gr_delay::general_work (int noutput_items,
			gr_vector_int &ninput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
{
  gruel::scoped_lock l(d_mutex_delay);
  assert(input_items.size() == output_items.size());

  const char *iptr;
  char *optr;
  int cons, ret;

  // No change in delay; just memcpy ins to outs
  if(d_delta == 0) {
    for(size_t i = 0; i < input_items.size(); i++) {
      iptr = (const char *) input_items[i];
      optr = (char *) output_items[i];
      std::memcpy(optr, iptr, noutput_items*d_itemsize);
    }
    cons = noutput_items;
    ret = noutput_items;
  }

  // Skip over d_delta items on the input
  else if(d_delta < 0) {
    int n_to_copy, n_adj;
    int delta = -d_delta;
    n_to_copy = std::max(0, noutput_items-delta);
    n_adj = std::min(delta, noutput_items);
    for(size_t i = 0; i < input_items.size(); i++) {
      iptr = (const char *) input_items[i];
      optr = (char *) output_items[i];
      std::memcpy(optr, iptr+delta*d_itemsize, n_to_copy*d_itemsize);
    }
    cons = noutput_items;
    ret = n_to_copy;
    delta -= n_adj;
    d_delta = -delta;
  }
  
  //produce but not consume (inserts zeros)
  else {  // d_delta > 0
    int n_from_input, n_padding;
    n_from_input = std::max(0, noutput_items-d_delta);
    n_padding = std::min(d_delta, noutput_items);
    for(size_t i = 0; i < input_items.size(); i++) {
      iptr = (const char *) input_items[i];
      optr = (char *) output_items[i];
      std::memset(optr, 0, n_padding*d_itemsize);
      std::memcpy(optr, iptr, n_from_input*d_itemsize);
    }
    cons = n_from_input;
    ret = noutput_items;
    d_delta -= n_padding;
  }

  consume_each(cons);
  return ret;
}

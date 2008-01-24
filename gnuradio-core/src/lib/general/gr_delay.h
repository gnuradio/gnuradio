/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_DELAY_H
#define INCLUDED_GR_DELAY_H

#include <gr_sync_block.h>

class gr_delay;
typedef boost::shared_ptr<gr_delay> gr_delay_sptr;

gr_delay_sptr gr_make_delay (size_t itemsize, int delay);

/*!
 * \brief delay the input by a certain number of samples
 * \ingroup misc
 */
class gr_delay : public gr_sync_block
{
  friend gr_delay_sptr gr_make_delay (size_t itemsize, int delay);

  gr_delay (size_t itemsize, int delay);

  size_t d_itemsize;

 public:
  int delay () const { return history()-1; }
  void set_delay (int delay) { set_history(delay+1); }

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif

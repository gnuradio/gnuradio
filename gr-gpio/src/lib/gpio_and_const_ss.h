/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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


#ifndef INCLUDED_GPIO_AND_CONST_SS_H
#define INCLUDED_GPIO_AND_CONST_SS_H

#include <gr_sync_block.h>

class gpio_and_const_ss;
typedef boost::shared_ptr<gpio_and_const_ss> gpio_and_const_ss_sptr;

gpio_and_const_ss_sptr gpio_make_and_const_ss (unsigned short k);

/*!
 * \brief output = input & constant
 * \ingroup block
 */
class gpio_and_const_ss : public gr_sync_block
{
  friend gpio_and_const_ss_sptr gpio_make_and_const_ss (unsigned short k);

  unsigned short	d_k;		// the constant
  gpio_and_const_ss (unsigned short k);

 public:
  unsigned short k () const { return d_k; }
  void set_k (unsigned short k) { d_k = k; }

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif

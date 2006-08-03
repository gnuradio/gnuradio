/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef INCLUDED_GR_DIFF_PHASOR_CC_H
#define INCLUDED_GR_DIFF_PHASOR_CC_H

#include <gr_sync_block.h>

class gr_diff_phasor_cc;
typedef boost::shared_ptr<gr_diff_phasor_cc> gr_diff_phasor_cc_sptr;

gr_diff_phasor_cc_sptr gr_make_diff_phasor_cc ();


class gr_diff_phasor_cc : public gr_sync_block
{
  friend gr_diff_phasor_cc_sptr gr_make_diff_phasor_cc ();

  gr_diff_phasor_cc ();  //constructor

 public:
  ~gr_diff_phasor_cc();	//destructor

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif

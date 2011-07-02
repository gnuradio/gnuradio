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

// WARNING: this file is machine generated.  Edits will be over written

#ifndef INCLUDED_GR_SAMPLE_AND_HOLD_II_H
#define INCLUDED_GR_SAMPLE_AND_HOLD_II_H

#include <gr_sync_block.h>

class gr_sample_and_hold_ii;
typedef boost::shared_ptr<gr_sample_and_hold_ii> gr_sample_and_hold_ii_sptr;

gr_sample_and_hold_ii_sptr gr_make_sample_and_hold_ii ();

/*!
 * \brief sample and hold circuit
 * \ingroup level_blk
 *
 * Samples the data stream (input stream 0) and holds the value
 * if the control signal is 1 (intput stream 1).
 */
class gr_sample_and_hold_ii : public gr_sync_block
{
  friend gr_sample_and_hold_ii_sptr gr_make_sample_and_hold_ii ();

  gr_sample_and_hold_ii ();

 private:
  int d_data;

 public:
  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif

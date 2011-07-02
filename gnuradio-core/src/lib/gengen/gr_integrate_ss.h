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

#ifndef INCLUDED_GR_INTEGRATE_SS_H
#define INCLUDED_GR_INTEGRATE_SS_H

#include <gr_sync_decimator.h>

class gr_integrate_ss;

typedef boost::shared_ptr<gr_integrate_ss> gr_integrate_ss_sptr;

gr_integrate_ss_sptr gr_make_integrate_ss (int decim);

/*!
 * \brief output = sum(input[0]...input[n])
 * \ingroup math_blk
 *
 * Integrate successive samples in input stream and decimate
 */
class gr_integrate_ss : public gr_sync_decimator
{
private:
  friend gr_integrate_ss_sptr gr_make_integrate_ss(int decim);

  gr_integrate_ss (int decim);

  int d_decim;
  int d_count;

public:
  ~gr_integrate_ss ();	

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif /* INCLUDED_GR_INTEGRATE_SS_H */

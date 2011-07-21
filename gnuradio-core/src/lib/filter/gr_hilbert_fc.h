/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_HILBERT_FC_H
#define INCLUDED_GR_HILBERT_FC_H

#include <gr_core_api.h>
#include <gr_sync_block.h>
#include <gr_io_signature.h>
#include <gr_types.h>

class gr_hilbert_fc;
typedef boost::shared_ptr<gr_hilbert_fc> gr_hilbert_fc_sptr;

// public constructor
GR_CORE_API gr_hilbert_fc_sptr gr_make_hilbert_fc (unsigned int ntaps);


class gr_fir_fff;

/*!
 * \brief Hilbert transformer.
 * \ingroup filter_blk
 *
 * real output is input appropriately delayed.
 * imaginary output is hilbert filtered (90 degree phase shift)
 * version of input.
 */
class GR_CORE_API gr_hilbert_fc : public gr_sync_block
{
 public:
  ~gr_hilbert_fc ();

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);

 protected:
  gr_hilbert_fc (unsigned int ntaps);

 private:
  unsigned int	 d_ntaps;
  gr_fir_fff	*d_hilb;

  friend GR_CORE_API gr_hilbert_fc_sptr gr_make_hilbert_fc (unsigned int ntaps);
};



#endif /* INCLUDED_GR_HILBERT_FC_H */

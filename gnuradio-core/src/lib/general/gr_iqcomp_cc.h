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


#ifndef INCLUDED_GR_IQCOMP_CC_H
#define INCLUDED_GR_IQCOMP_CC_H

#include <gr_core_api.h>
#include <gr_sync_block.h>

class gr_iqcomp_cc;
typedef boost::shared_ptr<gr_iqcomp_cc> gr_iqcomp_cc_sptr;

GR_CORE_API gr_iqcomp_cc_sptr gr_make_iqcomp_cc (float mu);

/*!
 * \brief 
 * \ingroup misc_blk
 */
class GR_CORE_API gr_iqcomp_cc : public gr_sync_block
{
  friend GR_CORE_API gr_iqcomp_cc_sptr gr_make_iqcomp_cc (float mu);

  float	d_mu, d_wi, d_wq;
  gr_iqcomp_cc (float mu);

 public:
  float mu () const { return d_mu; }
  void set_mu (float mu) { d_mu = mu; }

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif

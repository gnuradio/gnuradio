/* -*- c++ -*- */
/*
 * Copyright 2005 Free Software Foundation, Inc.
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
#ifndef INCLUDED_GR_RMS_FF_H
#define INCLUDED_GR_RMS_FF_H

#include <gr_core_api.h>
#include <gr_sync_block.h>
#include <gr_single_pole_iir.h>

class gr_rms_ff;
typedef boost::shared_ptr<gr_rms_ff> gr_rms_ff_sptr;

GR_CORE_API gr_rms_ff_sptr
gr_make_rms_ff (double alpha = 0.0001);

/*!
 * \brief RMS average power
 * \ingroup math_blk
 */
class GR_CORE_API gr_rms_ff : public gr_sync_block
{
  gr_single_pole_iir<double,double,double>	d_iir;
  bool						d_unmuted;

  friend GR_CORE_API gr_rms_ff_sptr
  gr_make_rms_ff (double alpha);

  gr_rms_ff (double alpha);

public:
  ~gr_rms_ff ();

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);

  bool unmuted () const { return d_unmuted; }

  void set_alpha (double alpha);
};

#endif /* INCLUDED_GR_RMS_FF_H */

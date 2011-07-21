/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
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
#ifndef INCLUDED_GR_PA_2X2_PHASE_COMBINER_H
#define INCLUDED_GR_PA_2X2_PHASE_COMBINER_H

#include <gr_core_api.h>
#include <gr_sync_block.h>

class gr_pa_2x2_phase_combiner;
typedef boost::shared_ptr<gr_pa_2x2_phase_combiner> gr_pa_2x2_phase_combiner_sptr;

GR_CORE_API gr_pa_2x2_phase_combiner_sptr gr_make_pa_2x2_phase_combiner ();

/*!
 * \brief pa_2x2 phase combiner
 * \ingroup misc_blk
 *
 * Anntenas are arranged like this:
 *
 *     2  3
 *     0  1
 *
 * dx and dy are lambda/2.
 */
class GR_CORE_API gr_pa_2x2_phase_combiner : public gr_sync_block
{
  static const int	NM = 4;

  float		d_theta;
  gr_complex	d_phase[NM];

  gr_pa_2x2_phase_combiner ();
  friend GR_CORE_API gr_pa_2x2_phase_combiner_sptr gr_make_pa_2x2_phase_combiner();

 public:
  float theta() const { return d_theta; }
  void set_theta(float theta);

  int work(int noutput_items,
	   gr_vector_const_void_star &input_items,
	   gr_vector_void_star &output_items);
};

#endif /* INCLUDED_GR_PA_2X2_PHASE_COMBINER_H */

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

#ifndef INCLUDED_GR_REGENERATE_BB_H
#define INCLUDED_GR_REGENERATE_BB_H

#include <gr_core_api.h>
#include <gr_sync_block.h>

class gr_regenerate_bb;
typedef boost::shared_ptr<gr_regenerate_bb> gr_regenerate_bb_sptr;

GR_CORE_API gr_regenerate_bb_sptr gr_make_regenerate_bb (int period, unsigned int max_regen=500);

/*!
 * \brief Detect the peak of a signal and repeat every period samples
 * \ingroup level_blk
 *
 * If a peak is detected, this block outputs a 1 repeated every period samples 
 * until reset by detection of another 1 on the input or stopped after max_regen
 * regenerations have occurred.
 *
 * Note that if max_regen=(-1)/ULONG_MAX then the regeneration will run forever.
 */
class GR_CORE_API gr_regenerate_bb : public gr_sync_block
{
  /*!
   * \brief Make a regenerate block
   * \param period The number of samples between regenerations
   * \param max_regen The maximum number of regenerations to perform; if set to 
   * ULONG_MAX, it will regenerate continuously.
   */
  friend GR_CORE_API gr_regenerate_bb_sptr gr_make_regenerate_bb (int period, unsigned int max_regen);

  gr_regenerate_bb (int period, unsigned int max_regen);

 private:
  int d_period;
  int d_countdown;
  unsigned int d_max_regen;
  unsigned int d_regen_count;

 public:
  /*! \brief Reset the maximum regeneration count; this will reset the current regen.
   */
  void set_max_regen(unsigned int regen);

  /*! \brief Reset the period of regenerations; this will reset the current regen.
   */
  void set_period(int period);

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif

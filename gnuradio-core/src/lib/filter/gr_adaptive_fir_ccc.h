/* -*- c++ -*- */
/*
 * Copyright 2011 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_ADAPTIVE_FIR_CCC_H
#define	INCLUDED_GR_ADAPTIVE_FIR_CCC_H

#include <gr_core_api.h>
#include <gr_sync_decimator.h>

/*!
 * \brief Adaptive FIR filter with gr_complex input, gr_complex output and float taps
 * \ingroup filter_blk
 */
class GR_CORE_API gr_adaptive_fir_ccc : public gr_sync_decimator
{
private:
  std::vector<gr_complex>  d_new_taps;
  bool                     d_updated;

protected:
  gr_complex	           d_error;
  std::vector<gr_complex>  d_taps;

  // Override to calculate error signal per output
  virtual gr_complex error(const gr_complex &out) = 0; 

  // Override to calculate new weight from old, corresponding input
  virtual void update_tap(gr_complex &tap, const gr_complex &in) = 0;

  gr_complex filter(gr_complex *x);

  gr_adaptive_fir_ccc(const char *name, int decimation,
		      const std::vector<gr_complex> &taps);

public:
  void set_taps(const std::vector<gr_complex> &taps);

  int work(int noutput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);
};

#endif

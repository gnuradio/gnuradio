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

#ifndef INCLUDED_GR_CMA_EQUALIZER_CC_H
#define	INCLUDED_GR_CMA_EQUALIZER_CC_H

#include <gr_adaptive_fir_ccf.h>

class gr_cma_equalizer_cc;
typedef boost::shared_ptr<gr_cma_equalizer_cc> gr_cma_equalizer_cc_sptr;

gr_cma_equalizer_cc_sptr 
gr_make_cma_equalizer_cc(int num_taps, float modulus, float mu);

/*!
 * \brief Implements constant modulus adaptive filter on complex stream
 * \ingroup filter
 */
class gr_cma_equalizer_cc : public gr_adaptive_fir_ccf
{
private:
  float d_modulus;
  float d_mu;
  
  friend gr_cma_equalizer_cc_sptr gr_make_cma_equalizer_cc(int num_taps, float modulus, float mu);
  gr_cma_equalizer_cc(int num_taps, float modulus, float mu);

protected:

  virtual float error(const gr_complex &out) 
  { 
    return (d_modulus - norm(out)); 
  }

  virtual void update_tap(float &tap, const gr_complex &in) 
  {
    tap += d_mu*d_error*abs(in);
  }
  
public:
};

#endif

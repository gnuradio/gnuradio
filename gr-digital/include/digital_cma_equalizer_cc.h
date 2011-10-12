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

#ifndef INCLUDED_DIGITAL_CMA_EQUALIZER_CC_H
#define	INCLUDED_DIGITAL_CMA_EQUALIZER_CC_H

#include <digital_api.h>
#include <gr_adaptive_fir_ccc.h>
#include <gr_math.h>
#include <iostream>

class digital_cma_equalizer_cc;
typedef boost::shared_ptr<digital_cma_equalizer_cc> digital_cma_equalizer_cc_sptr;

DIGITAL_API digital_cma_equalizer_cc_sptr 
digital_make_cma_equalizer_cc(int num_taps, float modulus, float mu, int sps);

/*!
 * \brief Implements constant modulus adaptive filter on complex stream
 * \ingroup eq_blk
 * \ingroup digital
 *
 * The error value and tap update equations (for p=2) can be found in:
 *
 * D. Godard, "Self-Recovering Equalization and Carrier Tracking in
 * Two-Dimensional Data Communication Systems," IEEE Transactions on
 * Communications, Vol. 28, No. 11, pp. 1867 - 1875, 1980,
 */
class DIGITAL_API digital_cma_equalizer_cc : public gr_adaptive_fir_ccc
{
private:
  float d_modulus;
  float d_mu;
  
  friend DIGITAL_API digital_cma_equalizer_cc_sptr digital_make_cma_equalizer_cc(int num_taps,
								     float modulus,
								     float mu,
								     int sps);
  digital_cma_equalizer_cc(int num_taps, float modulus, float mu, int sps);

protected:

  virtual gr_complex error(const gr_complex &out) 
  { 
    gr_complex error = out*(norm(out) - d_modulus);
    float re = gr_clip(error.real(), 1.0);
    float im = gr_clip(error.imag(), 1.0);
    return gr_complex(re, im);
  }

  virtual void update_tap(gr_complex &tap, const gr_complex &in) 
  {
    // Hn+1 = Hn - mu*conj(Xn)*zn*(|zn|^2 - 1)
    tap -= d_mu*conj(in)*d_error;
  }
  
public:
  float get_gain() 
  {
    return d_mu;
  }

  void set_gain(float mu) 
  {
    if(mu < 0.0f || mu > 1.0f) {
      throw std::out_of_range("digital_cma_equalizer::set_gain: Gain value must be in [0,1]");
    }
    d_mu = mu;
  }
    
  float get_modulus() 
  {
    return d_modulus;
  }

  void set_modulus(float mod) 
  {
    if(mod < 0)
      throw std::out_of_range("digital_cma_equalizer::set_modulus: Modulus value must be >= 0");
    d_modulus = mod;
  }
};

#endif

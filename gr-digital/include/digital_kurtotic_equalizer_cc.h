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

#ifndef INCLUDED_DIGITAL_KURTOTIC_EQUALIZER_CC_H
#define	INCLUDED_DIGITAL_KURTOTIC_EQUALIZER_CC_H

#include <digital_api.h>
#include <gr_adaptive_fir_ccc.h>
#include <gr_math.h>
#include <iostream>

class digital_kurtotic_equalizer_cc;
typedef boost::shared_ptr<digital_kurtotic_equalizer_cc> digital_kurtotic_equalizer_cc_sptr;

DIGITAL_API digital_kurtotic_equalizer_cc_sptr 
digital_make_kurtotic_equalizer_cc(int num_taps, float mu);

/*!
 * \brief Implements a kurtosis-based adaptive equalizer on complex stream
 * \ingroup eq_blk
 * \ingroup digital
 *
 * Y. Guo, J. Zhao, Y. Sun, "Sign kurtosis maximization based blind
 * equalization algorithm," IEEE Conf. on Control, Automation,
 * Robotics and Vision, Vol. 3, Dec. 2004, pp. 2052 - 2057.
 */
class DIGITAL_API digital_kurtotic_equalizer_cc : public gr_adaptive_fir_ccc
{
private:
  float d_mu;
  float d_p, d_m;
  gr_complex d_q, d_u;
  float d_alpha_p, d_alpha_q, d_alpha_m;
  
  friend DIGITAL_API digital_kurtotic_equalizer_cc_sptr digital_make_kurtotic_equalizer_cc(int num_taps,
									       float mu);
  digital_kurtotic_equalizer_cc(int num_taps, float mu);

  gr_complex sign(gr_complex x)
  {
    float re = (float)(x.real() >= 0.0f);
    float im = (float)(x.imag() >= 0.0f);
    return gr_complex(re, im);
  }

protected:

  virtual gr_complex error(const gr_complex &out) 
  {

    // p = E[|z|^2]
    // q = E[z^2]
    // m = E[|z|^4]
    // u = E[kurtosis(z)]

    float nrm = norm(out);
    gr_complex cnj = conj(out);
    float epsilon_f = 1e-12;
    gr_complex epsilon_c = gr_complex(1e-12, 1e-12);

    
    d_p = (1-d_alpha_p)*d_p + (d_alpha_p)*nrm + epsilon_f;
    d_q = (1-d_alpha_q)*d_q + (d_alpha_q)*out*out + epsilon_c;
    d_m = (1-d_alpha_m)*d_m + (d_alpha_m)*nrm*nrm + epsilon_f;
    d_u = d_m - 2.0f*(d_p*d_p) - d_q*d_q;

    gr_complex F = (1.0f / (d_p*d_p*d_p)) *
      (sign(d_u) * (nrm*cnj - 2.0f*d_p*cnj - conj(d_q)*out) -
       abs(d_u)*cnj);

    //std::cout << "out: " << out << "   p: " << d_p << "   q: " << d_q;
    //std::cout << "   m: " << d_m << "   u: " << d_u << std::endl;
    //std::cout << "error: " << F << std::endl;
    
    float re = gr_clip(F.real(), 1.0);
    float im = gr_clip(F.imag(), 1.0);
    return gr_complex(re, im);
  }

  virtual void update_tap(gr_complex &tap, const gr_complex &in) 
  {
    tap += d_mu*in*d_error;
  }
  
public:
  void set_gain(float mu) 
  {
    if(mu < 0)
      throw std::out_of_range("digital_kurtotic_equalizer::set_gain: Gain value must be >= 0");
    d_mu = mu;
  }
};

#endif

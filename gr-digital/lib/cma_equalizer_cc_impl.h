/* -*- c++ -*- */
/*
 * Copyright 2011,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_CMA_EQUALIZER_CC_IMPL_H
#define	INCLUDED_DIGITAL_CMA_EQUALIZER_CC_IMPL_H

#include <gnuradio/digital/cma_equalizer_cc.h>
#include <gnuradio/filter/fir_filter.h>
#include <gnuradio/math.h>
#include <stdexcept>

namespace gr {
  namespace digital {

    class cma_equalizer_cc_impl
      : public cma_equalizer_cc, filter::kernel::fir_filter_ccc
    {
    private:
      std::vector<gr_complex> d_new_taps;
      bool d_updated;
      gr_complex d_error;

      float d_modulus;
      float d_mu;

    protected:
      gr_complex error(const gr_complex &out);
      void update_tap(gr_complex &tap, const gr_complex &in);
  
    public:
      cma_equalizer_cc_impl(int num_taps, float modulus, float mu, int sps);
      ~cma_equalizer_cc_impl();

      void set_taps(const std::vector<gr_complex> &taps);
      std::vector<gr_complex> taps() const;

      float gain() const
      {
	return d_mu;
      }

      void set_gain(float mu) 
      {
	if(mu < 0.0f || mu > 1.0f) {
	  throw std::out_of_range("cma_equalizer::set_gain: Gain value must be in [0,1]");
	}
	d_mu = mu;
      }
    
      float modulus() const
      {
	return d_modulus;
      }

      void set_modulus(float mod) 
      {
	if(mod < 0)
	  throw std::out_of_range("cma_equalizer::set_modulus: Modulus value must be >= 0");
	d_modulus = mod;
      }

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_CMA_EQUALIZER_CC_IMPL_H */

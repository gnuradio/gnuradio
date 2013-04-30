/* -*- c++ -*- */
/*
 * Copyright 2010,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_CPMMOD_BC_IMPL_H
#define INCLUDED_DIGITAL_CPMMOD_BC_IMPL_H

#include <gnuradio/digital/cpmmod_bc.h>
#include <gnuradio/blocks/char_to_float.h>
#include <gnuradio/analog/frequency_modulator_fc.h>
#include <gnuradio/filter/interp_fir_filter_fff.h>

namespace gr {
  namespace digital {

    class cpmmod_bc_impl : public cpmmod_bc
    {
    private:
      int d_type;
      float d_index;
      int d_sps;
      int d_length;
      double d_beta;

    protected:
      std::vector<float> d_taps;
      gr::blocks::char_to_float::sptr d_char_to_float;
      gr::filter::interp_fir_filter_fff::sptr d_pulse_shaper;
      analog::frequency_modulator_fc::sptr d_fm;
  
    public:
      cpmmod_bc_impl(const std::string &name,
		     analog::cpm::cpm_type type, float h,
		     int samples_per_sym,
		     int L, double beta=0.3);
      ~cpmmod_bc_impl();

      std::vector<float> taps() const;
      int type() const;
      float index() const;
      int samples_per_sym() const;
      int length() const;
      double beta() const;
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_CPMMOD_BC_IMPL_H */


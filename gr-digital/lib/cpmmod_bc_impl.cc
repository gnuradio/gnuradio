/* -*- c++ -*- */
/*
 * Copyright 2010,2012 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "cpmmod_bc_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
  namespace digital {

    cpmmod_bc::sptr
    cpmmod_bc::make(analog::cpm::cpm_type type, float h,
		    int samples_per_sym,
		    int L, double beta)
    {
      return gnuradio::get_initial_sptr
	(new cpmmod_bc_impl("cpmmod_bc",
			    (analog::cpm::cpm_type)type,
			    h, samples_per_sym,
			    L, beta));
    }

    cpmmod_bc::sptr
    cpmmod_bc::make_gmskmod_bc(int samples_per_sym,
			       int L, double beta)
    {
      return gnuradio::get_initial_sptr
	(new cpmmod_bc_impl("gmskmod_bc",
			    analog::cpm::GAUSSIAN, 0.5,
			    samples_per_sym,
			    L, beta));
    }

    cpmmod_bc_impl::cpmmod_bc_impl(const std::string &name,
				   analog::cpm::cpm_type type, float h,
				   int samples_per_sym,
				   int L, double beta)
      : hier_block2(name,
		       io_signature::make(1, 1, sizeof(char)),
		       io_signature::make2(1, 1, sizeof(gr_complex), sizeof(float))),
	d_type(type), d_index(h), d_sps(samples_per_sym), d_length(L), d_beta(beta),
	d_taps(analog::cpm::phase_response(type, samples_per_sym, L, beta)),
	d_char_to_float(blocks::char_to_float::make()),
	d_pulse_shaper(filter::interp_fir_filter_fff::make(samples_per_sym, d_taps)),
	d_fm(analog::frequency_modulator_fc::make(M_PI * h))
    {
      switch(type) {
      case analog::cpm::LRC:
      case analog::cpm::LSRC:
      case analog::cpm::LREC:
      case analog::cpm::TFM:
      case analog::cpm::GAUSSIAN:
	break;
    
      default:
	throw std::invalid_argument("cpmmod_bc_impl: invalid CPM type");
      }
  
      connect(self(), 0, d_char_to_float, 0);
      connect(d_char_to_float, 0, d_pulse_shaper, 0);
      connect(d_pulse_shaper, 0, d_fm, 0);
      connect(d_fm, 0, self(), 0);
    }

    cpmmod_bc_impl::~cpmmod_bc_impl()
    {
    }

    std::vector<float>
    cpmmod_bc_impl::taps() const
    {
      return d_taps;
    }

    int
    cpmmod_bc_impl::type() const
    {
      return d_type;
    }

    float
    cpmmod_bc_impl::index() const
    {
      return d_index;
    }

    int
    cpmmod_bc_impl::samples_per_sym() const
    {
      return d_sps;
    }

    int
    cpmmod_bc_impl::length() const
    {
      return d_length;
    }

    double cpmmod_bc_impl::beta() const
    {
      return d_beta;
    }

  } /* namespace digital */
} /* namespace gr */

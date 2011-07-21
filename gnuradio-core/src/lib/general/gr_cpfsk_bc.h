/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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
#ifndef INCLUDED_GR_CPFSK_BC_H
#define INCLUDED_GR_CPFSK_BC_H

#include <gr_core_api.h>
#include <gr_sync_interpolator.h>

class gr_cpfsk_bc;

typedef boost::shared_ptr<gr_cpfsk_bc> gr_cpfsk_bc_sptr;

GR_CORE_API gr_cpfsk_bc_sptr gr_make_cpfsk_bc(float k, float ampl, int samples_per_sym);

/*!
 * \brief Perform continuous phase 2-level frequency shift keying modulation 
 * on an input stream of unpacked bits.
 * \ingroup modulation_blk
 *
 * \param k                     modulation index
 * \param ampl		        output amplitude
 * \param samples_per_sym	number of output samples per input bit
 */

class GR_CORE_API gr_cpfsk_bc : public gr_sync_interpolator
{
private:
  friend GR_CORE_API gr_cpfsk_bc_sptr gr_make_cpfsk_bc(float k, float ampl, int samples_per_sym);

  gr_cpfsk_bc(float k, float ampl, int samples_per_sym);

  int   d_samples_per_sym;	// Samples per symbol, square pulse
  float d_freq;			// Modulation index*pi/samples_per_sym
  float d_ampl;			// Output amplitude
  float d_phase;		// Current phase

 public:
  ~gr_cpfsk_bc();

  void set_amplitude(float amplitude) { d_ampl = amplitude; }

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif /* INCLUDED_GR_CPFSK_BC_H */

/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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

// @WARNING@

#ifndef @GUARD_NAME@
#define @GUARD_NAME@

#include <gr_core_api.h>
#include <gr_sync_block.h>
#include <gr_sig_source_waveform.h>
#include <gr_fxpt_nco.h>

class @NAME@;
typedef boost::shared_ptr<@NAME@> @NAME@_sptr;

/*!
 * \brief signal generator with @TYPE@ output.
 * \ingroup source_blk
 */

class GR_CORE_API @NAME@ : public gr_sync_block {
  friend GR_CORE_API @NAME@_sptr 
  gr_make_@BASE_NAME@ (double sampling_freq, gr_waveform_t waveform,
		       double frequency, double ampl, @TYPE@ offset);

  double		d_sampling_freq;
  gr_waveform_t		d_waveform;
  double		d_frequency;
  double		d_ampl;
  @TYPE@		d_offset;
  gr_fxpt_nco		d_nco;


  @NAME@ (double sampling_freq, gr_waveform_t waveform,
	  double wave_freq, double ampl, @TYPE@ offset);

 public:
  virtual int work (int noutput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items);

  // ACCESSORS
  double sampling_freq () const { return d_sampling_freq; }
  gr_waveform_t waveform () const { return d_waveform; }
  double frequency () const { return d_frequency; }
  double amplitude () const { return d_ampl; }
  @TYPE@ offset () const { return d_offset; }

  // MANIPULATORS
  void set_sampling_freq (double sampling_freq);
  void set_waveform (gr_waveform_t waveform);
  void set_frequency (double frequency);
  void set_amplitude (double ampl);
  void set_offset (@TYPE@ offset);
};

GR_CORE_API @NAME@_sptr
gr_make_@BASE_NAME@ (double sampling_freq, gr_waveform_t waveform,
		     double wave_freq, double ampl, @TYPE@ offset = 0);


#endif

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

// WARNING: this file is machine generated.  Edits will be over written

#ifndef INCLUDED_GR_SIG_SOURCE_F_H
#define INCLUDED_GR_SIG_SOURCE_F_H

#include <gr_sync_block.h>
#include <gr_sig_source_waveform.h>
#include <gr_fxpt_nco.h>

class gr_sig_source_f;
typedef boost::shared_ptr<gr_sig_source_f> gr_sig_source_f_sptr;

/*!
 * \brief signal generator with float output.
 * \ingroup source_blk
 */

class gr_sig_source_f : public gr_sync_block {
  friend gr_sig_source_f_sptr 
  gr_make_sig_source_f (double sampling_freq, gr_waveform_t waveform,
		       double frequency, double ampl, float offset);

  double		d_sampling_freq;
  gr_waveform_t		d_waveform;
  double		d_frequency;
  double		d_ampl;
  float		d_offset;
  gr_fxpt_nco		d_nco;


  gr_sig_source_f (double sampling_freq, gr_waveform_t waveform,
	  double wave_freq, double ampl, float offset);

 public:
  virtual int work (int noutput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items);

  // ACCESSORS
  double sampling_freq () const { return d_sampling_freq; }
  gr_waveform_t waveform () const { return d_waveform; }
  double frequency () const { return d_frequency; }
  double amplitude () const { return d_ampl; }
  float offset () const { return d_offset; }

  // MANIPULATORS
  void set_sampling_freq (double sampling_freq);
  void set_waveform (gr_waveform_t waveform);
  void set_frequency (double frequency);
  void set_amplitude (double ampl);
  void set_offset (float offset);
};

gr_sig_source_f_sptr
gr_make_sig_source_f (double sampling_freq, gr_waveform_t waveform,
		     double wave_freq, double ampl, float offset = 0);


#endif

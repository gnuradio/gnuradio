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

GR_SWIG_BLOCK_MAGIC(gr,sig_source_s);

gr_sig_source_s_sptr
gr_make_sig_source_s (double sampling_freq, gr_waveform_t waveform,
		     double wave_freq, double ampl, short offset = 0);


class gr_sig_source_s : public gr_sync_block {
 private:
  gr_sig_source_s (double sampling_freq, gr_waveform_t waveform,
	  double wave_freq, double ampl, short offset);

 public:

  // ACCESSORS
  double sampling_freq () const { return d_sampling_freq; }
  gr_waveform_t waveform () const { return d_waveform; }
  double frequency () const { return d_frequency; }
  double amplitude () const { return d_ampl; }
  short offset () const { return d_offset; }

  // MANIPULATORS
  void set_sampling_freq (double sampling_freq);
  void set_waveform (gr_waveform_t waveform);
  void set_frequency (double frequency);
  void set_amplitude (double ampl);
  void set_offset (short offset);
};

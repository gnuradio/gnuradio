/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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


GR_SWIG_BLOCK_MAGIC(gr,wavfile_sink);

gr_wavfile_sink_sptr
gr_make_wavfile_sink (const char *filename,
		      int n_channels,
		      unsigned int sample_rate,
		      int bits_per_sample = 16) throw (std::runtime_error);

class gr_wavfile_sink : public gr_sync_block
{
protected:
  gr_wavfile_sink(const char *filename,
		  int n_channels,
		  unsigned int sample_rate,
		  int bits_per_sample) throw (std::runtime_error);
  
public:
  ~gr_wavfile_sink ();
  bool open(const char* filename);
  void close();
  void set_sample_rate(unsigned int sample_rate);
  void set_bits_per_sample(int bits_per_sample);
};


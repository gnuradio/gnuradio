/* -*- c++ -*- */
/*
 * Copyright 2004,2005 Free Software Foundation, Inc.
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

GR_SWIG_BLOCK_MAGIC(gr,fft_filter_fff)

gr_fft_filter_fff_sptr 
gr_make_fft_filter_fff (int decimation,
			const std::vector<float> &taps
			) throw (std::invalid_argument);

class gr_fft_filter_fff : public gr_sync_decimator
{
 private:
  gr_fft_filter_fff (int decimation, const std::vector<float> &taps);

 public:
  ~gr_fft_filter_fff ();

  void set_taps (const std::vector<float> &taps);
  std::vector<float> taps () const;
};

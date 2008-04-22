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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#if 1

// This version works.

GR_SWIG_BLOCK_MAGIC(gcell, fft_vcc)

gcell_fft_vcc_sptr 
gcell_make_fft_vcc (int fft_size, bool forward, const std::vector<float> window, bool shift=false);

class gcell_fft_vcc : public gr_sync_block
{
 protected:
  gcell_fft_vcc (int fft_size, bool forward, const std::vector<float> &window, bool shift);

 public:
  bool set_window(const std::vector<float> &window);
};

#else

// This version gives swig heartburn.  We end up with an object that's
// not quite usable.

GR_SWIG_BLOCK_MAGIC(gcell, fft_vcc);

gcell_fft_vcc_sptr 
gcell_make_fft_vcc (int fft_size, bool forward, const std::vector<float> window, bool shift=false);

class gcell_fft_vcc : public gr_fft_vcc
{
 protected:
  gr_fft_vcc(int fft_size, bool forward, const std::vector<float> &window, bool shift);
};

#endif

/* -*- c++ -*- */
/*
 * Copyright 2004,2007,2008,2012 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_fft_vcc.h>		// abstract class
#include <gr_fft_vcc_fftw.h>	// concrete class
#include <gr_io_signature.h>
#include <gri_fft.h>
#include <math.h>
#include <string.h>

gr_fft_vcc_sptr
gr_make_fft_vcc (int fft_size, bool forward,
		 const std::vector<float> &window,
		 bool shift, int nthreads)
{
  return gr_make_fft_vcc_fftw(fft_size, forward,
			      window, shift, nthreads);
}

gr_fft_vcc::gr_fft_vcc (const std::string &name,
			int fft_size, bool forward, const std::vector<float> &window,
			bool shift)
  : gr_sync_block (name,
		   gr_make_io_signature (1, 1, fft_size * sizeof (gr_complex)),
		   gr_make_io_signature (1, 1, fft_size * sizeof (gr_complex))),
    d_fft_size(fft_size), d_forward(forward), d_shift(shift)
{
  set_window(window);
}

gr_fft_vcc::~gr_fft_vcc ()
{
}

bool 
gr_fft_vcc::set_window(const std::vector<float> &window)
{
  if(window.size()==0 || window.size()==d_fft_size) {
    d_window=window;
    return true;
  }
  else 
    return false;
}

void
gr_fft_vcc::set_nthreads(int n)
{
  throw std::runtime_error("gr_fft_vcc::set_nthreads not implemented.");
}

int
gr_fft_vcc::nthreads() const
{
  throw std::runtime_error("gr_fft_vcc::nthreads not implemented.");
  return 0;
}

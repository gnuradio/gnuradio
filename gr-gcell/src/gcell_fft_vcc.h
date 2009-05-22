/* -*- c++ -*- */
/*
 * Copyright 2004,2007,2008 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GCELL_FFT_VCC_H
#define INCLUDED_GCELL_FFT_VCC_H

#include <gr_fft_vcc.h>

class gc_job_manager;

class gcell_fft_vcc;
typedef boost::shared_ptr<gcell_fft_vcc> gcell_fft_vcc_sptr;

gcell_fft_vcc_sptr
gcell_make_fft_vcc(int fft_size, bool forward, const std::vector<float> &window, bool shift=false);

/*!
 * \brief Compute forward or reverse FFT.  complex vector in / complex vector out.
 * \ingroup dft_blk
 *
 * Concrete class that uses gcell to offload FFT to SPEs.
 */
class gcell_fft_vcc : public gr_fft_vcc
{
  int				     d_log2_fft_size;
  boost::shared_ptr<gc_job_manager>  d_mgr;
  std::complex<float>		    *d_twiddle;		// twiddle values (16-byte aligned)
  boost::shared_ptr<void>	     d_twiddle_boost;	// automatic storage mgmt
  
  friend gcell_fft_vcc_sptr
  gcell_make_fft_vcc(int fft_size, bool forward, const std::vector<float> &window, bool shift);

  gcell_fft_vcc(int fft_size, bool forward, const std::vector<float> &window, bool shift);

 public:
  ~gcell_fft_vcc();

  int work(int noutput_items,
	   gr_vector_const_void_star &input_items,
	   gr_vector_void_star &output_items);
};


#endif /* INCLUDED_GCELL_FFT_VCC_H */

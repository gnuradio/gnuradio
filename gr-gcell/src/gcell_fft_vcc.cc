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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gcell_fft_vcc.h>
#include <gr_io_signature.h>
#include <gc_job_manager.h>
#include <gc_aligned_alloc.h>
#include <gcp_fft_1d_r2.h>
#include <math.h>
#include <assert.h>
#include <stdexcept>
#include <string.h>


#define MIN_FFT_SIZE	  32
#define	MAX_FFT_SIZE	4096

inline static bool
is_power_of_2(int x)
{
  return x != 0 && (x & (x-1)) == 0;
}

static int 
log2(int x)	// x is an exact power of 2
{
  for (int i = 0; i < 32; i++)
    if (x == (1 << i))
      return i;

  assert(0);
}

#if 0
gr_fft_vcc_sptr
gcell_make_fft_vcc(int fft_size, bool forward, const std::vector<float> &window, bool shift)
{
  // If it doesn't meet our constraints, use standard implemenation
  if (fft_size < MIN_FFT_SIZE || fft_size > MAX_FFT_SIZE
      || !is_power_of_2(fft_size)
      || (window.size() != 0 && fft_size > MAX_FFT_SIZE/2))
    return gr_make_fft_vcc(fft_size, forward, window, shift);
  else
    return gr_fft_vcc_sptr (new gcell_fft_vcc(fft_size, forward, window, shift));
}
#else

gcell_fft_vcc_sptr
gcell_make_fft_vcc(int fft_size, bool forward, const std::vector<float> &window, bool shift)
{
  return gcell_fft_vcc_sptr (new gcell_fft_vcc(fft_size, forward, window, shift));
}

#endif

gcell_fft_vcc::gcell_fft_vcc (int fft_size, bool forward,
			      const std::vector<float> &window, bool shift)
  : gr_fft_vcc("gcell_fft_vcc", fft_size, forward, window, shift)
{
  if (fft_size < MIN_FFT_SIZE || fft_size > MAX_FFT_SIZE || !is_power_of_2(fft_size)){
    throw std::invalid_argument("fft_size");
  }

  if (window.size() != 0 && fft_size > MAX_FFT_SIZE/2){
    throw std::invalid_argument("fft_size too big to use window");
  }

  d_log2_fft_size = log2(fft_size);
  d_mgr = gc_job_manager::singleton();		// grab the singleton job manager
  d_twiddle_boost = gc_aligned_alloc_sptr(sizeof(std::complex<float>) * fft_size/4, 128);
  d_twiddle = (std::complex<float>*) d_twiddle_boost.get();
  gcp_fft_1d_r2_twiddle(d_log2_fft_size, d_twiddle);
}

gcell_fft_vcc::~gcell_fft_vcc ()
{
}

int
gcell_fft_vcc::work(int noutput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items)
{
  const gr_complex *in = (const gr_complex *) input_items[0];
  gr_complex *out = (gr_complex *) output_items[0];

  // unsigned int input_data_size = input_signature()->sizeof_stream_item(0);
  // unsigned int output_data_size = output_signature()->sizeof_stream_item(0);

  float window_buf[MAX_FFT_SIZE/2] __attribute__((aligned (16)));
  float *window = 0;
  
  // If we've got a window, ensure it's 16-byte aligned
  // FIXME move this to set_window
  if (d_window.size()){
    if ((((intptr_t)&d_window[0]) & 0xf) == 0)
      window = &d_window[0];		// OK as is
    else {
      window = window_buf;		// copy to aligned buffer
      memcpy(window, &d_window[0], sizeof(float) * d_window.size());
    }
  }

  std::vector<gc_job_desc_sptr>	jd_sptr(noutput_items);
  gc_job_desc *jd[noutput_items];
  bool done[noutput_items];
  
  // submit noutput_items jobs in parallel

  for (int i = 0; i < noutput_items; i++){
    jd_sptr[i] = gcp_fft_1d_r2_submit(d_mgr, d_log2_fft_size,
				      d_forward, d_shift,
				      &out[i * d_fft_size],
				      &in[i * d_fft_size],
				      d_twiddle,
				      window);
    jd[i] = jd_sptr[i].get();
  }

  int n = d_mgr->wait_jobs(noutput_items, jd, done, GC_WAIT_ALL);
  if (n != noutput_items){
    fprintf(stderr, "gcell_fft_vcc: wait_jobs returned %d, expected %d\n",
	    n, noutput_items);
    return -1;
  }

  for (int i = 0; i < noutput_items; i++){
    if (jd[i]->status != JS_OK){
      fprintf(stderr, "gcell_fft_vcc jd[%d]->status = %s\n",
	      i, gc_job_status_string(jd[i]->status).c_str());
      return -1;
    }
  }

  return noutput_items;
}


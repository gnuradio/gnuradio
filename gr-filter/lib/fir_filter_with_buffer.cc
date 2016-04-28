/* -*- c++ -*- */
/*
 * Copyright 2010,2012 Free Software Foundation, Inc.
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
#include <config.h>
#endif

#include <gnuradio/filter/fir_filter_with_buffer.h>
#include <gnuradio/fft/fft.h>
#include <volk/volk.h>
#include <algorithm>
#include <cstdio>
#include <cstring>

namespace gr {
  namespace filter {
    namespace kernel {

      fir_filter_with_buffer_fff::fir_filter_with_buffer_fff(const std::vector<float> &taps)
      {
	d_align = volk_get_alignment();
	d_naligned = std::max((size_t)1, d_align / sizeof(float));

	d_buffer_ptr = NULL;
	d_aligned_taps = NULL;
	set_taps(taps);

	// Make sure the output sample is always aligned, too.
	d_output = (float*)volk_malloc(1*sizeof(float), d_align);
      }

      fir_filter_with_buffer_fff::~fir_filter_with_buffer_fff()
      {
	if(d_buffer_ptr != NULL) {
	  volk_free(d_buffer_ptr);
	  d_buffer_ptr = NULL;
	}

	// Free aligned taps
	if(d_aligned_taps != NULL) {
	  for(int i = 0; i < d_naligned; i++) {
	    volk_free(d_aligned_taps[i]);
	  }
	  ::free(d_aligned_taps);
	  d_aligned_taps = NULL;
	}

	// Free output sample
	volk_free(d_output);
      }

      void
      fir_filter_with_buffer_fff::set_taps(const std::vector<float> &taps)
      {
	if(d_buffer_ptr != NULL) {
	  volk_free(d_buffer_ptr);
	  d_buffer_ptr = NULL;
	}

	// Free the taps if already allocated
	if(d_aligned_taps != NULL) {
	  for(int i = 0; i < d_naligned; i++) {
	    volk_free(d_aligned_taps[i]);
	  }
	  ::free(d_aligned_taps);
	  d_aligned_taps = NULL;
	}

	d_ntaps = (int)taps.size();
	d_taps = taps;
	std::reverse(d_taps.begin(), d_taps.end());

	// We allocate enough to be able to look back and forth
	// d_naligned beyond the buffer boundaries and make sure these
	// are zeroed out (or they may be nan, which will cause
	// problems). We then set d_buffer to the position in the
	// d_buffer_ptr such that we only touch the internally
	// allocated space.
        d_buffer_ptr = (float*)volk_malloc((2*(d_ntaps + d_naligned))*sizeof(float), d_align);

	memset(d_buffer_ptr, 0, 2*(d_ntaps + d_naligned)*sizeof(float));
	d_buffer = d_buffer_ptr + d_naligned;

	// Allocate aligned taps
	d_aligned_taps = (float**)malloc(d_naligned*sizeof(float*));
	for(int i = 0; i < d_naligned; i++) {
	  d_aligned_taps[i] = (float*)volk_malloc((d_ntaps+d_naligned-1)*sizeof(float), d_align);
	  memset(d_aligned_taps[i], 0, sizeof(float)*(d_ntaps+d_naligned-1));
	  for(unsigned int j = 0; j < d_ntaps; j++)
	    d_aligned_taps[i][i+j] = d_taps[j];
	}

	d_idx = 0;
      }

      std::vector<float>
      fir_filter_with_buffer_fff::taps() const
      {
	std::vector<float> t = d_taps;
	std::reverse(t.begin(), t.end());
	return t;
      }

      float
      fir_filter_with_buffer_fff::filter(float input)
      {
	d_buffer[d_idx] = input;
	d_buffer[d_idx+ntaps()] = input;

	d_idx++;
	if(d_idx >= ntaps())
	  d_idx = 0;

	const float *ar = (float*)((size_t)(&d_buffer[d_idx]) & ~(d_align-1));
	unsigned al = (&d_buffer[d_idx]) - ar;

	volk_32f_x2_dot_prod_32f_a(d_output, ar,
				   d_aligned_taps[al],
				   ntaps()+al);
	return *d_output;
      }

      float
      fir_filter_with_buffer_fff::filter(const float input[],
					 unsigned long dec)
      {
	unsigned int i;

	for(i = 0; i < dec; i++) {
	  d_buffer[d_idx] = input[i];
	  d_buffer[d_idx+ntaps()] = input[i];
	  d_idx++;
	  if(d_idx >= ntaps())
	    d_idx = 0;
	}

	const float *ar = (float*)((size_t)(&d_buffer[d_idx]) & ~(d_align-1));
	unsigned al = (&d_buffer[d_idx]) - ar;

	volk_32f_x2_dot_prod_32f_a(d_output, ar,
				   d_aligned_taps[al],
				   ntaps()+al);
	return *d_output;
      }

      void
      fir_filter_with_buffer_fff::filterN(float output[],
					  const float input[],
					  unsigned long n)
      {
	for(unsigned long i = 0; i < n; i++) {
	  output[i] = filter(input[i]);
	}
      }

      void
      fir_filter_with_buffer_fff::filterNdec(float output[],
					     const float input[],
					     unsigned long n,
					     unsigned long decimate)
      {
	unsigned long j = 0;
	for(unsigned long i = 0; i < n; i++) {
	  output[i] = filter(&input[j], decimate);
	  j += decimate;
	}
      }


      /**************************************************************/


      fir_filter_with_buffer_ccc::fir_filter_with_buffer_ccc(const std::vector<gr_complex> &taps)
      {
	d_align = volk_get_alignment();
	d_naligned = std::max((size_t)1, d_align / sizeof(gr_complex));

	d_buffer_ptr = NULL;
	d_aligned_taps = NULL;
	set_taps(taps);

	// Make sure the output sample is always aligned, too.
	d_output = (gr_complex*)volk_malloc(1*sizeof(gr_complex), d_align);
      }

      fir_filter_with_buffer_ccc::~fir_filter_with_buffer_ccc()
      {
	if(d_buffer_ptr != NULL) {
	  volk_free(d_buffer_ptr);
	  d_buffer_ptr = NULL;
	}

	// Free aligned taps
	if(d_aligned_taps != NULL) {
	  for(int i = 0; i < d_naligned; i++) {
	    volk_free(d_aligned_taps[i]);
	  }
	  ::free(d_aligned_taps);
	  d_aligned_taps = NULL;
	}

	// Free output sample
	volk_free(d_output);
      }

      void
      fir_filter_with_buffer_ccc::set_taps(const std::vector<gr_complex> &taps)
      {
	if(d_buffer_ptr != NULL) {
	  volk_free(d_buffer_ptr);
	  d_buffer_ptr = NULL;
	}

	// Free the taps if already allocated
	if(d_aligned_taps != NULL) {
	  for(int i = 0; i < d_naligned; i++) {
	    volk_free(d_aligned_taps[i]);
	  }
	  ::free(d_aligned_taps);
	  d_aligned_taps = NULL;
	}

	d_ntaps = (int)taps.size();
	d_taps = taps;
	std::reverse(d_taps.begin(), d_taps.end());

	// We allocate enough to be able to look back and forth
	// d_naligned beyond the buffer boundaries and make sure these
	// are zeroed out (or they may be nan, which will cause
	// problems). We then set d_buffer to the position in the
	// d_buffer_ptr such that we only touch the internally
	// allocated space.
        d_buffer_ptr = (gr_complex*)volk_malloc((2*(d_ntaps + d_naligned))*sizeof(gr_complex), d_align);
	memset(d_buffer_ptr, 0, 2*(d_ntaps + d_naligned)*sizeof(gr_complex));
	d_buffer = d_buffer_ptr + d_naligned;

	// Allocate aligned taps
	d_aligned_taps = (gr_complex**)malloc(d_naligned*sizeof(gr_complex*));
	for(int i = 0; i < d_naligned; i++) {
	  d_aligned_taps[i] = (gr_complex*)volk_malloc((d_ntaps+d_naligned-1)*sizeof(gr_complex), d_align);
	  memset(d_aligned_taps[i], 0, sizeof(gr_complex)*(d_ntaps+d_naligned-1));
	  for(unsigned int j = 0; j < d_ntaps; j++)
	    d_aligned_taps[i][i+j] = d_taps[j];
	}

	d_idx = 0;
      }

      std::vector<gr_complex>
      fir_filter_with_buffer_ccc::taps() const
      {
	std::vector<gr_complex> t = d_taps;
	std::reverse(t.begin(), t.end());
	return t;
      }

      gr_complex
      fir_filter_with_buffer_ccc::filter(gr_complex input)
      {
	d_buffer[d_idx] = input;
	d_buffer[d_idx+ntaps()] = input;

	d_idx++;
	if(d_idx >= ntaps())
	  d_idx = 0;

	const gr_complex *ar = (gr_complex *)((size_t)(&d_buffer[d_idx]) & ~(d_align-1));
	unsigned al = (&d_buffer[d_idx]) - ar;

	volk_32fc_x2_dot_prod_32fc_a(d_output, ar,
				     d_aligned_taps[al],
				     (ntaps()+al));
	return *d_output;
      }

      gr_complex
      fir_filter_with_buffer_ccc::filter(const gr_complex input[],
					 unsigned long dec)
      {
	unsigned int i;

	for(i = 0; i < dec; i++) {
	  d_buffer[d_idx] = input[i];
	  d_buffer[d_idx+ntaps()] = input[i];
	  d_idx++;
	  if(d_idx >= ntaps())
	    d_idx = 0;
	}

	const gr_complex *ar = (gr_complex *)((size_t)(&d_buffer[d_idx]) & ~(d_align-1));
	unsigned al = (&d_buffer[d_idx]) - ar;

	volk_32fc_x2_dot_prod_32fc_a(d_output, ar,
				     d_aligned_taps[al],
				     (ntaps()+al));
	return *d_output;
      }

      void
      fir_filter_with_buffer_ccc::filterN(gr_complex output[],
					  const gr_complex input[],
					  unsigned long n)
      {
	for(unsigned long i = 0; i < n; i++) {
	  output[i] = filter(input[i]);
	}
      }

      void
      fir_filter_with_buffer_ccc::filterNdec(gr_complex output[],
					     const gr_complex input[],
					     unsigned long n,
					     unsigned long decimate)
      {
	unsigned long j = 0;
	for(unsigned long i = 0; i < n; i++) {
	  output[i] = filter(&input[j], decimate);
	  j += decimate;
	}
      }


      /**************************************************************/


      fir_filter_with_buffer_ccf::fir_filter_with_buffer_ccf(const std::vector<float> &taps)
      {
	d_align = volk_get_alignment();
	d_naligned = std::max((size_t)1, d_align / sizeof(gr_complex));

	d_buffer_ptr = NULL;
	d_aligned_taps = NULL;
	set_taps(taps);

	// Make sure the output sample is always aligned, too.
	d_output = (gr_complex*)volk_malloc(1*sizeof(gr_complex), d_align);
      }

      fir_filter_with_buffer_ccf::~fir_filter_with_buffer_ccf()
      {
	if(d_buffer_ptr != NULL) {
	  volk_free(d_buffer_ptr);
	  d_buffer_ptr = NULL;
	}

	// Free aligned taps
	if(d_aligned_taps != NULL) {
	  for(int i = 0; i < d_naligned; i++) {
	    volk_free(d_aligned_taps[i]);
	  }
	  ::free(d_aligned_taps);
	  d_aligned_taps = NULL;
	}

	// Free output sample
	volk_free(d_output);
      }

      void
      fir_filter_with_buffer_ccf::set_taps(const std::vector<float> &taps)
      {
	if(d_buffer_ptr != NULL) {
	  volk_free(d_buffer_ptr);
	  d_buffer_ptr = NULL;
	}

	// Free the taps if already allocated
	if(d_aligned_taps != NULL) {
	  for(int i = 0; i < d_naligned; i++) {
	    volk_free(d_aligned_taps[i]);
	  }
	  ::free(d_aligned_taps);
	  d_aligned_taps = NULL;
	}

	d_ntaps = (int)taps.size();
	d_taps = taps;
	std::reverse(d_taps.begin(), d_taps.end());

	// We allocate enough to be able to look back and forth
	// d_naligned beyond the buffer boundaries and make sure these
	// are zeroed out (or they may be nan, which will cause
	// problems). We then set d_buffer to the position in the
	// d_buffer_ptr such that we only touch the internally
	// allocated space.
        d_buffer_ptr = (gr_complex*)volk_malloc((2*(d_ntaps + d_naligned))*sizeof(gr_complex), d_align);
	memset(d_buffer_ptr, 0, 2*(d_ntaps + d_naligned)*sizeof(gr_complex));
	d_buffer = d_buffer_ptr + d_naligned;

	// Allocate aligned taps
	d_aligned_taps = (float**)malloc(d_naligned*sizeof(float*));
	for(int i = 0; i < d_naligned; i++) {
	  d_aligned_taps[i] = (float*)volk_malloc((d_ntaps+d_naligned-1)*sizeof(float), d_align);
	  memset(d_aligned_taps[i], 0, sizeof(float)*(d_ntaps+d_naligned-1));
	  for(unsigned int j = 0; j < d_ntaps; j++)
	    d_aligned_taps[i][i+j] = d_taps[j];
	}

	d_idx = 0;
      }

      std::vector<float>
      fir_filter_with_buffer_ccf::taps() const
      {
	std::vector<float> t = d_taps;
	std::reverse(t.begin(), t.end());
	return t;
      }

      gr_complex
      fir_filter_with_buffer_ccf::filter(gr_complex input)
      {
	d_buffer[d_idx] = input;
	d_buffer[d_idx+ntaps()] = input;

	d_idx++;
	if(d_idx >= ntaps())
	  d_idx = 0;

	const gr_complex *ar = (gr_complex *)((size_t)(&d_buffer[d_idx]) & ~(d_align-1));
	unsigned al = (&d_buffer[d_idx]) - ar;

	volk_32fc_32f_dot_prod_32fc_a(d_output, ar,
				      d_aligned_taps[al],
				      ntaps()+al);
	return *d_output;
      }

      gr_complex
      fir_filter_with_buffer_ccf::filter(const gr_complex input[],
					 unsigned long dec)
      {
	unsigned int i;

	for(i = 0; i < dec; i++) {
	  d_buffer[d_idx] = input[i];
	  d_buffer[d_idx+ntaps()] = input[i];
	  d_idx++;
	  if(d_idx >= ntaps())
	    d_idx = 0;
	}

	const gr_complex *ar = (gr_complex *)((size_t)(&d_buffer[d_idx]) & ~(d_align-1));
	unsigned al = (&d_buffer[d_idx]) - ar;

	volk_32fc_32f_dot_prod_32fc_a(d_output, ar,
				      d_aligned_taps[al],
				      ntaps()+al);
	return *d_output;
      }

      void
      fir_filter_with_buffer_ccf::filterN(gr_complex output[],
					  const gr_complex input[],
					  unsigned long n)
      {
	for(unsigned long i = 0; i < n; i++) {
	  output[i] = filter(input[i]);
	}
      }

      void
      fir_filter_with_buffer_ccf::filterNdec(gr_complex output[],
					     const gr_complex input[],
					     unsigned long n,
					     unsigned long decimate)
      {
	unsigned long j = 0;
	for(unsigned long i = 0; i < n; i++) {
	  output[i] = filter(&input[j], decimate);
	  j += decimate;
	}
      }


    } /* namespace kernel */
  } /* namespace filter */
} /* namespace gr */

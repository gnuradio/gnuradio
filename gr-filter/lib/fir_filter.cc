/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2012 Free Software Foundation, Inc.
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

#include <filter/fir_filter.h>
#include <fft/fft.h>
#include <volk/volk.h>
#include <cstdio>
#include <float_dotprod_x86.h>

namespace gr {
  namespace filter {
    namespace kernel {
    
      fir_filter_fff::fir_filter_fff(int decimation,
				     const std::vector<float> &taps)
      {
	d_align = volk_get_alignment();
	d_naligned = d_align / sizeof(float);

	d_taps = NULL;
	set_taps(taps);

	// Make sure the output sample is always aligned, too.
	d_output = fft::malloc_float(1);
      }
      
      fir_filter_fff::~fir_filter_fff()
      {
	// Free taps
	if(d_taps != NULL) {
	  fft::free(d_taps);
	  d_taps = NULL;
	}

	// Free all aligned taps
	for(int i = 0; i < d_naligned; i++) {
	  fft::free(d_aligned_taps[i]);
	}
	fft::free(d_aligned_taps);

	// Free output sample
	fft::free(d_output);
      }
      
      void
      fir_filter_fff::set_taps(const std::vector<float> &taps)
      {
	// Free the taps if already allocated
	if(d_taps != NULL) {
	  fft::free(d_taps);
	  d_taps = NULL;

	  for(int i = 0; i < d_naligned; i++) {
	    fft::free(d_aligned_taps[i]);
	  }
	  fft::free(d_aligned_taps);
	}
	
	d_ntaps = (int)taps.size();
	d_taps = fft::malloc_float(d_ntaps);
	for(unsigned int i = 0; i < d_ntaps; i++) {
	  d_taps[d_ntaps-i-1] = taps[i];
	}

	// Make a set of taps at all possible arch alignments
	d_aligned_taps = (float**)malloc(d_naligned*sizeof(float**));
	for(int i = 0; i < d_naligned; i++) {
	  d_aligned_taps[i] = fft::malloc_float(d_ntaps+d_naligned-1);
	  memset(d_aligned_taps[i], 0, sizeof(float)*(d_ntaps+d_naligned-1));
	  memcpy(&d_aligned_taps[i][i], d_taps, sizeof(float)*(d_ntaps));
	}
      }
      
      std::vector<float>
      fir_filter_fff::taps() const
      {
	std::vector<float> t;
	for(unsigned int i = 0; i < d_ntaps; i++)
	  t.push_back(d_taps[d_ntaps-i-1]);
	return t;
      }

      unsigned int
      fir_filter_fff::ntaps() const
      {
	return d_ntaps;
      }

      float
      fir_filter_fff::filter(const float input[])
      {
	const float *ar = (float *)((unsigned long) input & ~(d_align-1));
	unsigned al = input - ar;

	volk_32f_x2_dot_prod_32f_a(d_output, ar,
				   d_aligned_taps[al],
				   d_ntaps+al);
	return *d_output;
      }
      
      void
      fir_filter_fff::filterN(float output[],
			      const float input[],
			      unsigned long n)
      {
	for(unsigned long i = 0; i < n; i++) {
	  output[i] = filter(&input[i]);
	}
      }
      
      void
      fir_filter_fff::filterNdec(float output[],
				 const float input[],
				 unsigned long n,
				 unsigned int decimate)
      {
	unsigned long j = 0;
	for(unsigned long i = 0; i < n; i++) {
	  output[i] = filter(&input[j]);
	  j += decimate;
	}
      }
      
      /**************************************************************/

      fir_filter_ccf::fir_filter_ccf(int decimation,
				     const std::vector<float> &taps)
      {
	d_taps = NULL;
	set_taps(taps);
      }
      
      fir_filter_ccf::~fir_filter_ccf()
      {
	if(d_taps != NULL) {
	  fft::free(d_taps);
	  d_taps = NULL;
	}
    }
      
      void
      fir_filter_ccf::set_taps(const std::vector<float> &taps)
      {
	// Free the taps if already allocated
	if(d_taps != NULL) {
	  fft::free(d_taps);
	  d_taps = NULL;
	}
	
	d_ntaps = (int)taps.size();
	d_taps = fft::malloc_complex(d_ntaps);
	for(unsigned int i = 0; i < d_ntaps; i++) {
	  d_taps[d_ntaps-i-1] = gr_complex(taps[i],0);
	}
      }
      
      std::vector<float>
      fir_filter_ccf::taps() const
      {
	std::vector<float> t;
	for(unsigned int i = 0; i < d_ntaps; i++)
	  t.push_back(d_taps[d_ntaps-i-1].real());
	return t;
      }

      unsigned int
      fir_filter_ccf::ntaps() const
      {
	return d_ntaps;
      }
      
      gr_complex
      fir_filter_ccf::filter(const gr_complex input[])
      {
	gr_complex output;
	volk_32fc_x2_dot_prod_32fc_u(&output, input, d_taps, d_ntaps);
	return output;
      }
      
      void
      fir_filter_ccf::filterN(gr_complex output[],
			      const gr_complex input[],
			      unsigned long n)
      {
	for(unsigned long i = 0; i < n; i++)
	  output[i] = filter(&input[i]);
      }
      
      
      void
      fir_filter_ccf::filterNdec(gr_complex output[],
				 const gr_complex input[],
				 unsigned long n,
				 unsigned int decimate)
      {
	unsigned long j = 0;
	for(unsigned long i = 0; i < n; i++){
	  output[i] = filter(&input[j]);
	  j += decimate;
	}
      }
      
      /**************************************************************/

      fir_filter_ccc::fir_filter_ccc(int decimation,
				     const std::vector<gr_complex> &taps)
      {
	d_align = volk_get_alignment();
	d_naligned = d_align / sizeof(gr_complex);

	d_taps = NULL;
	set_taps(taps);

	// Make sure the output sample is always aligned, too.
	d_output = fft::malloc_complex(1);
      }
      
      fir_filter_ccc::~fir_filter_ccc()
      {
	// Free taps
	if(d_taps != NULL) {
	  fft::free(d_taps);
	  d_taps = NULL;
	}

	// Free all aligned taps
	for(int i = 0; i < d_naligned; i++) {
	  fft::free(d_aligned_taps[i]);
	}
	fft::free(d_aligned_taps);

	// Free output sample
	fft::free(d_output);
    }
      
      void
      fir_filter_ccc::set_taps(const std::vector<gr_complex> &taps)
      {
	// Free the taps if already allocated
	if(d_taps != NULL) {
	  fft::free(d_taps);
	  d_taps = NULL;

	  for(int i = 0; i < d_naligned; i++) {
	    fft::free(d_aligned_taps[i]);
	  }
	  fft::free(d_aligned_taps);
	}
	
	d_ntaps = (int)taps.size();
	d_taps = fft::malloc_complex(d_ntaps);
	for(unsigned int i = 0; i < d_ntaps; i++) {
	  d_taps[d_ntaps-i-1] = taps[i];
	}

	// Make a set of taps at all possible arch alignments
	d_aligned_taps = (gr_complex**)malloc(d_naligned*sizeof(gr_complex**));
	for(int i = 0; i < d_naligned; i++) {
	  d_aligned_taps[i] = fft::malloc_complex(d_ntaps+d_naligned-1);
	  memset(d_aligned_taps[i], 0, sizeof(gr_complex)*(d_ntaps+d_naligned-1));
	  memcpy(&d_aligned_taps[i][i], d_taps, sizeof(gr_complex)*(d_ntaps));
	}
      }
      
      std::vector<gr_complex>
      fir_filter_ccc::taps() const
      {
	std::vector<gr_complex> t;
	for(unsigned int i = 0; i < d_ntaps; i++)
	  t.push_back(d_taps[d_ntaps-i-1]);
	return t;
      }

      unsigned int
      fir_filter_ccc::ntaps() const
      {
	return d_ntaps;
      }
      
      gr_complex
      fir_filter_ccc::filter(const gr_complex input[])
      {
	const gr_complex *ar = (gr_complex *)((unsigned long) input & ~(d_align-1));
	unsigned al = input - ar;

	volk_32fc_x2_dot_prod_32fc_a(d_output, ar,
				     d_aligned_taps[al],
				     (d_ntaps+al)*sizeof(gr_complex));
	return *d_output;
      }
      
      void
      fir_filter_ccc::filterN(gr_complex output[],
			      const gr_complex input[],
			      unsigned long n)
      {
	for(unsigned long i = 0; i < n; i++)
	  output[i] = filter(&input[i]);
      }
      
      
      void
      fir_filter_ccc::filterNdec(gr_complex output[],
				 const gr_complex input[],
				 unsigned long n,
				 unsigned int decimate)
      {
	unsigned long j = 0;
	for(unsigned long i = 0; i < n; i++){
	  output[i] = filter(&input[j]);
	  j += decimate;
	}
      }
      
    } /* namespace kernel */
  } /* namespace filter */
} /* namespace gr */

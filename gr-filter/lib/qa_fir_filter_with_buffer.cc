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

#include <gr_types.h>
#include <qa_fir_filter_with_buffer.h>
#include <filter/fir_filter_with_buffer.h>
#include <fft/fft.h>
#include <cppunit/TestAssert.h>
#include <cmath>
#include <random.h>
#include <cstring>

namespace gr {
  namespace filter {

#define MAX_DATA        (16383)
#define	ERR_DELTA	(1e-5)

    static float
    uniform() 
    {
      return 2.0 * ((float) random() / RANDOM_MAX - 0.5); // uniformly (-1, 1)
    }

    static void
    random_floats(float *buf, unsigned n)
    {
      for(unsigned i = 0; i < n; i++)
	buf[i] = (float)rint(uniform() * 32767);
    }

    static void
    random_complex(gr_complex *buf, unsigned n)
    {
      for(unsigned i = 0; i < n; i++) {
	float re = rint(uniform() * MAX_DATA);
	float im = rint(uniform() * MAX_DATA);
	buf[i] = gr_complex(re, im);
      }
    }

    namespace fff {

      typedef float i_type;
      typedef float o_type;
      typedef float tap_type;
      typedef float acc_type;

      using std::vector;

      static o_type
      ref_dotprod(const i_type input[], const tap_type taps[], int ntaps)
      {
	acc_type sum = 0;
	for (int i = 0; i < ntaps; i++) {
	  sum += input[i] * taps[i];
	}
	return sum;
      }

      void
      qa_fir_filter_with_buffer_fff::t1()
      {
	test_decimate(1);
      }

      void
      qa_fir_filter_with_buffer_fff::t2()
      {
	test_decimate(2);
      }

      void
      qa_fir_filter_with_buffer_fff::t3()
      {
	test_decimate(5);
      }

      //
      // Test for ntaps in [0,9], and input lengths in [0,17].
      // This ensures that we are building the shifted taps correctly,
      // and exercises all corner cases on input alignment and length.
      //
      void
      qa_fir_filter_with_buffer_fff::test_decimate(unsigned int decimate)
      {
	const int MAX_TAPS   = 29;
	const int OUTPUT_LEN = 37;
	const int INPUT_LEN  = MAX_TAPS + OUTPUT_LEN;

	// Mem aligned buffer not really necessary, but why not?
	i_type   *input = fft::malloc_float(INPUT_LEN);
	i_type   *dline = fft::malloc_float(INPUT_LEN);
	o_type   *expected_output = fft::malloc_float(OUTPUT_LEN);
	o_type   *actual_output = fft::malloc_float(OUTPUT_LEN);
	tap_type *taps = fft::malloc_float(MAX_TAPS);

	srandom(0);	// we want reproducibility
	memset(dline, 0, INPUT_LEN*sizeof(i_type));

	for(int n = 0; n <= MAX_TAPS; n++) {
	  for(int ol = 0; ol <= OUTPUT_LEN; ol++) {

	    // build random test case
	    random_floats(input, INPUT_LEN);
	    random_floats(taps, MAX_TAPS);

	    // compute expected output values
	    memset(dline, 0, INPUT_LEN*sizeof(i_type));
	    for(int o = 0; o < (int)(ol/decimate); o++) {
	      // use an actual delay line for this test
	      for(int dd = 0; dd < (int)decimate; dd++) {
		for(int oo = INPUT_LEN-1; oo > 0; oo--)
		  dline[oo] = dline[oo-1];
		dline[0] = input[decimate*o+dd];
	      }
	      expected_output[o] = ref_dotprod(dline, taps, n);
	    }

	    // build filter
	    vector<tap_type> f1_taps(&taps[0], &taps[n]);
	    kernel::fir_filter_with_buffer_fff *f1 = \
	      new kernel::fir_filter_with_buffer_fff(f1_taps);

	    // zero the output, then do the filtering
	    memset(actual_output, 0, OUTPUT_LEN*sizeof(o_type));
	    f1->filterNdec(actual_output, input, ol/decimate, decimate);

	    // check results
	    //
	    // we use a sloppy error margin because on the x86 architecture,
	    // our reference implementation is using 80 bit floating point
	    // arithmetic, while the SSE version is using 32 bit float point
	    // arithmetic.

	    for(int o = 0; o < (int)(ol/decimate); o++) {
	      CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_output[o], actual_output[o],
			     sqrt((float)n)*0.25*MAX_DATA*MAX_DATA * ERR_DELTA);
	    }
	    delete f1;
	  }
	}
	fft::free(input);
	fft::free(dline);
	fft::free(expected_output);
	fft::free(actual_output);
	fft::free(taps);
      }

    } /* namespace fff */


    /**************************************************************/


    namespace ccc {

      typedef gr_complex i_type;
      typedef gr_complex o_type;
      typedef gr_complex tap_type;
      typedef gr_complex acc_type;

      using std::vector;

      static o_type
      ref_dotprod(const i_type input[], const tap_type taps[], int ntaps)
      {
	acc_type sum = 0;
	for(int i = 0; i < ntaps; i++) {
	  sum += input[i] * taps[i];
	}

	return sum;
      }

      void
      qa_fir_filter_with_buffer_ccc::t1()
      {
	test_decimate(1);
      }

      void
      qa_fir_filter_with_buffer_ccc::t2()
      {
	test_decimate(2);
      }

      void
      qa_fir_filter_with_buffer_ccc::t3()
      {
	test_decimate(5);
      }

      //
      // Test for ntaps in [0,9], and input lengths in [0,17].
      // This ensures that we are building the shifted taps correctly,
      // and exercises all corner cases on input alignment and length.
      //
      void
      qa_fir_filter_with_buffer_ccc::test_decimate(unsigned int decimate)
      {
	const int MAX_TAPS   = 29;
	const int OUTPUT_LEN = 37;
	const int INPUT_LEN  = MAX_TAPS + OUTPUT_LEN;

	// Mem aligned buffer not really necessary, but why not?
	i_type   *input = fft::malloc_complex(INPUT_LEN);
	i_type   *dline = fft::malloc_complex(INPUT_LEN);
	o_type   *expected_output = fft::malloc_complex(OUTPUT_LEN);
	o_type   *actual_output = fft::malloc_complex(OUTPUT_LEN);
	tap_type *taps = fft::malloc_complex(MAX_TAPS);

	srandom(0);	// we want reproducibility
	memset(dline, 0, INPUT_LEN*sizeof(i_type));

	for(int n = 0; n <= MAX_TAPS; n++) {
	  for(int ol = 0; ol <= OUTPUT_LEN; ol++) {

	    // build random test case
	    random_complex(input, INPUT_LEN);
	    random_complex(taps, MAX_TAPS);

	    // compute expected output values
	    memset(dline, 0, INPUT_LEN*sizeof(i_type));
	    for(int o = 0; o < (int)(ol/decimate); o++) {
	      // use an actual delay line for this test
	      for(int dd = 0; dd < (int)decimate; dd++) {
		for(int oo = INPUT_LEN-1; oo > 0; oo--)
		  dline[oo] = dline[oo-1];
		dline[0] = input[decimate*o+dd];
	      }
	      expected_output[o] = ref_dotprod(dline, taps, n);
	    }

	    // build filter
	    vector<tap_type> f1_taps(&taps[0], &taps[n]);
	    kernel::fir_filter_with_buffer_ccc *f1 = \
	      new kernel::fir_filter_with_buffer_ccc(f1_taps);

	    // zero the output, then do the filtering
	    memset(actual_output, 0, OUTPUT_LEN*sizeof(o_type));
	    f1->filterNdec(actual_output, input, ol/decimate, decimate);

	    // check results
	    //
	    // we use a sloppy error margin because on the x86 architecture,
	    // our reference implementation is using 80 bit floating point
	    // arithmetic, while the SSE version is using 32 bit float point
	    // arithmetic.

	    for(int o = 0; o < (int)(ol/decimate); o++) {
	      CPPUNIT_ASSERT_COMPLEXES_EQUAL(expected_output[o], actual_output[o],
			      sqrt((float)n)*0.25*MAX_DATA*MAX_DATA * ERR_DELTA);
	    }
	    delete f1;
	  }
	}
	fft::free(input);
	fft::free(dline);
	fft::free(expected_output);
	fft::free(actual_output);
	fft::free(taps);
      }

    } /* namespace ccc */


    /**************************************************************/

    namespace ccf {

      typedef gr_complex i_type;
      typedef gr_complex o_type;
      typedef float      tap_type;
      typedef gr_complex acc_type;

      using std::vector;

      static o_type
      ref_dotprod(const i_type input[], const tap_type taps[], int ntaps)
      {
	acc_type sum = 0;
	for(int i = 0; i < ntaps; i++) {
	  sum += input[i] * taps[i];
	}

	//return gr_complex(121,9)*sum; 
	return sum;
     }

      void
      qa_fir_filter_with_buffer_ccf::t1()
      {
	test_decimate(1);
      }

      void
      qa_fir_filter_with_buffer_ccf::t2()
      {
	test_decimate(2);
      }

      void
      qa_fir_filter_with_buffer_ccf::t3()
      {
	test_decimate(5);
      }

      //
      // Test for ntaps in [0,9], and input lengths in [0,17].
      // This ensures that we are building the shifted taps correctly,
      // and exercises all corner cases on input alignment and length.
      //
      void
      qa_fir_filter_with_buffer_ccf::test_decimate(unsigned int decimate)
      {
	const int MAX_TAPS   = 29;
	const int OUTPUT_LEN = 37;
	const int INPUT_LEN  = MAX_TAPS + OUTPUT_LEN;

	// Mem aligned buffer not really necessary, but why not?
	i_type   *input = fft::malloc_complex(INPUT_LEN);
	i_type   *dline = fft::malloc_complex(INPUT_LEN);
	o_type   *expected_output = fft::malloc_complex(OUTPUT_LEN);
	o_type   *actual_output = fft::malloc_complex(OUTPUT_LEN);
	tap_type *taps = fft::malloc_float(MAX_TAPS);

	srandom(0);	// we want reproducibility
	memset(dline, 0, INPUT_LEN*sizeof(i_type));

	for(int n = 0; n <= MAX_TAPS; n++) {
	  for(int ol = 0; ol <= OUTPUT_LEN; ol++) {

	    // build random test case
	    random_complex(input, INPUT_LEN);
	    random_floats(taps, MAX_TAPS);

	    // compute expected output values
	    memset(dline, 0, INPUT_LEN*sizeof(i_type));
	    for(int o = 0; o < (int)(ol/decimate); o++) {
	      // use an actual delay line for this test
	      for(int dd = 0; dd < (int)decimate; dd++) {
		for(int oo = INPUT_LEN-1; oo > 0; oo--)
		  dline[oo] = dline[oo-1];
		dline[0] = input[decimate*o+dd];
	      }
	      expected_output[o] = ref_dotprod(dline, taps, n);
	    }

	    // build filter
	    vector<tap_type> f1_taps(&taps[0], &taps[n]);
	    kernel::fir_filter_with_buffer_ccf *f1 = \
	      new kernel::fir_filter_with_buffer_ccf(f1_taps);

	    // zero the output, then do the filtering
	    memset(actual_output, 0, sizeof(OUTPUT_LEN*sizeof(gr_complex)));
	    f1->filterNdec(actual_output, input, ol/decimate, decimate);

	    // check results
	    //
	    // we use a sloppy error margin because on the x86 architecture,
	    // our reference implementation is using 80 bit floating point
	    // arithmetic, while the SSE version is using 32 bit float point
	    // arithmetic.

	    for(int o = 0; o < (int)(ol/decimate); o++) {
	      CPPUNIT_ASSERT_COMPLEXES_EQUAL(expected_output[o], actual_output[o],
			      sqrt((float)n)*0.25*MAX_DATA*MAX_DATA * ERR_DELTA);
	    }
	    delete f1;
	  }
	}
	fft::free(input);
	fft::free(dline);
	fft::free(expected_output);
	fft::free(actual_output);
	fft::free(taps);
      }

    } /* namespace ccf */

  } /* namespace filter */
} /* namespace gr */

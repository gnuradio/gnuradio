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

#include <gnuradio/types.h>
#include <gnuradio/filter/fir_filter_with_buffer.h>
#include <gnuradio/fft/fft.h>
#include <gnuradio/random.h>
#include <volk/volk.h>
#include <boost/test/unit_test.hpp>
#include <cmath>
#include <cstring>

using std::vector;

namespace gr {
  namespace filter {

#define MAX_DATA        (16383)
#define	ERR_DELTA	(1e-5)

    static gr::random rndm;

    static float
    uniform()
    {
      return 2.0 * (rndm.ran1() - 0.5); // uniformly (-1, 1)
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

      static o_type
      ref_dotprod(const i_type input[], const tap_type taps[], int ntaps)
      {
	acc_type sum = 0;
	for (int i = 0; i < ntaps; i++) {
	  sum += input[i] * taps[i];
	}
	return sum;
      }

      //
      // Test for ntaps in [0,9], and input lengths in [0,17].
      // This ensures that we are building the shifted taps correctly,
      // and exercises all corner cases on input alignment and length.
      //
      void test_decimate(unsigned int decimate)
      {
	const int MAX_TAPS   = 29;
	const int OUTPUT_LEN = 37;
	const int INPUT_LEN  = MAX_TAPS + OUTPUT_LEN;
        size_t align = volk_get_alignment();

	// Mem aligned buffer not really necessary, but why not?
	i_type   *input = (float*)volk_malloc(INPUT_LEN*sizeof(float), align);
	i_type   *dline = (float*)volk_malloc(INPUT_LEN*sizeof(float), align);
	o_type   *expected_output = (float*)volk_malloc(OUTPUT_LEN*sizeof(float), align);
	o_type   *actual_output = (float*)volk_malloc(OUTPUT_LEN*sizeof(float), align);
	tap_type *taps = (float*)volk_malloc(MAX_TAPS*sizeof(float), align);

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
	      BOOST_CHECK(std::abs(expected_output[o] - actual_output[o]) <=
			     sqrt((float)n)*0.25*MAX_DATA*MAX_DATA * ERR_DELTA);
	    }
	    delete f1;
	  }
	}
	volk_free(input);
	volk_free(dline);
	volk_free(expected_output);
	volk_free(actual_output);
	volk_free(taps);
      }

      BOOST_AUTO_TEST_CASE(t1_fff)
      {
	test_decimate(1);
      }

      BOOST_AUTO_TEST_CASE(t2_fff)
      {
	test_decimate(2);
      }

      BOOST_AUTO_TEST_CASE(t3_fff)
      {
	test_decimate(5);
      }


    } /* namespace fff */


    /**************************************************************/


    namespace ccc {

      typedef gr_complex i_type;
      typedef gr_complex o_type;
      typedef gr_complex tap_type;
      typedef gr_complex acc_type;


      static o_type
      ref_dotprod(const i_type input[], const tap_type taps[], int ntaps)
      {
	acc_type sum = 0;
	for(int i = 0; i < ntaps; i++) {
	  sum += input[i] * taps[i];
	}

	return sum;
      }

      //
      // Test for ntaps in [0,9], and input lengths in [0,17].
      // This ensures that we are building the shifted taps correctly,
      // and exercises all corner cases on input alignment and length.
      //
      void
      test_decimate(unsigned int decimate)
      {
	const int MAX_TAPS   = 29;
	const int OUTPUT_LEN = 37;
	const int INPUT_LEN  = MAX_TAPS + OUTPUT_LEN;
        size_t align = volk_get_alignment();

	// Mem aligned buffer not really necessary, but why not?
	i_type   *input = (gr_complex*)volk_malloc(INPUT_LEN*sizeof(gr_complex), align);
	i_type   *dline = (gr_complex*)volk_malloc(INPUT_LEN*sizeof(gr_complex), align);
	o_type   *expected_output = (gr_complex*)volk_malloc(OUTPUT_LEN*sizeof(gr_complex), align);
	o_type   *actual_output = (gr_complex*)volk_malloc(OUTPUT_LEN*sizeof(gr_complex), align);
	tap_type *taps = (gr_complex*)volk_malloc(MAX_TAPS*sizeof(gr_complex), align);

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
	      BOOST_CHECK(std::abs(expected_output[o] - actual_output[o]) <=
			      sqrt((float)n)*0.25*MAX_DATA*MAX_DATA * ERR_DELTA);
	    }
	    delete f1;
	  }
	}
	volk_free(input);
	volk_free(dline);
	volk_free(expected_output);
	volk_free(actual_output);
	volk_free(taps);
      }

      BOOST_AUTO_TEST_CASE(t1_ccc)
      {
	test_decimate(1);
      }

      BOOST_AUTO_TEST_CASE(t2_ccc)
      {
	test_decimate(2);
      }

      BOOST_AUTO_TEST_CASE(t3_ccc)
      {
	test_decimate(5);
      }

    } /* namespace ccc */


    /**************************************************************/

    namespace ccf {

      typedef gr_complex i_type;
      typedef gr_complex o_type;
      typedef float      tap_type;
      typedef gr_complex acc_type;

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

      //
      // Test for ntaps in [0,9], and input lengths in [0,17].
      // This ensures that we are building the shifted taps correctly,
      // and exercises all corner cases on input alignment and length.
      //
      void test_decimate(unsigned int decimate)
      {
	const int MAX_TAPS   = 29;
	const int OUTPUT_LEN = 37;
	const int INPUT_LEN  = MAX_TAPS + OUTPUT_LEN;
        size_t align = volk_get_alignment();

	// Mem aligned buffer not really necessary, but why not?
	i_type   *input = (gr_complex*)volk_malloc(INPUT_LEN*sizeof(gr_complex), align);
	i_type   *dline = (gr_complex*)volk_malloc(INPUT_LEN*sizeof(gr_complex), align);
	o_type   *expected_output = (gr_complex*)volk_malloc(OUTPUT_LEN*sizeof(gr_complex), align);
	o_type   *actual_output = (gr_complex*)volk_malloc(OUTPUT_LEN*sizeof(gr_complex), align);
	tap_type *taps = (float*)volk_malloc(MAX_TAPS*sizeof(float), align);

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
	    memset(actual_output, 0, OUTPUT_LEN*sizeof(gr_complex));
	    f1->filterNdec(actual_output, input, ol/decimate, decimate);

	    // check results
	    //
	    // we use a sloppy error margin because on the x86 architecture,
	    // our reference implementation is using 80 bit floating point
	    // arithmetic, while the SSE version is using 32 bit float point
	    // arithmetic.

	    for(int o = 0; o < (int)(ol/decimate); o++) {
	      BOOST_CHECK(std::abs(expected_output[o] - actual_output[o]) <=
			      sqrt((float)n)*0.25*MAX_DATA*MAX_DATA * ERR_DELTA);
	    }
	    delete f1;
	  }
	}
	volk_free(input);
	volk_free(dline);
	volk_free(expected_output);
	volk_free(actual_output);
	volk_free(taps);
      }

      BOOST_AUTO_TEST_CASE(t1_ccf)
      {
	test_decimate(1);
      }

      BOOST_AUTO_TEST_CASE(t2_ccf)
      {
	test_decimate(2);
      }

      BOOST_AUTO_TEST_CASE(t3_ccf)
      {
	test_decimate(5);
      }

    } /* namespace ccf */

  } /* namespace filter */
} /* namespace gr */

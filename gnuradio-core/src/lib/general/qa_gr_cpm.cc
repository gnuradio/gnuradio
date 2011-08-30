/* -*- c++ -*- */
/*
 * Copyright 2010 Free Software Foundation, Inc.
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

#include <qa_gr_cpm.h>
#include <gr_cpm.h>
#include <cppunit/TestAssert.h>

const double DELTA = 1e-5;
const int L = 5;
const int samples_per_sym = 4;
const float taps_lrc[20] = {
                   0, 0.002447174185242, 0.009549150281253, 0.020610737385376,
   0.034549150281253, 0.050000000000000, 0.065450849718747, 0.079389262614624,
   0.090450849718747, 0.097552825814758, 0.100000000000000, 0.097552825814758,
   0.090450849718747, 0.079389262614624, 0.065450849718747, 0.050000000000000,
   0.034549150281253, 0.020610737385376, 0.009549150281253, 0.002447174185242
};


const float taps_lsrc[20] = { // beta = 0.2
   0.000000000000000, 0.009062686687436, 0.019517618142920, 0.030875041875917,
   0.042552315421249, 0.053912556756416, 0.064308860403517, 0.073130584159352,
   0.079847961304114, 0.084051371489937, 0.085482007518284, 0.084051371489937,
   0.079847961304114, 0.073130584159352, 0.064308860403517, 0.053912556756416,
   0.042552315421249, 0.030875041875917, 0.019517618142920, 0.009062686687436
};


const float taps_tfm[20] = {
  -0.003946522220317, -0.005147757690530, -0.003171631690177,  0.003959659609805,
   0.017498721302356,  0.037346982678383,  0.062251889790391,  0.087364237065604,
   0.110049050955117,  0.125677762224511,  0.132288693729399,  0.125677762224511,
   0.110049050955117,  0.087364237065604,  0.062251889790391,  0.037346982678383,
   0.017498721302356,  0.003959659609805, -0.003171631690177, -0.005147757690530
};


const float taps_gaussian[20] = { // BT = 0.3
   0.000000743866524, 0.000009286258371, 0.000085441834550, 0.000581664421923,
   0.002945540765422, 0.011178079812344, 0.032117220937421, 0.070841188736816,
   0.122053715366673, 0.167389736919915, 0.185594670675172, 0.167389736919915,
   0.122053715366673, 0.070841188736816, 0.032117220937421, 0.011178079812344,
   0.002945540765422, 0.000581664421923, 0.000085441834550, 0.000009286258371
};


// Check LREC phase response
void
qa_gr_cpm::t1 ()
{
	std::vector<float> taps(gr_cpm::phase_response(gr_cpm::LREC, samples_per_sym, L));

	for (int i = 0; i < L * samples_per_sym; i++) {
		CPPUNIT_ASSERT_DOUBLES_EQUAL(taps[i], 0.05, DELTA);
	}
}


// Check LRC phase response
void
qa_gr_cpm::t2 ()
{
	std::vector<float> taps(gr_cpm::phase_response(gr_cpm::LRC, samples_per_sym, L));
	float sum = 0;

	for (int i = 0; i < L * samples_per_sym; i++) {
		CPPUNIT_ASSERT_DOUBLES_EQUAL(taps[i], taps_lrc[i], DELTA);
		sum += taps[i];
	}

	CPPUNIT_ASSERT_DOUBLES_EQUAL(sum, 1.0, DELTA);
}


// Check LSRC phase response
void
qa_gr_cpm::t3 ()
{
	std::vector<float> taps(gr_cpm::phase_response(gr_cpm::LSRC, samples_per_sym, L, 0.2));
	float sum = 0;

	for (int i = 0; i < L * samples_per_sym; i++) {
		CPPUNIT_ASSERT_DOUBLES_EQUAL(taps[i], taps_lsrc[i], DELTA);
		sum += taps[i];
	}

	CPPUNIT_ASSERT_DOUBLES_EQUAL(sum, 1.0, DELTA);
}


// Check the TFM phase response
void
qa_gr_cpm::t4 ()
{
	std::vector<float> taps(gr_cpm::phase_response(gr_cpm::TFM, samples_per_sym, L));
	float sum = 0;

	for (int i = 0; i < L * samples_per_sym; i++) {
		CPPUNIT_ASSERT_DOUBLES_EQUAL(taps[i], taps_tfm[i], DELTA);
		sum += taps[i];
	}

	CPPUNIT_ASSERT_DOUBLES_EQUAL(sum, 1.0, DELTA);
}


// Check the Gaussian phase response
void
qa_gr_cpm::t5 ()
{
	std::vector<float> taps(gr_cpm::phase_response(gr_cpm::GAUSSIAN, samples_per_sym, L, 0.3));
	float sum = 0;

	for (int i = 0; i < L * samples_per_sym; i++) {
		CPPUNIT_ASSERT_DOUBLES_EQUAL(taps[i], taps_gaussian[i], DELTA);
		sum += taps[i];
	}

	CPPUNIT_ASSERT_DOUBLES_EQUAL(sum, 1.0, DELTA);
}


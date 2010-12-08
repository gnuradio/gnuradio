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

// Check LREC phase response
void
qa_gr_cpm::t1 ()
{
	int L = 5;
	int samples_per_sym = 4;
	std::vector<float> taps(gr_cpm::phase_response(gr_cpm::LREC, samples_per_sym, L));

	for (int i = 0; i < L * samples_per_sym; i++) {
		CPPUNIT_ASSERT_DOUBLES_EQUAL(taps[i], 0.05, DELTA);
	}
}


// Check LRC phase response
void
qa_gr_cpm::t2 ()
{
	int L = 5;
	int samples_per_sym = 4;
	std::vector<float> taps(gr_cpm::phase_response(gr_cpm::LRC, samples_per_sym, L));
	float sum = 0;

	for (int i = 0; i < L * samples_per_sym; i++) {
		sum += taps[i];
	}

	CPPUNIT_ASSERT_DOUBLES_EQUAL(sum, 1.0, DELTA);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(taps[L*samples_per_sym/2], 0.1, DELTA);
}


// Check LSRC phase response
void
qa_gr_cpm::t3 ()
{
	int L = 5;
	int samples_per_sym = 4;
	std::vector<float> taps(gr_cpm::phase_response(gr_cpm::LSRC, samples_per_sym, L, 0.2));
	float sum = 0;

	for (int i = 0; i < L * samples_per_sym; i++) {
		sum += taps[i];
	}

	CPPUNIT_ASSERT_DOUBLES_EQUAL(sum, 1.0, DELTA);
}


// Check the TFM phase response
void
qa_gr_cpm::t4 ()
{
	int L = 5;
	int samples_per_sym = 4;
	std::vector<float> taps(gr_cpm::phase_response(gr_cpm::TFM, samples_per_sym, L));
	float sum = 0;

	for (int i = 0; i < L * samples_per_sym; i++) {
		sum += taps[i];
	}

	CPPUNIT_ASSERT_DOUBLES_EQUAL(sum, 1.0, DELTA);
}


// Check the Gaussian phase response
void
qa_gr_cpm::t5 ()
{
	int L = 5;
	int samples_per_sym = 4;
	std::vector<float> taps(gr_cpm::phase_response(gr_cpm::GAUSSIAN, samples_per_sym, L, 0.3));
	float sum = 0;

	for (int i = 0; i < L * samples_per_sym; i++) {
		sum += taps[i];
	}

	CPPUNIT_ASSERT_DOUBLES_EQUAL(sum, 1.0, 1e-4);
}


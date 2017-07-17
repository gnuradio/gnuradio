/*
 * Copyright (C) 2012,2017 Free Software Foundation, Inc.
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

/*
 * This class gathers together all the test cases for the gr-filter
 * directory into a single test suite.  As you create new test cases,
 * add them here.
 */

#include <qa_filter.h>
#include <qa_firdes.h>
#include <qa_fir_filter_with_buffer.h>
#include <qa_mmse_fir_interpolator_cc.h>
#include <qa_mmse_fir_interpolator_ff.h>
#include <qa_mmse_interp_differentiator_cc.h>
#include <qa_mmse_interp_differentiator_ff.h>

CppUnit::TestSuite *
qa_gr_filter::suite ()
{
  CppUnit::TestSuite *s = new CppUnit::TestSuite ("gr-filter");

  s->addTest(gr::filter::qa_firdes::suite());
  s->addTest(gr::filter::fff::qa_fir_filter_with_buffer_fff::suite());
  s->addTest(gr::filter::ccc::qa_fir_filter_with_buffer_ccc::suite());
  s->addTest(gr::filter::ccf::qa_fir_filter_with_buffer_ccf::suite());
  s->addTest(gr::filter::qa_mmse_fir_interpolator_cc::suite());
  s->addTest(gr::filter::qa_mmse_fir_interpolator_ff::suite());
  s->addTest(gr::filter::qa_mmse_interp_differentiator_cc::suite());
  s->addTest(gr::filter::qa_mmse_interp_differentiator_ff::suite());

  return s;
}

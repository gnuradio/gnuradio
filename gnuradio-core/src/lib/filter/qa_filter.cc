/*
 * Copyright 2002,2007 Free Software Foundation, Inc.
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
 * This class gathers together all the test cases for the gr
 * directory into a single test suite.  As you create new test cases,
 * add them here.
 */

#include <qa_filter.h>
#include <qa_gr_fir_ccf.h>
#include <qa_gr_fir_fff.h>
#include <qa_gr_fir_ccc.h>
#include <qa_gr_fir_fcc.h>
#include <qa_gr_fir_scc.h>
#include <qa_gr_firdes.h>
#include <qa_dotprod.h>
#include <qa_gri_mmse_fir_interpolator.h>
#include <qa_gri_mmse_fir_interpolator_cc.h>
#include <qa_gr_rotator.h>
#include <qa_gri_fir_filter_with_buffer_ccf.h>
#include <qa_gri_fir_filter_with_buffer_ccc.h>
#include <qa_gri_fir_filter_with_buffer_fcc.h>
#include <qa_gri_fir_filter_with_buffer_fff.h>
#include <qa_gri_fir_filter_with_buffer_fsf.h>
#include <qa_gri_fir_filter_with_buffer_scc.h>

CppUnit::TestSuite *
qa_filter::suite ()
{
  CppUnit::TestSuite	*s = new CppUnit::TestSuite ("filter");

  s->addTest (qa_dotprod_suite ());
  s->addTest (qa_gr_fir_fff::suite ());
  s->addTest (qa_gr_fir_ccc::suite ());
  s->addTest (qa_gr_fir_fcc::suite ());
  s->addTest (qa_gr_fir_scc::suite ());
  s->addTest (qa_gr_fir_ccf::suite ());
  s->addTest (qa_gri_mmse_fir_interpolator::suite ());
  s->addTest (qa_gri_mmse_fir_interpolator_cc::suite ());
  s->addTest (qa_gr_rotator::suite ());
  s->addTest (qa_gri_fir_filter_with_buffer_ccf::suite ());
  s->addTest (qa_gri_fir_filter_with_buffer_ccc::suite ());
  s->addTest (qa_gri_fir_filter_with_buffer_fcc::suite ());
  s->addTest (qa_gri_fir_filter_with_buffer_fff::suite ());
  s->addTest (qa_gri_fir_filter_with_buffer_fsf::suite ());
  s->addTest (qa_gri_fir_filter_with_buffer_scc::suite ());

  return s;
}

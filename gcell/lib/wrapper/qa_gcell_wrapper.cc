/* -*- c++ -*- */
/*
 * Copyright 2007,2009 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/*
 * This class gathers together all the test cases for the lib/wrapper
 * directory into a single test suite.  As you create new test cases,
 * add them here.
 */

#include <qa_gcell_wrapper.h>
#include <qa_gcell_general.h>
//#include <qa_gcp_fft_1d_r2.h>

CppUnit::TestSuite *
qa_gcell_wrapper::suite()
{
  CppUnit::TestSuite	*s = new CppUnit::TestSuite("wrapper");

  s->addTest(qa_gcell_general::suite());
  //s->addTest(qa_gcp_fft_1d_r2::suite());

  return s;
}

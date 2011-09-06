/*
 * Copyright 2002 Free Software Foundation, Inc.
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

#include <qa_general.h>
#include <qa_gr_firdes.h>
#include <qa_gr_circular_file.h>
#include <qa_gr_cpm.h>
#include <qa_gr_fxpt.h>
#include <qa_gr_fxpt_nco.h>
#include <qa_gr_fxpt_vco.h>
#include <qa_gr_math.h>
#include <qa_gri_lfsr.h>

CppUnit::TestSuite *
qa_general::suite ()
{
  CppUnit::TestSuite	*s = new CppUnit::TestSuite ("general");

  s->addTest (qa_gr_firdes::suite ());
  s->addTest (qa_gr_circular_file::suite ());
  s->addTest (qa_gr_cpm::suite ());
  s->addTest (qa_gr_fxpt::suite ());
  s->addTest (qa_gr_fxpt_nco::suite ());
  s->addTest (qa_gr_fxpt_vco::suite ());
  s->addTest (qa_gr_math::suite ());
  s->addTest (qa_gri_lfsr::suite ());
  
  return s;
}

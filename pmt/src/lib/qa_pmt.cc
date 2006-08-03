/*
 * Copyright 2006 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * This class gathers together all the test cases for pmt into
 * a single test suite.  As you create new test cases, add them here.
 */

#include <qa_pmt.h>
#include <qa_pmt_prims.h>

CppUnit::TestSuite *
qa_pmt::suite ()
{
  CppUnit::TestSuite	*s = new CppUnit::TestSuite ("pmt");

  s->addTest (qa_pmt_prims::suite ());
  //s->addTest (qa_gr_circular_file::suite ());
  //s->addTest (qa_gr_fxpt::suite ());
  
  return s;
}

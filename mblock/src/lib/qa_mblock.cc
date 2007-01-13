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
 * This class gathers together all the test cases for mblock into
 * a single test suite.  As you create new test cases, add them here.
 */

#include <qa_mblock.h>
#include <qa_mblock_prims.h>

CppUnit::TestSuite *
qa_mblock::suite()
{
  CppUnit::TestSuite	*s = new CppUnit::TestSuite("mblock");

  s->addTest (qa_mblock_prims::suite());
  
  return s;
}

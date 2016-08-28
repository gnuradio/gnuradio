/*
 * Copyright 2016 Andrew Jeffery
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
 * This class gathers together all the test cases for the gr-pager
 * directory into a single test suite.  As you create new test cases,
 * add them here.
 */

#include "qa_pager.h"
#include "qa_flex_parse_impl.h"

CppUnit::TestSuite *
qa_gr_pager::suite ()
{
  CppUnit::TestSuite *s = new CppUnit::TestSuite ("gr-pager");
  s->addTest(gr::pager::qa_flex_parse_impl::suite());

  return s;
}

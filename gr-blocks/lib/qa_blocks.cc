/*
 * Copyright 2012-2013 Free Software Foundation, Inc.
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
 * This class gathers together all the test cases for the gr-blocks
 * directory into a single test suite.  As you create new test cases,
 * add them here.
 */

#include <qa_blocks.h>
#include <qa_block_tags.h>
#include <qa_rotator.h>
#include <qa_gr_block.h>
#include <qa_gr_flowgraph.h>
#include <qa_set_msg_handler.h>

CppUnit::TestSuite *
qa_blocks::suite()
{
  CppUnit::TestSuite *s = new CppUnit::TestSuite("gr-blocks");

  s->addTest(qa_block_tags::suite());
  s->addTest(qa_rotator::suite());
  s->addTest(qa_gr_block::suite());
  s->addTest(qa_gr_flowgraph::suite());
  s->addTest(qa_set_msg_handler::suite());
  return s;
}

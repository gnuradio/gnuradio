/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <qa_gr_block.h>
#include <gr_block.h>
#include <gr_io_signature.h>
#include <gr_null_sink.h>
#include <gr_null_source.h>


// ----------------------------------------------------------------


void
qa_gr_block::t0 ()
{
  // test creation of sources
  gr_block_sptr src1 (gr_make_null_source (sizeof (int)));
  CPPUNIT_ASSERT_EQUAL (std::string ("null_source"), src1->name ());
  CPPUNIT_ASSERT_EQUAL (0, src1->input_signature()->max_streams ());
  CPPUNIT_ASSERT_EQUAL (1, src1->output_signature()->min_streams ());
  CPPUNIT_ASSERT_EQUAL (1, src1->output_signature()->max_streams ());
  CPPUNIT_ASSERT_EQUAL ((int) sizeof(int),
			src1->output_signature()->sizeof_stream_item (0));

  gr_block_sptr src2 (gr_make_null_source (sizeof (short)));
  CPPUNIT_ASSERT_EQUAL (std::string ("null_source"), src2->name ());
  CPPUNIT_ASSERT_EQUAL (0, src2->input_signature()->max_streams ());
  CPPUNIT_ASSERT_EQUAL (1, src2->output_signature()->min_streams ());
  CPPUNIT_ASSERT_EQUAL (1, src2->output_signature()->max_streams ());
  CPPUNIT_ASSERT_EQUAL ((int) sizeof (short),
			src2->output_signature()->sizeof_stream_item (0));
}


void
qa_gr_block::t1 ()
{
  // test creation of sinks
  gr_block_sptr dst1 (gr_make_null_sink (sizeof (int)));
  CPPUNIT_ASSERT_EQUAL (std::string ("null_sink"), dst1->name ());
  CPPUNIT_ASSERT_EQUAL (1, dst1->input_signature()->min_streams ());
  CPPUNIT_ASSERT_EQUAL (1, dst1->input_signature()->max_streams ());
  CPPUNIT_ASSERT_EQUAL ((int) sizeof (int),
			dst1->input_signature()->sizeof_stream_item (0));

  CPPUNIT_ASSERT_EQUAL (0, dst1->output_signature()->max_streams ());

  gr_block_sptr dst2 (gr_make_null_sink (sizeof (short)));
  CPPUNIT_ASSERT_EQUAL (std::string ("null_sink"), dst2->name ());
  CPPUNIT_ASSERT_EQUAL (1, dst2->input_signature()->min_streams ());
  CPPUNIT_ASSERT_EQUAL (1, dst2->input_signature()->max_streams ());
  CPPUNIT_ASSERT_EQUAL ((int) sizeof (short),
			dst2->input_signature()->sizeof_stream_item (0));
  CPPUNIT_ASSERT_EQUAL (0, dst2->output_signature()->max_streams ());
}

void
qa_gr_block::t2 ()
{
}

void
qa_gr_block::t3 ()
{
}

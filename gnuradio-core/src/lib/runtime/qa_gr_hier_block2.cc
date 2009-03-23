/* -*- c++ -*- */
/*
 * Copyright 2006,2008,2009 Free Software Foundation, Inc.
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

#include <qa_gr_hier_block2.h>
#include <gr_hier_block2.h>
#include <gr_io_signature.h>
#include <gr_null_source.h>
#include <gr_null_sink.h>

void qa_gr_hier_block2::test_make()
{
    gr_hier_block2_sptr src1(gr_make_hier_block2("test",
						 gr_make_io_signature(1, 1, 1 * sizeof(int)),
						 gr_make_io_signature(1, 1, 1 * sizeof(int))));

    CPPUNIT_ASSERT(src1);
    CPPUNIT_ASSERT_EQUAL(std::string("test"), src1->name());

    CPPUNIT_ASSERT_EQUAL(1 * (int) sizeof(int), 
			 src1->input_signature()->sizeof_stream_item(0));

    CPPUNIT_ASSERT_EQUAL(1, src1->input_signature()->min_streams());
    CPPUNIT_ASSERT_EQUAL(1, src1->input_signature()->max_streams());


    CPPUNIT_ASSERT_EQUAL(1 * (int) sizeof(int), 
			 src1->output_signature()->sizeof_stream_item(0));

    CPPUNIT_ASSERT_EQUAL(1, src1->output_signature()->min_streams());
    CPPUNIT_ASSERT_EQUAL(1, src1->output_signature()->max_streams());

}



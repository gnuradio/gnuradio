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

#include <gnuradio/hier_block2.h>
#include <gnuradio/io_signature.h>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(test_make)
{
  gr::hier_block2_sptr src1(gr::make_hier_block2("test",
						 gr::io_signature::make(1, 1, 1 * sizeof(int)),
						 gr::io_signature::make(1, 1, 1 * sizeof(int))));

    BOOST_REQUIRE(src1);
    BOOST_REQUIRE_EQUAL(std::string("test"), src1->name());

    BOOST_REQUIRE_EQUAL(1 * (int) sizeof(int),
			 src1->input_signature()->sizeof_stream_item(0));

    BOOST_REQUIRE_EQUAL(1, src1->input_signature()->min_streams());
    BOOST_REQUIRE_EQUAL(1, src1->input_signature()->max_streams());


    BOOST_REQUIRE_EQUAL(1 * (int) sizeof(int),
			 src1->output_signature()->sizeof_stream_item(0));

    BOOST_REQUIRE_EQUAL(1, src1->output_signature()->min_streams());
    BOOST_REQUIRE_EQUAL(1, src1->output_signature()->max_streams());
}


/* -*- c++ -*- */
/*
 * Copyright 2006,2008,2009 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnuradio/hier_block2.h>
#include <gnuradio/io_signature.h>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(test_make)
{
    gr::hier_block2_sptr src1(
        gr::make_hier_block2("test",
                             gr::io_signature::make(1, 1, 1 * sizeof(int)),
                             gr::io_signature::make(1, 1, 1 * sizeof(int))));

    BOOST_REQUIRE(src1);
    BOOST_REQUIRE_EQUAL(std::string("test"), src1->name());

    BOOST_REQUIRE_EQUAL(1 * (int)sizeof(int),
                        src1->input_signature()->sizeof_stream_item(0));

    BOOST_REQUIRE_EQUAL(1, src1->input_signature()->min_streams());
    BOOST_REQUIRE_EQUAL(1, src1->input_signature()->max_streams());


    BOOST_REQUIRE_EQUAL(1 * (int)sizeof(int),
                        src1->output_signature()->sizeof_stream_item(0));

    BOOST_REQUIRE_EQUAL(1, src1->output_signature()->min_streams());
    BOOST_REQUIRE_EQUAL(1, src1->output_signature()->max_streams());
}

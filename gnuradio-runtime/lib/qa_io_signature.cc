/* -*- c++ -*- */
/*
 * Copyright 2004,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnuradio/io_signature.h>
#include <boost/test/unit_test.hpp>
#include <stdexcept>

BOOST_AUTO_TEST_CASE(t0) { gr::io_signature::make(1, 1, sizeof(int)); }

BOOST_AUTO_TEST_CASE(t1)
{
    BOOST_REQUIRE_THROW(gr::io_signature::make(3, 1, sizeof(int)), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(t2)
{
    gr::io_signature::sptr p =
        gr::io_signature::make(3, gr::io_signature::IO_INFINITE, sizeof(int));

    BOOST_CHECK_EQUAL(p->min_streams(), 3);
    BOOST_CHECK_EQUAL(p->sizeof_stream_item(0), (int)sizeof(int));
}

BOOST_AUTO_TEST_CASE(t3)
{
    gr::io_signature::sptr p = gr::io_signature::make3(0, 5, 1, 2, 3);

    BOOST_CHECK_EQUAL(p->min_streams(), 0);
    BOOST_CHECK_EQUAL(p->max_streams(), 5);
    BOOST_CHECK_EQUAL(p->sizeof_stream_item(0), 1);
    BOOST_CHECK_EQUAL(p->sizeof_stream_item(1), 2);
    BOOST_CHECK_EQUAL(p->sizeof_stream_item(2), 3);
    BOOST_CHECK_EQUAL(p->sizeof_stream_item(3), 3);
    BOOST_CHECK_EQUAL(p->sizeof_stream_item(4), 3);
}

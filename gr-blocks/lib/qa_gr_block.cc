/* -*- c++ -*- */
/*
 * Copyright 2004.2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnuradio/block.h>
#include <gnuradio/blocks/null_sink.h>
#include <gnuradio/blocks/null_source.h>
#include <gnuradio/buffer_reader.h>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(t0)
{
    // test creation of sources
    gr::block_sptr src1(gr::blocks::null_source::make(sizeof(int)));
    BOOST_REQUIRE_EQUAL(std::string("null_source"), src1->name());
    BOOST_REQUIRE_EQUAL(0, src1->input_signature()->max_streams());
    BOOST_REQUIRE_EQUAL(1, src1->output_signature()->min_streams());
    BOOST_REQUIRE_EQUAL(-1, src1->output_signature()->max_streams());
    BOOST_REQUIRE_EQUAL((int)sizeof(int),
                        src1->output_signature()->sizeof_stream_item(0));

    gr::block_sptr src2(gr::blocks::null_source::make(sizeof(short)));
    BOOST_REQUIRE_EQUAL(std::string("null_source"), src2->name());
    BOOST_REQUIRE_EQUAL(0, src2->input_signature()->max_streams());
    BOOST_REQUIRE_EQUAL(1, src2->output_signature()->min_streams());
    BOOST_REQUIRE_EQUAL(-1, src2->output_signature()->max_streams());
    BOOST_REQUIRE_EQUAL((int)sizeof(short),
                        src2->output_signature()->sizeof_stream_item(0));
}


BOOST_AUTO_TEST_CASE(t1)
{
    // test creation of sinks
    gr::block_sptr dst1(gr::blocks::null_sink::make(sizeof(int)));
    BOOST_REQUIRE_EQUAL(std::string("null_sink"), dst1->name());
    BOOST_REQUIRE_EQUAL(1, dst1->input_signature()->min_streams());
    BOOST_REQUIRE_EQUAL(-1, dst1->input_signature()->max_streams());
    BOOST_REQUIRE_EQUAL((int)sizeof(int), dst1->input_signature()->sizeof_stream_item(0));

    BOOST_REQUIRE_EQUAL(0, dst1->output_signature()->max_streams());

    gr::block_sptr dst2(gr::blocks::null_sink::make(sizeof(short)));
    BOOST_REQUIRE_EQUAL(std::string("null_sink"), dst2->name());
    BOOST_REQUIRE_EQUAL(1, dst2->input_signature()->min_streams());
    BOOST_REQUIRE_EQUAL(-1, dst2->input_signature()->max_streams());
    BOOST_REQUIRE_EQUAL((int)sizeof(short),
                        dst2->input_signature()->sizeof_stream_item(0));
    BOOST_REQUIRE_EQUAL(0, dst2->output_signature()->max_streams());
}

#include <gnuradio/block_detail.h>
#include <gnuradio/blocks/nop.h>
#include <gnuradio/buffer.h>
#include <gnuradio/top_block.h>

BOOST_AUTO_TEST_CASE(t2)
{
    gr::block_sptr src1(gr::blocks::null_source::make(sizeof(int)));
    gr::block_sptr nop(gr::blocks::nop::make(sizeof(int)));
    gr::block_sptr dst1(gr::blocks::null_sink::make(sizeof(int)));

    gr::top_block_sptr tb(gr::make_top_block("t2"));
    tb->connect(src1, 0, nop, 0);
    tb->connect(nop, 0, dst1, 0);
    tb->start();

    const char* obuf = nop->detail()->output(0)->base();
    int obsize = nop->detail()->output(0)->bufsize();
    const char* ibuf = nop->detail()->input(0)->buffer()->base();
    int ibsize = nop->detail()->input(0)->buffer()->bufsize();

    BOOST_REQUIRE(obuf != NULL);
    BOOST_REQUIRE(ibuf != NULL);
    BOOST_REQUIRE(obsize > 0);
    BOOST_REQUIRE(ibsize > 0);

    tb->stop();
    tb->wait();
}

/* -*- c++ -*- */
/*
 * Copyright 2006,2008,2013,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnuradio/blocks/copy.h>
#include <gnuradio/blocks/head.h>
#include <gnuradio/blocks/null_sink.h>
#include <gnuradio/blocks/null_source.h>
#include <gnuradio/io_signature.h>
#include <gnuradio/top_block.h>
#include <boost/test/unit_test.hpp>

// Declare a test C++ hierarchical block

class gr_derived_block;
typedef std::shared_ptr<gr_derived_block> gr_derived_block_sptr;
gr_derived_block_sptr gr_make_derived_block();

class gr_derived_block : public gr::hier_block2
{
private:
    template <typename T, typename... Args>
    friend std::shared_ptr<T> gnuradio::make_block_sptr(Args&&... args);

    gr_derived_block();

public:
    ~gr_derived_block() override;
};


gr_derived_block_sptr gr_make_derived_block()
{
    return gnuradio::make_block_sptr<gr_derived_block>();
}

gr_derived_block::gr_derived_block()
    : gr::hier_block2("gr_derived_block",
                      gr::io_signature::make(1, 1, sizeof(int)), // Input signature
                      gr::io_signature::make(1, 1, sizeof(int))) // Output signature
{
    gr::block_sptr copy(gr::blocks::copy::make(sizeof(int)));

    connect(self(), 0, copy, 0);
    connect(copy, 0, self(), 0);
}

gr_derived_block::~gr_derived_block() {}

BOOST_AUTO_TEST_CASE(test_1)
{
    gr::top_block_sptr tb(gr::make_top_block("test"));

    gr::block_sptr src(gr::blocks::null_source::make(sizeof(int)));
    gr::block_sptr head(gr::blocks::head::make(sizeof(int), 1000));
    gr_derived_block_sptr blk(gr_make_derived_block());
    gr::block_sptr dst(gr::blocks::null_sink::make(sizeof(int)));

    tb->connect(src, 0, head, 0);
    tb->connect(head, 0, blk, 0);
    tb->connect(blk, 0, dst, 0);

    tb->run();
}

BOOST_AUTO_TEST_CASE(test_2)
{
    gr_derived_block_sptr blk(gr_make_derived_block());
    blk->lock();
    blk->unlock();
}

BOOST_AUTO_TEST_CASE(test_3)
{
    gr::top_block_sptr tb(gr::make_top_block("test"));

    gr::block_sptr src(gr::blocks::null_source::make(sizeof(int)));
    gr::block_sptr head(gr::blocks::head::make(sizeof(int), 1000));
    gr_derived_block_sptr blk(gr_make_derived_block());
    gr::block_sptr dst0(gr::blocks::null_sink::make(sizeof(int)));
    gr::block_sptr dst1(gr::blocks::null_sink::make(sizeof(int)));

    tb->connect(src, 0, head, 0);
    tb->connect(head, 0, blk, 0);
    tb->connect(blk, 0, dst0, 0);
    tb->connect(blk, 0, dst1, 0);

    tb->run();

    tb->disconnect(blk, 0, dst1, 0);
    blk->lock();
    blk->unlock();
    tb->run();
}

BOOST_AUTO_TEST_CASE(test_4)
{
    gr_derived_block_sptr blk(gr_make_derived_block());
    gr::hier_block2_sptr parent0(
        gr::make_hier_block2("parent0",
                             gr::io_signature::make(1, 1, sizeof(int)),
                             gr::io_signature::make(1, 1, sizeof(int))));
    gr::hier_block2_sptr parent1(
        gr::make_hier_block2("parent1",
                             gr::io_signature::make(1, 1, sizeof(int)),
                             gr::io_signature::make(1, 1, sizeof(int))));
    try {
        parent0->connect(parent0->self(), 0, blk, 0);
        parent1->connect(blk, 0, parent1->self(), 0);
        BOOST_TEST(false);
    } catch (std::runtime_error& e) {
        BOOST_TEST(true);
    }
}

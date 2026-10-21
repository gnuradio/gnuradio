/* -*- c++ -*- */
/*
 * Copyright 2026 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnuradio/buffer_double_mapped.h>
#include <gnuradio/io_signature.h>
#include <gnuradio/sync_block.h>
#include <gnuradio/top_block.h>
#include <boost/test/unit_test.hpp>
#include <new>
#include <stdexcept>

// Checks that a flowgraph refuses to start when one output port feeds inputs
// that use different buffer types, and still runs when they all match.

namespace {

// A custom buffer type that behaves like the default one but has its own type.
class custom_buffer : public gr::buffer_double_mapped
{
public:
    static gr::buffer_type type;

    static gr::buffer_sptr make_buffer(int nitems,
                                       size_t sizeof_item,
                                       uint64_t downstream_lcm_nitems,
                                       uint32_t downstream_max_out_mult,
                                       gr::block_sptr link,
                                       gr::block_sptr /* buf_owner */)
    {
        return gr::buffer_sptr(new custom_buffer(
            nitems, sizeof_item, downstream_lcm_nitems, downstream_max_out_mult, link));
    }

    gr::buffer_type get_buffer_type() const override { return type; }

private:
    custom_buffer(int nitems,
                  size_t sizeof_item,
                  uint64_t downstream_lcm_nitems,
                  uint32_t downstream_max_out_mult,
                  gr::block_sptr link)
        : buffer_double_mapped(nitems,
                               sizeof_item,
                               downstream_lcm_nitems,
                               downstream_max_out_mult,
                               link,
                               defer_alloc_t::defer_alloc)
    {
        if (!allocate_buffer(nitems))
            throw std::bad_alloc();
    }
};

gr::buffer_type custom_buffer::type(gr::buftype<custom_buffer, custom_buffer>{});

gr::buffer_type host = gr::buffer_double_mapped::type;

// Produces items until stopped, or finishes on its first call when done is set.
class test_source : public gr::sync_block
{
public:
    typedef std::shared_ptr<test_source> sptr;
    static sptr make(gr::buffer_type buftype, bool done = true)
    {
        return gnuradio::make_block_sptr<test_source>(buftype, done);
    }

    test_source(gr::buffer_type buftype, bool done)
        : sync_block("test_source",
                     gr::io_signature::make(0, 0, 0),
                     gr::io_signature::make(1, 1, sizeof(float), buftype)),
          d_done(done)
    {
    }

    int work(int noutput_items, gr_vector_const_void_star&, gr_vector_void_star&) override
    {
        return d_done ? WORK_DONE : noutput_items;
    }

private:
    const bool d_done;
};

class test_sink : public gr::sync_block
{
public:
    typedef std::shared_ptr<test_sink> sptr;
    static sptr make(gr::buffer_type buftype)
    {
        return gnuradio::make_block_sptr<test_sink>(buftype);
    }

    // replace_buffer() takes the buffer type from the consumer's output
    // signature, so declare an optional output of the same type.
    test_sink(gr::buffer_type buftype)
        : sync_block("test_sink",
                     gr::io_signature::make(1, 1, sizeof(float), buftype),
                     gr::io_signature::make(0, 1, sizeof(float), buftype))
    {
    }

    int work(int noutput_items, gr_vector_const_void_star&, gr_vector_void_star&) override
    {
        return noutput_items;
    }
};

// Connects src to one sink per buffer type, in the given order, and runs.
void run_fanout(gr::buffer_type src_type,
                gr::buffer_type sink0_type,
                gr::buffer_type sink1_type)
{
    auto tb = gr::make_top_block("fanout");
    auto src = test_source::make(src_type);
    tb->connect(src, 0, test_sink::make(sink0_type), 0);
    tb->connect(src, 0, test_sink::make(sink1_type), 0);
    tb->run();
}

} // namespace

BOOST_AUTO_TEST_CASE(t0_default_fanout_runs)
{
    BOOST_CHECK_NO_THROW(run_fanout(host, host, host));
}

// The source's default buffer is replaced once and shared by both consumers.
BOOST_AUTO_TEST_CASE(t1_same_custom_fanout_runs)
{
    BOOST_CHECK_NO_THROW(run_fanout(host, custom_buffer::type, custom_buffer::type));
}

BOOST_AUTO_TEST_CASE(t2_mixed_fanout_from_default_source_throws)
{
    BOOST_CHECK_THROW(run_fanout(host, host, custom_buffer::type), std::runtime_error);
    BOOST_CHECK_THROW(run_fanout(host, custom_buffer::type, host), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(t3_mixed_fanout_from_custom_source_throws)
{
    auto custom = custom_buffer::type;
    BOOST_CHECK_THROW(run_fanout(custom, custom, host), std::runtime_error);
    BOOST_CHECK_THROW(run_fanout(custom, host, custom), std::runtime_error);
}

// A mismatched consumer added while running is caught when the flowgraph
// restarts on unlock().
BOOST_AUTO_TEST_CASE(t4_mixed_fanout_on_reconfigure_throws)
{
    auto tb = gr::make_top_block("fanout");
    auto src = test_source::make(host, false);
    tb->connect(src, 0, test_sink::make(host), 0);
    tb->start();

    tb->lock();
    tb->connect(src, 0, test_sink::make(custom_buffer::type), 0);
    BOOST_CHECK_THROW(tb->unlock(), std::runtime_error);

    tb->stop();
    tb->wait();
}

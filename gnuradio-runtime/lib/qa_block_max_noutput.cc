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

#include <gnuradio/block.h>
#include <gnuradio/block_detail.h>
#include <gnuradio/buffer_double_mapped.h>
#include <gnuradio/io_signature.h>
#include <gnuradio/sync_block.h>
#include <gnuradio/top_block.h>
#include <boost/test/unit_test.hpp>
#include <algorithm>
#include <new>

// Checks how block sets max_noutput_items() from its output buffers: the value
// follows the allocated bufsize() rather than the requested size, leaves the
// scheduler's top_block maximum in effect, and preserves explicit limits from
// set_max_noutput_items() and max_output_buffer.

namespace {

// A custom buffer type that allocates more than it was asked for, as buffer
// types that enforce a minimum size do.
class oversized_buffer : public gr::buffer_double_mapped
{
public:
    // Large enough that bufsize() clearly exceeds the default request.
    static constexpr int growth = 8;
    static gr::buffer_type type;

    static gr::buffer_sptr make_buffer(int nitems,
                                       size_t sizeof_item,
                                       uint64_t downstream_lcm_nitems,
                                       uint32_t downstream_max_out_mult,
                                       gr::block_sptr link,
                                       gr::block_sptr /* buf_owner */)
    {
        return gr::buffer_sptr(new oversized_buffer(
            nitems, sizeof_item, downstream_lcm_nitems, downstream_max_out_mult, link));
    }

    gr::buffer_type get_buffer_type() const override { return type; }

private:
    // The deferred-allocation base constructor skips the default allocation,
    // so the size below is the only one made.
    oversized_buffer(int nitems,
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
        if (!allocate_buffer(nitems * growth))
            throw std::bad_alloc();
    }
};

gr::buffer_type oversized_buffer::type(gr::buftype<oversized_buffer, oversized_buffer>{});

// Finishes on its first work() call. The tests only inspect what flowgraph
// setup assigned, so run() just needs to build the buffers and return.
class done_source : public gr::sync_block
{
public:
    typedef std::shared_ptr<done_source> sptr;
    static sptr make(gr::buffer_type buftype)
    {
        return gnuradio::make_block_sptr<done_source>(buftype);
    }

    done_source(gr::buffer_type buftype)
        : sync_block("done_source",
                     gr::io_signature::make(0, 0, 0),
                     gr::io_signature::make(1, 1, sizeof(float), buftype))
    {
    }

    int work(int, gr_vector_const_void_star&, gr_vector_void_star&) override
    {
        return WORK_DONE;
    }
};

// Consumer whose input buffer type decides whether the upstream output buffer
// is kept or replaced.
class drain_sink : public gr::sync_block
{
public:
    typedef std::shared_ptr<drain_sink> sptr;
    static sptr make(gr::buffer_type buftype)
    {
        return gnuradio::make_block_sptr<drain_sink>(buftype);
    }

    // replace_buffer() takes the buffer type from the consumer's output
    // signature, so declare an optional output of the same type.
    drain_sink(gr::buffer_type buftype)
        : sync_block("drain_sink",
                     gr::io_signature::make(1, 1, sizeof(float), buftype),
                     gr::io_signature::make(0, 1, sizeof(float), buftype))
    {
    }

    int work(int noutput_items, gr_vector_const_void_star&, gr_vector_void_star&) override
    {
        return noutput_items;
    }
};

// Like done_source, with a separately chosen buffer type on each output port.
class two_output_source : public gr::sync_block
{
public:
    typedef std::shared_ptr<two_output_source> sptr;
    static sptr make(gr::buffer_type buftype0, gr::buffer_type buftype1)
    {
        return gnuradio::make_block_sptr<two_output_source>(buftype0, buftype1);
    }

    two_output_source(gr::buffer_type buftype0, gr::buffer_type buftype1)
        : sync_block("two_output_source",
                     gr::io_signature::make(0, 0, 0),
                     gr::io_signature::make2(
                         2, 2, sizeof(float), sizeof(float), buftype0, buftype1))
    {
    }

    int work(int, gr_vector_const_void_star&, gr_vector_void_star&) override
    {
        return WORK_DONE;
    }
};

// Returns the source's output buffer as it is after setup, which is the
// replacement if replace_buffer() swapped it.
gr::buffer_sptr run_and_get_output(done_source::sptr src, drain_sink::sptr snk)
{
    auto tb = gr::make_top_block("max_noutput");
    tb->connect(src, 0, snk, 0);
    tb->run();
    return src->detail()->output(0);
}

void run_two_outputs(two_output_source::sptr src,
                     drain_sink::sptr snk0,
                     drain_sink::sptr snk1)
{
    auto tb = gr::make_top_block("max_noutput_two");
    tb->connect(src, 0, snk0, 0);
    tb->connect(src, 1, snk1, 0);
    tb->run();
}

// The default (non-custom) buffer type.
const gr::buffer_type& host = gr::buffer_double_mapped::type;

} // namespace

// With default buffers the value is the allocated bufsize() and no
// block-specific limit is set.
BOOST_AUTO_TEST_CASE(t0_default_buffer_unchanged)
{
    auto src = done_source::make(gr::buffer_double_mapped::type);
    auto snk = drain_sink::make(gr::buffer_double_mapped::type);

    auto buf = run_and_get_output(src, snk);

    BOOST_CHECK_EQUAL(src->max_noutput_items(), static_cast<int>(buf->bufsize()));
    BOOST_CHECK(!src->is_set_max_noutput_items());
}

// A buffer that allocates more than requested sets the value to what it
// actually allocated.
BOOST_AUTO_TEST_CASE(t1_allocated_buffer_larger_than_request)
{
    auto src = done_source::make(oversized_buffer::type);
    auto snk = drain_sink::make(gr::buffer_double_mapped::type);

    auto buf = run_and_get_output(src, snk);

    BOOST_REQUIRE(buf->get_buffer_type() == oversized_buffer::type);
    // Larger than the default request of 2 * 32 KiB worth of floats.
    BOOST_CHECK_GT(buf->bufsize(), 2u * 32768 / sizeof(float));
    BOOST_CHECK_EQUAL(src->max_noutput_items(), static_cast<int>(buf->bufsize()));
    BOOST_CHECK(!src->is_set_max_noutput_items());
}

// The consumer's buffer type replaces the source's default buffer after it was
// allocated; the value grows to the replacement's bufsize().
BOOST_AUTO_TEST_CASE(t2_replace_buffer_grows)
{
    auto src = done_source::make(gr::buffer_double_mapped::type);
    auto snk = drain_sink::make(oversized_buffer::type);

    auto buf = run_and_get_output(src, snk);

    BOOST_REQUIRE(buf->get_buffer_type() == oversized_buffer::type);
    BOOST_CHECK_EQUAL(src->max_noutput_items(), static_cast<int>(buf->bufsize()));
    BOOST_CHECK(!src->is_set_max_noutput_items());
}

// An explicit set_max_noutput_items() is kept even when the buffer is larger.
BOOST_AUTO_TEST_CASE(t3_user_value_survives_allocate)
{
    auto src = done_source::make(oversized_buffer::type);
    auto snk = drain_sink::make(gr::buffer_double_mapped::type);
    src->set_max_noutput_items(1024);

    run_and_get_output(src, snk);

    BOOST_CHECK_EQUAL(src->max_noutput_items(), 1024);
    BOOST_CHECK(src->is_set_max_noutput_items());
}

// An explicit set_max_noutput_items() is kept across buffer replacement.
BOOST_AUTO_TEST_CASE(t4_user_value_survives_replace)
{
    auto src = done_source::make(gr::buffer_double_mapped::type);
    auto snk = drain_sink::make(oversized_buffer::type);
    src->set_max_noutput_items(1024);

    auto buf = run_and_get_output(src, snk);

    BOOST_REQUIRE(buf->get_buffer_type() == oversized_buffer::type);
    BOOST_CHECK_EQUAL(src->max_noutput_items(), 1024);
    BOOST_CHECK(src->is_set_max_noutput_items());
}

// A max_output_buffer limits the block to the requested size, not the
// rounded-up bufsize(), and counts as a block-specific limit.
BOOST_AUTO_TEST_CASE(t5_max_output_buffer_caps_below_bufsize)
{
    auto src = done_source::make(gr::buffer_double_mapped::type);
    auto snk = drain_sink::make(gr::buffer_double_mapped::type);
    src->set_max_output_buffer(100);

    auto buf = run_and_get_output(src, snk);

    // The allocation rounds 100 items up to the page granularity.
    BOOST_REQUIRE_GT(buf->bufsize(), 100u);
    BOOST_CHECK_EQUAL(src->max_noutput_items(), 100);
    BOOST_CHECK(src->is_set_max_noutput_items());
}

// With differently sized output buffers the value is the smallest bufsize(),
// whichever port it is on.
BOOST_AUTO_TEST_CASE(t6_multi_output_takes_smallest_bufsize)
{
    for (bool oversized_first : { true, false }) {
        BOOST_TEST_CONTEXT("oversized_first=" << oversized_first)
        {
            auto src = oversized_first
                           ? two_output_source::make(oversized_buffer::type, host)
                           : two_output_source::make(host, oversized_buffer::type);
            run_two_outputs(src, drain_sink::make(host), drain_sink::make(host));

            auto b0 = src->detail()->output(0)->bufsize();
            auto b1 = src->detail()->output(1)->bufsize();
            BOOST_REQUIRE_NE(b0, b1);
            BOOST_CHECK_EQUAL(src->max_noutput_items(),
                              static_cast<int>(std::min(b0, b1)));
            BOOST_CHECK(!src->is_set_max_noutput_items());
        }
    }
}

// Replacing one port's buffer with a larger one does not raise the value past
// the other port's bufsize().
BOOST_AUTO_TEST_CASE(t7_multi_output_replace_does_not_exceed_other_port)
{
    auto src = two_output_source::make(host, host);
    // Only port 0's consumer takes the oversized type, so only port 0 is
    // replaced.
    run_two_outputs(
        src, drain_sink::make(oversized_buffer::type), drain_sink::make(host));

    auto replaced = src->detail()->output(0);
    auto other = src->detail()->output(1);
    BOOST_REQUIRE(replaced->get_buffer_type() == oversized_buffer::type);
    BOOST_REQUIRE_GT(replaced->bufsize(), other->bufsize());
    BOOST_CHECK_EQUAL(src->max_noutput_items(), static_cast<int>(other->bufsize()));
    BOOST_CHECK(!src->is_set_max_noutput_items());
}

// With max_output_buffer on several ports the smallest one wins, regardless of
// which port is allocated first.
BOOST_AUTO_TEST_CASE(t8_multi_output_max_output_buffer_takes_minimum)
{
    for (bool small_first : { true, false }) {
        BOOST_TEST_CONTEXT("small_first=" << small_first)
        {
            auto src = two_output_source::make(host, host);
            src->set_max_output_buffer(0, small_first ? 100 : 1000);
            src->set_max_output_buffer(1, small_first ? 1000 : 100);
            run_two_outputs(src, drain_sink::make(host), drain_sink::make(host));

            BOOST_CHECK_EQUAL(src->max_noutput_items(), 100);
            BOOST_CHECK(src->is_set_max_noutput_items());
        }
    }
}

// A max_output_buffer on one port limits the whole block, even though the
// other, uncapped port's bufsize() is larger.
BOOST_AUTO_TEST_CASE(t9_multi_output_one_capped_port)
{
    auto src = two_output_source::make(host, oversized_buffer::type);
    src->set_max_output_buffer(1, 100);
    run_two_outputs(src, drain_sink::make(host), drain_sink::make(host));

    BOOST_CHECK_EQUAL(src->max_noutput_items(), 100);
    BOOST_CHECK(src->is_set_max_noutput_items());
}

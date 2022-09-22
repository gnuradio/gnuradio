/* -*- c++ -*- */
/*
 * Copyright 2021 BlackLynx Inc.
 * Copyright 2022 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <gtest/gtest.h>

#include <gnuradio/block.h>
#include <gnuradio/buffer_cpu_host.h>

#include <cstdlib>
#include <iostream>


using namespace gr;

// ----------------------------------------------------------------------------
// Basic checks for buffer_single_mapped using the buffer_cpu_host implementation
// of the interface for testing.
// ----------------------------------------------------------------------------
TEST(HostBuffer, t0)
{
    size_t nitems = 65536 / sizeof(int);

    auto buf_props = BUFFER_CPU_HOST_ARGS_H2D;
    auto buf = buf_props->factory()(nitems, 1, buf_props);
    auto rdr1 = buf->add_reader(buf_props, 1);

    EXPECT_TRUE(buf->space_available() == nitems);
    EXPECT_TRUE(rdr1->items_available() == 0);

    for (size_t idx = 1; idx <= 16; ++idx) {
        buf->post_write(1000);
        EXPECT_TRUE(buf->space_available() == (nitems - (idx * 1000)));

        EXPECT_TRUE(rdr1->items_available() == (idx * 1000));
    }

    EXPECT_TRUE(buf->space_available() == 384);

    buf->post_write(buf->space_available());
    EXPECT_TRUE(buf->space_available() == 0);
    EXPECT_TRUE(rdr1->items_available() == nitems);
    EXPECT_TRUE(buf->space_available() == 0);
}

// ----------------------------------------------------------------------------
// Basic checks for buffer_single_mapped using the buffer_cpu_host implementation
// of the interface for testing.
// ----------------------------------------------------------------------------
TEST(HostBuffer, t1)
{
    size_t nitems = 65536 / sizeof(int);

    auto buf_props = BUFFER_CPU_HOST_ARGS_H2D;
    auto buf = buf_props->factory()(nitems, 1, buf_props);
    auto rdr1 = buf->add_reader(buf_props, 1);

    size_t space = buf->space_available();
    EXPECT_TRUE(nitems == space);

    EXPECT_TRUE(rdr1->items_available() == 0);

    buf->post_write(nitems);
    EXPECT_TRUE(buf->space_available() == 0);
    EXPECT_TRUE(rdr1->items_available() == nitems);

    for (size_t idx = 1; idx <= 16; ++idx) {
        rdr1->post_read(1000);
        EXPECT_TRUE(rdr1->items_available() == (nitems - (idx * 1000)));

        space = buf->space_available();
        EXPECT_TRUE(space == (idx * 1000));
    }

    EXPECT_TRUE(rdr1->items_available() == 384);
    rdr1->post_read(384);
    EXPECT_TRUE(rdr1->items_available() == 0);
}

// ----------------------------------------------------------------------------
// Basic check reader/write wrapping of buffer_single_mapped with 1 reader.
// ----------------------------------------------------------------------------
TEST(HostBuffer, t2)
{
    size_t nitems = 65536 / sizeof(int);

    auto buf_props = BUFFER_CPU_HOST_ARGS_H2D;
    auto buf = buf_props->factory()(nitems, 1, buf_props);
    auto rdr1 = buf->add_reader(buf_props, 1);

    size_t space = buf->space_available();
    EXPECT_TRUE(nitems == space);
    EXPECT_TRUE(rdr1->items_available() == 0);

    buf->post_write(nitems);
    EXPECT_TRUE(buf->space_available() == 0);

    for (size_t idx = 1; idx <= 16; ++idx) {
        rdr1->post_read(1000);
        EXPECT_TRUE(rdr1->items_available() == (nitems - (idx * 1000)));

        space = buf->space_available();

        if (idx <= 9)
            EXPECT_TRUE(space == (idx * 1000));
        else
            EXPECT_TRUE(space == ((idx * 1000) - (nitems / 2)));

        if (idx == 9) {
            buf->post_write(nitems / 2);
        }
    }

    // At this point we can only read up until the end of the buffer even though
    // additional data is available at the beginning of the buffer
    EXPECT_TRUE(rdr1->items_available() == 384);
    rdr1->post_read(384);

    // Now the (nitems / 2) at the beginning of the buffer should be available
    EXPECT_TRUE(rdr1->items_available() == (nitems / 2));

    for (size_t idx = 0; idx < 4; ++idx)
        rdr1->post_read(1024);

    EXPECT_TRUE(buf->space_available() == (nitems / 2));
    EXPECT_TRUE(rdr1->items_available() == (nitems / 4));

    for (size_t idx = 0; idx < 4; ++idx)
        rdr1->post_read(1000);

    EXPECT_TRUE(buf->space_available() == (nitems / 2));
    EXPECT_TRUE(rdr1->items_available() == 96);

    rdr1->post_read(96);
    EXPECT_TRUE(rdr1->items_available() == 0);

    EXPECT_TRUE(buf->space_available() == (nitems / 2));
}

// ----------------------------------------------------------------------------
// Basic check reader/write wrapping of buffer_single_mapped with 2 readers.
// ----------------------------------------------------------------------------
TEST(HostBuffer, t3)
{
    size_t nitems = 65536 / sizeof(int);

    auto buf_props = BUFFER_CPU_HOST_ARGS_H2D;
    auto buf = buf_props->factory()(nitems, 1, buf_props);
    auto rdr1 = buf->add_reader(buf_props, 1);
    auto rdr2 = buf->add_reader(buf_props, 1);

    size_t space = buf->space_available();
    EXPECT_TRUE(nitems == space);
    EXPECT_TRUE(rdr1->items_available() == 0);
    EXPECT_TRUE(rdr2->items_available() == 0);

    buf->post_write(nitems);
    EXPECT_TRUE(buf->space_available() == 0);
    EXPECT_TRUE(rdr1->items_available() == nitems);
    EXPECT_TRUE(rdr2->items_available() == nitems);

    for (size_t idx = 1; idx <= 16; ++idx) {
        rdr1->post_read(1000);
        EXPECT_TRUE(rdr1->items_available() == (nitems - (idx * 1000)));

        // Reader 2 hasn't read anything so space available should remain 0
        EXPECT_TRUE(buf->space_available() == 0);
    }

    size_t last_rdr1_available = rdr1->items_available();
    size_t increment = last_rdr1_available / 4;

    for (size_t idx = 1; idx <= 16; ++idx) {
        rdr2->post_read(1000);
        EXPECT_TRUE(rdr2->items_available() == (nitems - (idx * 1000)));

        EXPECT_TRUE(rdr1->items_available() == last_rdr1_available);
        if (idx % 4 == 0) {
            rdr1->post_read(increment);
            EXPECT_TRUE(rdr1->items_available() == (last_rdr1_available - increment));
            last_rdr1_available = rdr1->items_available();
        }

        EXPECT_TRUE(buf->space_available() == (idx * 1000));
    }
}

// ----------------------------------------------------------------------------
// Basic check of output blocked callback
// ----------------------------------------------------------------------------
TEST(HostBuffer, t4)
{
    size_t nitems = 65536 / sizeof(int);

    auto buf_props = BUFFER_CPU_HOST_ARGS_H2D;
    auto buf = buf_props->factory()(nitems, 1, buf_props);
    auto rdr1 = buf->add_reader(buf_props, 1);

    EXPECT_TRUE(nitems == buf->space_available());
    EXPECT_TRUE(rdr1->items_available() == 0);

    buf->post_write(nitems / 2);
    EXPECT_TRUE(buf->space_available() == (nitems / 2));
    EXPECT_TRUE(rdr1->items_available() == (nitems / 2));

    rdr1->post_read(nitems / 2);
    EXPECT_TRUE(buf->space_available() == (nitems / 2));
    EXPECT_TRUE(rdr1->items_available() == 0);

    buf->post_write(8000);
    EXPECT_TRUE(buf->space_available() == 192);

    // bool ready = buf->output_blkd_cb_ready(200);
    // EXPECT_TRUE(ready == true);

    bool success = buf->output_blocked_callback(200);
    EXPECT_TRUE(success == true);
    EXPECT_TRUE(buf->space_available() == 8384);
    EXPECT_TRUE(rdr1->items_available() == 8000);

    rdr1->post_read(4000);
    EXPECT_TRUE(buf->space_available() == 8384);
    EXPECT_TRUE(rdr1->items_available() == 4000);

    buf->post_write(4000);
    EXPECT_TRUE(buf->space_available() == 4384);
    EXPECT_TRUE(rdr1->items_available() == 8000);

    rdr1->post_read(8000);
    EXPECT_TRUE(buf->space_available() == 4384);
    EXPECT_TRUE(rdr1->items_available() == 0);
}

// ----------------------------------------------------------------------------
// Basic check of input blocked callback
// ----------------------------------------------------------------------------
TEST(HostBuffer, t5)
{
    size_t nitems = 65536 / sizeof(int);

    auto buf_props = BUFFER_CPU_HOST_ARGS_H2D;
    auto buf = buf_props->factory()(nitems, 1, buf_props);
    auto rdr1 = buf->add_reader(buf_props, 1);

    EXPECT_TRUE(nitems == buf->space_available());
    EXPECT_TRUE(rdr1->items_available() == 0);

    buf->post_write(16000);
    EXPECT_TRUE(buf->space_available() == 384);
    EXPECT_TRUE(rdr1->items_available() == 16000);

    rdr1->post_read(16000);
    EXPECT_TRUE(buf->space_available() == 384);
    EXPECT_TRUE(rdr1->items_available() == 0);

    buf->post_write(384);
    EXPECT_TRUE(buf->space_available() == 16000);
    EXPECT_TRUE(rdr1->items_available() == 384);

    buf->post_write(116);
    EXPECT_TRUE(buf->space_available() == 15884);
    EXPECT_TRUE(rdr1->items_available() == 384);

    // bool ready = rdr1->input_blkd_cb_ready(400);
    // EXPECT_TRUE(ready == true);

    bool success = rdr1->input_blocked_callback(400);
    EXPECT_TRUE(success == true);
    EXPECT_TRUE(rdr1->items_available() == 500);

    rdr1->post_read(500);
    EXPECT_TRUE(buf->space_available() == 15884);
    EXPECT_TRUE(rdr1->items_available() == 0);
}

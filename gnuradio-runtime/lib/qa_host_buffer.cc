/* -*- c++ -*- */
/*
 * Copyright 2021 BlackLynx Inc.
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
#include <gnuradio/buffer_reader.h>
#include <gnuradio/host_buffer.h>
#include <gnuradio/random.h>
#include <gnuradio/transfer_type.h>
#include <boost/test/unit_test.hpp>
#include <cstdlib>
#include <iostream>

// This is a trivial mocked up block inspired by gr::blocks::nop that is used
// only as a placeholder for testing host_buffer below.
class nop : public gr::block
{
public:
    typedef std::shared_ptr<nop> sptr;
    static sptr make(size_t sizeof_stream_item)
    {
        return gnuradio::make_block_sptr<nop>(sizeof_stream_item);
    }

    nop(size_t sizeof_stream_item)
        : block("nop",
                gr::io_signature::make(0, -1, sizeof_stream_item),
                gr::io_signature::make(0, -1, sizeof_stream_item))
    {
    }

    ~nop() override {}

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override
    {
        // eat any input that's available
        for (unsigned i = 0; i < ninput_items.size(); i++)
            consume(i, ninput_items[i]);

        return noutput_items;
    }
};


// ----------------------------------------------------------------------------
// Basic checks for buffer_single_mapped using the host_buffer implementation
// of the interface for testing.
// ----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(t0)
{
    int nitems = 65536 / sizeof(int);
    gr::block_sptr nop(nop::make(sizeof(int))); // used as placeholder

    gr::buffer_sptr buf(
        gr::host_buffer::make_host_buffer(nitems, sizeof(int), nitems, 1, nop, nop));
    buf->set_transfer_type(gr::transfer_type::HOST_TO_HOST);

    gr::buffer_reader_sptr rdr1(gr::buffer_add_reader(buf, 0, nop));

    BOOST_CHECK(buf->space_available() == nitems);
    BOOST_CHECK(rdr1->items_available() == 0);

    for (int idx = 1; idx <= 16; ++idx) {
        buf->update_write_pointer(1000);
        BOOST_CHECK(buf->space_available() == (nitems - (idx * 1000)));

        BOOST_CHECK(rdr1->items_available() == (idx * 1000));
    }

    BOOST_CHECK(buf->space_available() == 384);

    buf->update_write_pointer(buf->space_available());
    BOOST_CHECK(buf->space_available() == 0);
    BOOST_CHECK(rdr1->items_available() == nitems);
    BOOST_CHECK(buf->space_available() == 0);
}

// ----------------------------------------------------------------------------
// Basic checks for buffer_single_mapped using the host_buffer implementation
// of the interface for testing.
// ----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(t1)
{
    int nitems = 65536 / sizeof(int);
    gr::block_sptr nop(nop::make(sizeof(int))); // used as placeholder

    gr::buffer_sptr buf(
        gr::host_buffer::make_host_buffer(nitems, sizeof(int), nitems, 1, nop, nop));
    buf->set_transfer_type(gr::transfer_type::HOST_TO_HOST);

    gr::buffer_reader_sptr rdr1(gr::buffer_add_reader(buf, 0, nop));

    int space = buf->space_available();
    BOOST_CHECK(nitems == space);

    BOOST_CHECK(rdr1->items_available() == 0);

    buf->update_write_pointer(nitems);
    BOOST_CHECK(buf->space_available() == 0);
    BOOST_CHECK(rdr1->items_available() == nitems);

    for (int idx = 1; idx <= 16; ++idx) {
        rdr1->update_read_pointer(1000);
        BOOST_CHECK(rdr1->items_available() == (nitems - (idx * 1000)));

        space = buf->space_available();
        BOOST_CHECK(space == (idx * 1000));
    }

    BOOST_CHECK(rdr1->items_available() == 384);
    rdr1->update_read_pointer(384);
    BOOST_CHECK(rdr1->items_available() == 0);
}

// ----------------------------------------------------------------------------
// Basic check reader/write wrapping of buffer_single_mapped with 1 reader.
// ----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(t2)
{
    int nitems = 65536 / sizeof(int);
    gr::block_sptr nop(nop::make(sizeof(int))); // used as placeholder

    gr::buffer_sptr buf(
        gr::host_buffer::make_host_buffer(nitems, sizeof(int), nitems, 1, nop, nop));
    buf->set_transfer_type(gr::transfer_type::HOST_TO_HOST);

    gr::buffer_reader_sptr rdr1(gr::buffer_add_reader(buf, 0, nop));

    int space = buf->space_available();
    BOOST_CHECK(nitems == space);
    BOOST_CHECK(rdr1->items_available() == 0);

    buf->update_write_pointer(nitems);
    BOOST_CHECK(buf->space_available() == 0);

    for (int idx = 1; idx <= 16; ++idx) {
        rdr1->update_read_pointer(1000);
        BOOST_CHECK(rdr1->items_available() == (nitems - (idx * 1000)));

        space = buf->space_available();

        if (idx <= 9)
            BOOST_CHECK(space == (idx * 1000));
        else
            BOOST_CHECK(space == ((idx * 1000) - (nitems / 2)));

        if (idx == 9) {
            buf->update_write_pointer(nitems / 2);
        }
    }

    // At this point we can only read up until the end of the buffer even though
    // additional data is available at the beginning of the buffer
    BOOST_CHECK(rdr1->items_available() == 384);
    rdr1->update_read_pointer(384);

    // Now the (nitems / 2) at the beginning of the buffer should be available
    BOOST_CHECK(rdr1->items_available() == (nitems / 2));

    for (int idx = 0; idx < 4; ++idx)
        rdr1->update_read_pointer(1024);

    BOOST_CHECK(buf->space_available() == (nitems / 2));
    BOOST_CHECK(rdr1->items_available() == (nitems / 4));

    for (int idx = 0; idx < 4; ++idx)
        rdr1->update_read_pointer(1000);

    BOOST_CHECK(buf->space_available() == (nitems / 2));
    BOOST_CHECK(rdr1->items_available() == 96);

    rdr1->update_read_pointer(96);
    BOOST_CHECK(rdr1->items_available() == 0);

    BOOST_CHECK(buf->space_available() == (nitems / 2));
}

// ----------------------------------------------------------------------------
// Basic check reader/write wrapping of buffer_single_mapped with 2 readers.
// ----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(t3)
{
    int nitems = 65536 / sizeof(int);
    gr::block_sptr nop(nop::make(sizeof(int))); // used as placeholder

    gr::buffer_sptr buf(
        gr::host_buffer::make_host_buffer(nitems, sizeof(int), nitems, 1, nop, nop));
    buf->set_transfer_type(gr::transfer_type::HOST_TO_HOST);

    gr::buffer_reader_sptr rdr1(gr::buffer_add_reader(buf, 0, nop));
    gr::buffer_reader_sptr rdr2(gr::buffer_add_reader(buf, 0, nop));

    int space = buf->space_available();
    BOOST_CHECK(nitems == space);
    BOOST_CHECK(rdr1->items_available() == 0);
    BOOST_CHECK(rdr2->items_available() == 0);

    buf->update_write_pointer(nitems);
    BOOST_CHECK(buf->space_available() == 0);
    BOOST_CHECK(rdr1->items_available() == nitems);
    BOOST_CHECK(rdr2->items_available() == nitems);

    for (int idx = 1; idx <= 16; ++idx) {
        rdr1->update_read_pointer(1000);
        BOOST_CHECK(rdr1->items_available() == (nitems - (idx * 1000)));

        // Reader 2 hasn't read anything so space available should remain 0
        BOOST_CHECK(buf->space_available() == 0);
    }

    int last_rdr1_available = rdr1->items_available();
    int increment = last_rdr1_available / 4;

    for (int idx = 1; idx <= 16; ++idx) {
        rdr2->update_read_pointer(1000);
        BOOST_CHECK(rdr2->items_available() == (nitems - (idx * 1000)));

        BOOST_CHECK(rdr1->items_available() == last_rdr1_available);
        if (idx % 4 == 0) {
            rdr1->update_read_pointer(increment);
            BOOST_CHECK(rdr1->items_available() == (last_rdr1_available - increment));
            last_rdr1_available = rdr1->items_available();
        }

        BOOST_CHECK(buf->space_available() == (idx * 1000));
    }
}

// ----------------------------------------------------------------------------
// Basic check of output blocked callback
// ----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(t4)
{
    int nitems = 65536 / sizeof(int);
    gr::block_sptr nop(nop::make(sizeof(int))); // used as placeholder

    gr::buffer_sptr buf(
        gr::host_buffer::make_host_buffer(nitems, sizeof(int), nitems, 1, nop, nop));
    buf->set_transfer_type(gr::transfer_type::HOST_TO_HOST);

    gr::buffer_reader_sptr rdr1(gr::buffer_add_reader(buf, 0, nop));

    BOOST_CHECK(nitems == buf->space_available());
    BOOST_CHECK(rdr1->items_available() == 0);

    buf->update_write_pointer(nitems / 2);
    BOOST_CHECK(buf->space_available() == (nitems / 2));
    BOOST_CHECK(rdr1->items_available() == (nitems / 2));

    rdr1->update_read_pointer(nitems / 2);
    BOOST_CHECK(buf->space_available() == (nitems / 2));
    BOOST_CHECK(rdr1->items_available() == 0);

    buf->update_write_pointer(8000);
    BOOST_CHECK(buf->space_available() == 192);

    bool ready = buf->output_blkd_cb_ready(200);
    BOOST_CHECK(ready == true);

    bool success = buf->output_blocked_callback(200);
    BOOST_CHECK(success == true);
    BOOST_CHECK(buf->space_available() == 8384);
    BOOST_CHECK(rdr1->items_available() == 8000);

    rdr1->update_read_pointer(4000);
    BOOST_CHECK(buf->space_available() == 8384);
    BOOST_CHECK(rdr1->items_available() == 4000);

    buf->update_write_pointer(4000);
    BOOST_CHECK(buf->space_available() == 4384);
    BOOST_CHECK(rdr1->items_available() == 8000);

    rdr1->update_read_pointer(8000);
    BOOST_CHECK(buf->space_available() == 4384);
    BOOST_CHECK(rdr1->items_available() == 0);
}

// ----------------------------------------------------------------------------
// Basic check of input blocked callback
// ----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(t5)
{
    int nitems = 65536 / sizeof(int);
    gr::block_sptr nop(nop::make(sizeof(int))); // used as placeholder

    gr::buffer_sptr buf(
        gr::host_buffer::make_host_buffer(nitems, sizeof(int), nitems, 1, nop, nop));
    buf->set_transfer_type(gr::transfer_type::HOST_TO_HOST);

    gr::buffer_reader_sptr rdr1(gr::buffer_add_reader(buf, 0, nop));

    BOOST_CHECK(nitems == buf->space_available());
    BOOST_CHECK(rdr1->items_available() == 0);

    buf->update_write_pointer(16000);
    BOOST_CHECK(buf->space_available() == 384);
    BOOST_CHECK(rdr1->items_available() == 16000);

    rdr1->update_read_pointer(16000);
    BOOST_CHECK(buf->space_available() == 384);
    BOOST_CHECK(rdr1->items_available() == 0);

    buf->update_write_pointer(384);
    BOOST_CHECK(buf->space_available() == 16000);
    BOOST_CHECK(rdr1->items_available() == 384);

    buf->update_write_pointer(116);
    BOOST_CHECK(buf->space_available() == 15884);
    BOOST_CHECK(rdr1->items_available() == 384);

    bool ready = rdr1->input_blkd_cb_ready(400);
    BOOST_CHECK(ready == true);

    bool success = rdr1->input_blocked_callback(400, rdr1->items_available());
    BOOST_CHECK(success == true);
    BOOST_CHECK(rdr1->items_available() == 500);

    rdr1->update_read_pointer(500);
    BOOST_CHECK(buf->space_available() == 15884);
    BOOST_CHECK(rdr1->items_available() == 0);
}

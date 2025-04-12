/* -*- c++ -*- */
/*
 * Copyright 2004,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <gnuradio/buffer.h>
#include <gnuradio/buffer_double_mapped.h>
#include <gnuradio/buffer_reader.h>
#include <gnuradio/random.h>
#include <boost/test/unit_test.hpp>
#include <cstdlib>


static void leak_check(void f())
{
    long buffer_count = gr::buffer_ncurrently_allocated();
    long buffer_reader_count = gr::buffer_reader_ncurrently_allocated();

    f();

    BOOST_CHECK_EQUAL(buffer_reader_count, gr::buffer_reader_ncurrently_allocated());
    BOOST_CHECK_EQUAL(buffer_count, gr::buffer_ncurrently_allocated());
}


// ----------------------------------------------------------------------------
// test single writer, no readers...
//

static void t0_body()
{
    int nitems = 4000 / sizeof(int);
    int counter = 0;

    gr::buffer_sptr buf(gr::buffer_double_mapped::make_buffer(
        nitems, sizeof(int), nitems, 1, gr::block_sptr()));

    int last_sa;
    int sa;

    sa = buf->space_available();
    BOOST_CHECK(sa > 0);
    last_sa = sa;

    for (int i = 0; i < 5; i++) {
        sa = buf->space_available();
        BOOST_CHECK_EQUAL(last_sa, sa);
        last_sa = sa;

        int* p = (int*)buf->write_pointer();
        BOOST_CHECK(p != 0);

        for (int j = 0; j < sa; j++)
            *p++ = counter++;

        buf->update_write_pointer(sa);
    }
}

// ----------------------------------------------------------------------------
// test single writer, single reader
//

static void t1_body()
{
    int nitems = 4000 / sizeof(int);
    int write_counter = 0;
    int read_counter = 0;

    gr::buffer_sptr buf(gr::buffer_double_mapped::make_buffer(
        nitems, sizeof(int), nitems, 1, gr::block_sptr()));
    gr::buffer_reader_sptr r1(gr::buffer_add_reader(buf, 0, gr::block_sptr()));

    int sa;

    // write 1/3 of buffer

    sa = buf->space_available();
    BOOST_CHECK(sa > 0);

    int* p = (int*)buf->write_pointer();
    BOOST_CHECK(p != 0);

    for (int j = 0; j < sa / 3; j++) {
        *p++ = write_counter++;
    }
    buf->update_write_pointer(sa / 3);

    // write the next 1/3 (1/2 of what's left)

    sa = buf->space_available();
    BOOST_CHECK(sa > 0);

    p = (int*)buf->write_pointer();
    BOOST_CHECK(p != 0);

    for (int j = 0; j < sa / 2; j++) {
        *p++ = write_counter++;
    }
    buf->update_write_pointer(sa / 2);

    // check that we can read it OK

    int ia = r1->items_available();
    BOOST_CHECK_EQUAL(write_counter, ia);

    const int* rp = (const int*)r1->read_pointer();
    BOOST_CHECK(rp != 0);

    for (int i = 0; i < ia / 2; i++) {
        BOOST_CHECK_EQUAL(read_counter, *rp);
        read_counter++;
        rp++;
    }
    r1->update_read_pointer(ia / 2);

    // read the rest

    ia = r1->items_available();
    rp = (const int*)r1->read_pointer();
    BOOST_CHECK(rp != 0);

    for (int i = 0; i < ia; i++) {
        BOOST_CHECK_EQUAL(read_counter, *rp);
        read_counter++;
        rp++;
    }
    r1->update_read_pointer(ia);
}

// ----------------------------------------------------------------------------
// single writer, single reader: check wrap-around
//

static void t2_body()
{
    // 64K is the largest granularity we've seen so far (MS windows file mapping).
    // This allows a bit of "white box testing"

    int nitems = (64 * (1L << 10)) / sizeof(int); // 64K worth of ints

    gr::buffer_sptr buf(gr::buffer_double_mapped::make_buffer(
        nitems, sizeof(int), nitems, 1, gr::block_sptr()));
    gr::buffer_reader_sptr r1(gr::buffer_add_reader(buf, 0, gr::block_sptr()));

    int read_counter = 0;
    int write_counter = 0;
    int n;
    int* wp = 0;
    const int* rp = 0;

    // Write 3/4 of buffer

    n = (int)(buf->space_available() * 0.75);
    wp = (int*)buf->write_pointer();

    for (int i = 0; i < n; i++)
        *wp++ = write_counter++;
    buf->update_write_pointer(n);

    // Now read it all

    int m = r1->items_available();
    BOOST_CHECK_EQUAL(n, m);
    rp = (const int*)r1->read_pointer();

    for (int i = 0; i < m; i++) {
        BOOST_CHECK_EQUAL(read_counter, *rp);
        read_counter++;
        rp++;
    }
    r1->update_read_pointer(m);

    // Now write as much as we can.
    // This will wrap around the buffer

    n = buf->space_available();
    BOOST_CHECK_EQUAL(nitems - 1, n); // white box test
    wp = (int*)buf->write_pointer();

    for (int i = 0; i < n; i++)
        *wp++ = write_counter++;
    buf->update_write_pointer(n);

    // now read it all

    m = r1->items_available();
    BOOST_CHECK_EQUAL(n, m);
    rp = (const int*)r1->read_pointer();

    for (int i = 0; i < m; i++) {
        BOOST_CHECK_EQUAL(read_counter, *rp);
        read_counter++;
        rp++;
    }
    r1->update_read_pointer(m);
}

// ----------------------------------------------------------------------------
// single writer, N readers, randomized order and lengths
// ----------------------------------------------------------------------------

static void t3_body()
{
    int nitems = (64 * (1L << 10)) / sizeof(int);

    static const int N = 5;
    gr::buffer_sptr buf(gr::buffer_double_mapped::make_buffer(
        nitems, sizeof(int), nitems, 1, gr::block_sptr()));
    gr::buffer_reader_sptr reader[N];
    int read_counter[N];
    int write_counter = 0;
    gr::random random;

    for (int i = 0; i < N; i++) {
        read_counter[i] = 0;
        reader[i] = buffer_add_reader(buf, 0, gr::block_sptr());
    }

    for (int lc = 0; lc < 1000; lc++) {

        // write some

        int n = (int)(buf->space_available() * random.ran1());
        int* wp = (int*)buf->write_pointer();

        for (int i = 0; i < n; i++)
            *wp++ = write_counter++;

        buf->update_write_pointer(n);

        // pick a random reader and read some

        int r = (int)(N * random.ran1());

        int m = reader[r]->items_available();
        const int* rp = (const int*)reader[r]->read_pointer();

        for (int i = 0; i < m; i++) {
            BOOST_CHECK_EQUAL(read_counter[r], *rp);
            read_counter[r]++;
            rp++;
        }
        reader[r]->update_read_pointer(m);
    }
}


// ----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(t0) { leak_check(t0_body); }

BOOST_AUTO_TEST_CASE(t1) { leak_check(t1_body); }

BOOST_AUTO_TEST_CASE(t2) { leak_check(t2_body); }

BOOST_AUTO_TEST_CASE(t3) { leak_check(t3_body); }

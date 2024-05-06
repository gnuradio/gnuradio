/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "check_lfsr_32k_s_impl.h"
#include <gnuradio/io_signature.h>

static constexpr bool CHECK_LFSR_DEBUG = false;

namespace gr {
namespace blocks {

check_lfsr_32k_s::sptr check_lfsr_32k_s::make()
{
    return gnuradio::make_block_sptr<check_lfsr_32k_s_impl>();
}

check_lfsr_32k_s_impl::check_lfsr_32k_s_impl()
    : sync_block("check_lfsr_32k",
                 io_signature::make(1, 1, sizeof(short)),
                 io_signature::make(0, 0, 0)),
      d_state(SEARCHING),
      d_history(0),
      d_ntotal(0),
      d_nright(0),
      d_runlength(0),
      d_index(0)
{
    lfsr_32k lfsr;

    for (int i = 0; i < BUFSIZE; i++)
        d_buffer[i] = lfsr.next_short();

    enter_SEARCHING();
}

check_lfsr_32k_s_impl::~check_lfsr_32k_s_impl() {}

int check_lfsr_32k_s_impl::work(int noutput_items,
                                gr_vector_const_void_star& input_items,
                                gr_vector_void_star& output_items)
{
    const unsigned short* in = (const unsigned short*)input_items[0];

    for (int i = 0; i < noutput_items; i++) {
        unsigned short x = in[i];
        unsigned short expected;

        switch (d_state) {
        case MATCH0:
            if (x == d_buffer[0])
                enter_MATCH1();
            break;

        case MATCH1:
            if (x == d_buffer[1])
                enter_MATCH2();
            else
                enter_MATCH0();
            break;

        case MATCH2:
            if (x == d_buffer[2])
                enter_LOCKED();
            else
                enter_MATCH0();
            break;

        case LOCKED:
            expected = d_buffer[d_index];
            d_index = d_index + 1;
            if (d_index >= BUFSIZE)
                d_index = 0;

            if (x == expected)
                right();
            else {
                wrong();
                log_error(expected, x);
                if (wrong_three_times())
                    enter_SEARCHING();
            }
            break;

        default:
            abort();
        }

        d_ntotal++;
    }

    return noutput_items;
}

void check_lfsr_32k_s_impl::enter_SEARCHING()
{
    d_state = SEARCHING;
    wrong(); // reset history
    wrong();
    wrong();

    d_runlength = 0;
    d_index = 0; // reset LFSR to beginning

    if (CHECK_LFSR_DEBUG)
        d_debug_logger->debug("check_lfsr_32k: enter_SEARCHING at offset {:8d} ({:#08x})",
                              d_ntotal,
                              d_ntotal);

    enter_MATCH0();
}

void check_lfsr_32k_s_impl::enter_MATCH0() { d_state = MATCH0; }

void check_lfsr_32k_s_impl::enter_MATCH1() { d_state = MATCH1; }

void check_lfsr_32k_s_impl::enter_MATCH2() { d_state = MATCH2; }

void check_lfsr_32k_s_impl::enter_LOCKED()
{
    d_state = LOCKED;
    right(); // setup history
    right();
    right();

    d_index = 3; // already matched first 3 items

    if (CHECK_LFSR_DEBUG)
        d_debug_logger->debug(
            "check_lfsr_32k: enter_LOCKED at offset {:8d} ({:#08x})", d_ntotal, d_ntotal);
}

void check_lfsr_32k_s_impl::log_error(unsigned short expected, unsigned short actual)
{
    if (CHECK_LFSR_DEBUG)
        d_debug_logger->debug("check_lfsr_32k: expected {:5d} ({:#04x}) got {:5d} "
                              "({:#04x}) offset {:8d} ({:#08x})",
                              expected,
                              expected,
                              actual,
                              actual,
                              d_ntotal,
                              d_ntotal);
}

} /* namespace blocks */
} /* namespace gr */

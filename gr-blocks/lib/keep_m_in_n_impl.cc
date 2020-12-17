/* -*- c++ -*- */
/*
 * Copyright 2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "keep_m_in_n_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace blocks {

keep_m_in_n::sptr keep_m_in_n::make(size_t itemsize, int m, int n, int offset)
{
    return gnuradio::make_block_sptr<keep_m_in_n_impl>(itemsize, m, n, offset);
}

keep_m_in_n_impl::keep_m_in_n_impl(size_t itemsize, int m, int n, int offset)
    : block("keep_m_in_n",
            io_signature::make(1, 1, itemsize),
            io_signature::make(1, 1, itemsize)),
      d_m(m),
      d_n(n),
      d_offset(offset),
      d_itemsize(itemsize)
{
    // sanity checking
    if (d_m <= 0) {
        std::string s =
            boost::str(boost::format("keep_m_in_n: m=%1% but must be > 0") % d_m);
        throw std::runtime_error(s);
    }
    if (d_n <= 0) {
        std::string s =
            boost::str(boost::format("keep_m_in_n: n=%1% but must be > 0") % d_n);
        throw std::runtime_error(s);
    }
    if (d_m > d_n) {
        std::string s =
            boost::str(boost::format("keep_m_in_n: m (%1%) <= n %2%") % d_m % d_n);
        throw std::runtime_error(s);
    }
    if (d_offset < 0) {
        std::string s = boost::str(
            boost::format("keep_m_in_n: offset (%1%) must be >= 0") % d_offset);
        throw std::runtime_error(s);
    }
    if (d_offset >= d_n) {
        std::string s = boost::str(boost::format("keep_m_in_n: offset (%1%) < n (%2%)") %
                                   d_offset % d_n);
        throw std::runtime_error(s);
    }

    set_output_multiple(m);
    set_relative_rate(static_cast<uint64_t>(d_m), static_cast<uint64_t>(d_n));
}

void keep_m_in_n_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{
    ninput_items_required[0] = d_n * (noutput_items / d_m);
}

void keep_m_in_n_impl::set_m(int m)
{
    d_m = m;
    set_output_multiple(m);
    set_relative_rate(static_cast<uint64_t>(d_m), static_cast<uint64_t>(d_n));
}

void keep_m_in_n_impl::set_n(int n)
{
    d_n = n;
    set_relative_rate(static_cast<uint64_t>(d_m), static_cast<uint64_t>(d_n));
}

void keep_m_in_n_impl::set_offset(int offset) { d_offset = offset; }

int keep_m_in_n_impl::general_work(int noutput_items,
                                   gr_vector_int& ninput_items,
                                   gr_vector_const_void_star& input_items,
                                   gr_vector_void_star& output_items)
{
    uint8_t* out = (uint8_t*)output_items[0];
    const uint8_t* in = (const uint8_t*)input_items[0];

    // iterate over data blocks of size {n, input : m, output}
    int blks = std::min(noutput_items / d_m, ninput_items[0] / d_n);
    int excess = (d_offset + d_m - d_n) * d_itemsize;

    for (int i = 0; i < blks; i++) {
        // set up copy pointers
        const uint8_t* iptr = &in[(i * d_n + d_offset) * d_itemsize];
        uint8_t* optr = &out[i * d_m * d_itemsize];
        // perform copy
        if (excess <= 0) {
            memcpy(optr, iptr, d_m * d_itemsize);
        } else {
            memcpy(optr, &in[i * d_n * d_itemsize], excess);
            memcpy(optr + excess, iptr, d_m * d_itemsize - excess);
        }
    }

    consume_each(blks * d_n);
    return blks * d_m;
}

} /* namespace blocks */
} /* namespace gr */

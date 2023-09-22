/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "interleave_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace blocks {

interleave::sptr interleave::make(size_t itemsize, unsigned int blocksize)
{
    return gnuradio::make_block_sptr<interleave_impl>(itemsize, blocksize);
}

interleave_impl::interleave_impl(size_t itemsize, unsigned int blocksize)
    : block("interleave",
            io_signature::make(1, io_signature::IO_INFINITE, itemsize),
            io_signature::make(1, 1, itemsize)),
      d_itemsize(itemsize),
      d_blocksize(blocksize)
{
    set_fixed_rate(true);
    set_output_multiple(d_blocksize);
}

bool interleave_impl::check_topology(int ninputs, int noutputs)
{
    set_relative_rate((uint64_t)ninputs, 1);
    d_ninputs = ninputs;
    set_output_multiple(d_blocksize * d_ninputs);
    return true;
}


int interleave_impl::fixed_rate_ninput_to_noutput(int ninput)
{
    return ninput * d_ninputs;
}

int interleave_impl::fixed_rate_noutput_to_ninput(int noutput)
{
    return (noutput / d_ninputs);
}

void interleave_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{
    for (unsigned int i = 0; i < ninput_items_required.size(); ++i) {
        ninput_items_required[i] = noutput_items / ninput_items_required.size();
    }
}

int interleave_impl::general_work(int noutput_items,
                                  gr_vector_int& ninput_items,
                                  gr_vector_const_void_star& input_items,
                                  gr_vector_void_star& output_items)
{
    size_t noutput_blocks = (size_t)(noutput_items / d_blocksize);
    const char** in = (const char**)&input_items[0];
    char* out = (char*)output_items[0];

    for (unsigned int i = 0; i < noutput_blocks; i += d_ninputs) {
        for (unsigned int n = 0; n < d_ninputs; n++) {
            memcpy(out, in[n], d_itemsize * d_blocksize);
            out += d_itemsize * d_blocksize;
            in[n] += d_itemsize * d_blocksize;
        }
    }
    consume_each(noutput_items / d_ninputs);
    return noutput_items;
}


} /* namespace blocks */
} /* namespace gr */

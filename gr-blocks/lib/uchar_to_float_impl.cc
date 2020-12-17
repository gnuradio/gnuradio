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

#include "uchar_array_to_float.h"
#include "uchar_to_float_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace blocks {

uchar_to_float::sptr uchar_to_float::make()
{
    return gnuradio::make_block_sptr<uchar_to_float_impl>();
}

uchar_to_float_impl::uchar_to_float_impl()
    : sync_block("uchar_to_float",
                 io_signature::make(1, 1, sizeof(unsigned char)),
                 io_signature::make(1, 1, sizeof(float)))
{
}

int uchar_to_float_impl::work(int noutput_items,
                              gr_vector_const_void_star& input_items,
                              gr_vector_void_star& output_items)
{
    const unsigned char* in = (const unsigned char*)input_items[0];
    float* out = (float*)output_items[0];

    uchar_array_to_float(in, out, noutput_items);

    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */

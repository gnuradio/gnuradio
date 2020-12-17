/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ival_decimator_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace filter {


ival_decimator::sptr ival_decimator::make(int decimation, int data_size)
{
    return gnuradio::make_block_sptr<ival_decimator_impl>(decimation, data_size);
}

/*
 * The private constructor
 */
ival_decimator_impl::ival_decimator_impl(int decimation, int data_size)
    : gr::sync_decimator("ival_decimator",
                         gr::io_signature::make(1, 1, data_size),
                         gr::io_signature::make(1, 1, data_size),
                         decimation),
      d_data_size(data_size)
{
    if ((data_size != 1) && (data_size != 2))
        throw std::invalid_argument("data size must be in 1 (char) or 2 (short)");

    d_doubledecimation = 2 * decimation;

    // Make sure we work with pairs of bytes (I and Q as byte)
    gr::block::set_output_multiple(2);
}

int ival_decimator_impl::work(int noutput_items,
                              gr_vector_const_void_star& input_items,
                              gr_vector_void_star& output_items)
{
    long i = 0;
    int octr = 0;

    long nin = noutput_items * decimation();

    switch (d_data_size) {
    case 2: {
        const int16_t* in = (const int16_t*)input_items[0];
        int16_t* out = (int16_t*)output_items[0];

        while (i < nin) {
            *out++ = in[i];
            *out++ = in[i + 1];

            i += d_doubledecimation;
            octr += 2;
        }
    } break;

    case 1: {
        const char* in = (const char*)input_items[0];
        char* out = (char*)output_items[0];

        while (i < nin) {
            *out++ = in[i];
            *out++ = in[i + 1];

            i += d_doubledecimation;
            octr += 2;
        }
    } break;
    }

    // Tell runtime system how many output items we produced.
    return octr;
}

} /* namespace filter */
} /* namespace gr */

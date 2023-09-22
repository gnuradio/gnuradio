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

#include "correctiq_swapiq_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace blocks {

swap_iq::sptr swap_iq::make(int datatype, int datasize)
{
    return gnuradio::make_block_sptr<swap_iq_impl>(datatype, datasize);
}

/*
 * The private constructor
 */
swap_iq_impl::swap_iq_impl(int datatype, int datasize)
    : gr::sync_block("swap_iq",
                     gr::io_signature::make(1, 1, datasize),
                     gr::io_signature::make(1, 1, datasize)),
      d_datatype(datatype)
{
    if (d_datatype != SWAPTYPE_FLOATCOMPLEX) {
        gr::block::set_output_multiple(2); // Make sure we work with pairs
    }
}

/*
 * Our virtual destructor.
 */
swap_iq_impl::~swap_iq_impl() {}

int swap_iq_impl::work(int noutput_items,
                       gr_vector_const_void_star& input_items,
                       gr_vector_void_star& output_items)
{
    // Constructor guarantees we'll have pairs.

    long i;
    long noi;

    switch (d_datatype) {
    case SWAPTYPE_FLOATCOMPLEX: {
        noi = noutput_items * 2; // Each complex is 2 floats
        const float* in_float = (const float*)input_items[0];
        float* out_float = (float*)output_items[0];

        for (i = 0; i < noi; i += 2) {
            *out_float++ = in_float[i + 1];
            *out_float++ = in_float[i];
        }
    } break;
    case SWAPTYPE_SHORTCOMPLEX: {
        noi = noutput_items;
        const int16_t* in_short = (const int16_t*)input_items[0];
        int16_t* out_short = (int16_t*)output_items[0];

        for (i = 0; i < noi; i += 2) {
            *out_short++ = in_short[i + 1];
            *out_short++ = in_short[i];
        }
    } break;
    case SWAPTYPE_BYTECOMPLEX: {
        noi = noutput_items;
        const char* in_byte = (const char*)input_items[0];
        char* out_byte = (char*)output_items[0];

        for (i = 0; i < noi; i += 2) {
            *out_byte++ = in_byte[i + 1];
            *out_byte++ = in_byte[i];
        }
    } break;
    default: {
        d_logger->error("Unknown data type.  Bytes not swapped.");
        return WORK_DONE;
    }
    }

    // Tell runtime system how many output items we produced.
    return noutput_items;
}
} /* namespace blocks */
} /* namespace gr */

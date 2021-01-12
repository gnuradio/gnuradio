/* -*- c++ -*- */
/*
 * Copyright 2007,2010,2012,2016 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "glfsr_source_f_impl.h"
#include <gnuradio/io_signature.h>
#include <stdexcept>

namespace gr {
namespace digital {

glfsr_source_f::sptr
glfsr_source_f::make(unsigned int degree, bool repeat, uint64_t mask, uint64_t seed)
{
    return gnuradio::make_block_sptr<glfsr_source_f_impl>(degree, repeat, mask, seed);
}

glfsr_source_f_impl::glfsr_source_f_impl(unsigned int degree,
                                         bool repeat,
                                         uint64_t mask,
                                         uint64_t seed)
    : sync_block("glfsr_source_f",
                 io_signature::make(0, 0, 0),
                 io_signature::make(1, 1, sizeof(float))),
      d_glfsr(mask ? mask : glfsr::glfsr_mask(degree), seed),
      d_repeat(repeat),
      d_index(0),
      d_length((1ULL << degree) - 1)
{
    if (degree < 1 || degree > 64)
        throw std::runtime_error("glfsr_source_f_impl: must be 1 <= degree <= 64");
}

glfsr_source_f_impl::~glfsr_source_f_impl() {}

uint64_t glfsr_source_f_impl::mask() const { return d_glfsr.mask(); }

int glfsr_source_f_impl::work(int noutput_items,
                              gr_vector_const_void_star& input_items,
                              gr_vector_void_star& output_items)
{
    float* out = (float*)output_items[0];
    if ((d_index > d_length) && d_repeat == false)
        return WORK_DONE; /* once through the sequence */

    int i;
    for (i = 0; i < noutput_items; i++) {
        out[i] = (float)d_glfsr.next_bit() * 2.0 - 1.0;
        d_index++;
        if (d_index > d_length && d_repeat == false)
            break;
    }

    return i;
}

} /* namespace digital */
} /* namespace gr */

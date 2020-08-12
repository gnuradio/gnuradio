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

#include "glfsr_source_b_impl.h"
#include <gnuradio/io_signature.h>
#include <stdexcept>

namespace gr {
namespace digital {

glfsr_source_b::sptr
glfsr_source_b::make(unsigned int degree, bool repeat, uint32_t mask, uint32_t seed)
{
    return gnuradio::make_block_sptr<glfsr_source_b_impl>(degree, repeat, mask, seed);
}

glfsr_source_b_impl::glfsr_source_b_impl(unsigned int degree,
                                         bool repeat,
                                         uint32_t mask,
                                         uint32_t seed)
    : sync_block("glfsr_source_b",
                 io_signature::make(0, 0, 0),
                 io_signature::make(1, 1, sizeof(unsigned char))),
      d_glfsr(mask ? mask : glfsr::glfsr_mask(degree), seed),
      d_repeat(repeat),
      d_index(0),
      d_length((((uint32_t)1) << degree) - 1)
{
    if (degree < 1 || degree > 32)
        throw std::runtime_error(
            "glfsr_source_b_impl: degree must be between 1 and 32 inclusive");
}

glfsr_source_b_impl::~glfsr_source_b_impl() {}

uint32_t glfsr_source_b_impl::mask() const { return d_glfsr.mask(); }

int glfsr_source_b_impl::work(int noutput_items,
                              gr_vector_const_void_star& input_items,
                              gr_vector_void_star& output_items)
{
    char* out = (char*)output_items[0];
    if ((d_index > d_length) && d_repeat == false)
        return WORK_DONE; /* once through the sequence */

    int i;
    for (i = 0; i < noutput_items; i++) {
        out[i] = d_glfsr.next_bit();
        d_index++;
        if (d_index > d_length && d_repeat == false)
            break;
    }

    return i;
}

} /* namespace digital */
} /* namespace gr */

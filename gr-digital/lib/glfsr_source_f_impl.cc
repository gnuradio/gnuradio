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
glfsr_source_f::make(unsigned int degree, bool repeat, uint32_t mask, uint32_t seed)
{
    return gnuradio::get_initial_sptr(
        new glfsr_source_f_impl(degree, repeat, mask, seed));
}

glfsr_source_f_impl::glfsr_source_f_impl(unsigned int degree,
                                         bool repeat,
                                         uint32_t mask,
                                         uint32_t seed)
    : sync_block("glfsr_source_f",
                 io_signature::make(0, 0, 0),
                 io_signature::make(1, 1, sizeof(float))),
      d_repeat(repeat),
      d_index(0),
      d_length((((uint32_t)1) << degree) - 1)
{
    if (degree < 1 || degree > 32)
        throw std::runtime_error(
            "glfsr_source_f_impl: degree must be between 1 and 32 inclusive");

    if (mask == 0)
        mask = glfsr::glfsr_mask(degree);
    d_glfsr = new glfsr(mask, seed);
}

glfsr_source_f_impl::~glfsr_source_f_impl() { delete d_glfsr; }

uint32_t glfsr_source_f_impl::mask() const { return d_glfsr->mask(); }

int glfsr_source_f_impl::work(int noutput_items,
                              gr_vector_const_void_star& input_items,
                              gr_vector_void_star& output_items)
{
    float* out = (float*)output_items[0];
    if ((d_index > d_length) && d_repeat == false)
        return WORK_DONE; /* once through the sequence */

    int i;
    for (i = 0; i < noutput_items; i++) {
        out[i] = (float)d_glfsr->next_bit() * 2.0 - 1.0;
        d_index++;
        if (d_index > d_length && d_repeat == false)
            break;
    }

    return i;
}

} /* namespace digital */
} /* namespace gr */

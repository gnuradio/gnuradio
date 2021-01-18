/* -*- c++ -*- */
/*
 * Copyright 2005,2010,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "vco_f_impl.h"
#include <gnuradio/io_signature.h>
#include <cmath>

namespace gr {
namespace blocks {

vco_f::sptr vco_f::make(double sampling_rate, double sensitivity, double amplitude)
{
    return gnuradio::make_block_sptr<vco_f_impl>(sampling_rate, sensitivity, amplitude);
}

vco_f_impl::vco_f_impl(double sampling_rate, double sensitivity, double amplitude)
    : sync_block("vco_f",
                 io_signature::make(1, 1, sizeof(float)),
                 io_signature::make(1, 1, sizeof(float))),
      d_sampling_rate(sampling_rate),
      d_sensitivity(sensitivity),
      d_amplitude(amplitude),
      d_k(d_sensitivity / d_sampling_rate)
{
}

vco_f_impl::~vco_f_impl() {}

int vco_f_impl::work(int noutput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items)
{
    const float* input = (const float*)input_items[0];
    float* output = (float*)output_items[0];

    d_vco.cos(output, input, noutput_items, d_k, d_amplitude);

    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */

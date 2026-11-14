/* -*- c++ -*- */
/*
 * Copyright 2005,2010,2013 Free Software Foundation, Inc.
 * Copyright 2026 Jay Bhatt
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "vco_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace blocks {

template <typename T>
typename vco<T>::sptr vco<T>::make(double sampling_rate, double sensitivity, double amplitude)
{
    return gnuradio::make_block_sptr<vco_impl<T>>(sampling_rate, sensitivity, amplitude);
}

template <>
vco_impl<float>::vco_impl(double sampling_rate, double sensitivity, double amplitude)
    : sync_block("vco_f",
                 io_signature::make(1, 1, sizeof(float)),
                 io_signature::make(1, 1, sizeof(float))),
      d_sampling_rate(sampling_rate),
      d_sensitivity(sensitivity),
      d_amplitude(amplitude),
      d_k(d_sensitivity / d_sampling_rate)
{
}

template <>
vco_impl<gr_complex>::vco_impl(double sampling_rate, double sensitivity, double amplitude)
    : sync_block("vco_c",
                 io_signature::make(1, 1, sizeof(float)),
                 io_signature::make(1, 1, sizeof(gr_complex))),
      d_sampling_rate(sampling_rate),
      d_sensitivity(sensitivity),
      d_amplitude(amplitude),
      d_k(d_sensitivity / d_sampling_rate)
{
}

template <typename T>
vco_impl<T>::~vco_impl()
{
}

template <>
int vco_impl<float>::work(int noutput_items,
                          gr_vector_const_void_star& input_items,
                          gr_vector_void_star& output_items)
{
    d_vco.cos(static_cast<float*>(output_items[0]),
              static_cast<const float*>(input_items[0]),
              noutput_items,
              d_k,
              d_amplitude);
    return noutput_items;
}

template <>
int vco_impl<gr_complex>::work(int noutput_items,
                               gr_vector_const_void_star& input_items,
                               gr_vector_void_star& output_items)
{
    d_vco.sincos(static_cast<gr_complex*>(output_items[0]),
                 static_cast<const float*>(input_items[0]),
                 noutput_items,
                 d_k,
                 d_amplitude);
    return noutput_items;
}

template class vco<float>;
template class vco<gr_complex>;

} /* namespace blocks */
} /* namespace gr */

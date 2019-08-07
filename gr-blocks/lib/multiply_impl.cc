/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2010,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "multiply_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

namespace gr {
namespace blocks {

template <class T>
typename multiply<T>::sptr multiply<T>::make(size_t vlen)
{
    return gnuradio::get_initial_sptr(new multiply_impl<T>(vlen));
}

template <>
multiply_impl<float>::multiply_impl(size_t vlen)
    : sync_block("multiply_ff",
                 io_signature::make(1, -1, sizeof(float) * vlen),
                 io_signature::make(1, 1, sizeof(float) * vlen)),
      d_vlen(vlen)
{
    const int alignment_multiple = volk_get_alignment() / sizeof(float);
    set_alignment(std::max(1, alignment_multiple));
}

template <>
int multiply_impl<float>::work(int noutput_items,
                               gr_vector_const_void_star& input_items,
                               gr_vector_void_star& output_items)
{
    float* out = (float*)output_items[0];
    int noi = d_vlen * noutput_items;

    memcpy(out, input_items[0], noi * sizeof(float));
    for (size_t i = 1; i < input_items.size(); i++)
        volk_32f_x2_multiply_32f(out, out, (float*)input_items[i], noi);

    return noutput_items;
}

template <>
multiply_impl<gr_complex>::multiply_impl(size_t vlen)
    : sync_block("multiply_cc",
                 io_signature::make(1, -1, sizeof(gr_complex) * vlen),
                 io_signature::make(1, 1, sizeof(gr_complex) * vlen)),
      d_vlen(vlen)
{
    const int alignment_multiple = volk_get_alignment() / sizeof(gr_complex);
    set_alignment(std::max(1, alignment_multiple));
}

template <>
int multiply_impl<gr_complex>::work(int noutput_items,
                                    gr_vector_const_void_star& input_items,
                                    gr_vector_void_star& output_items)
{
    gr_complex* out = (gr_complex*)output_items[0];
    int noi = d_vlen * noutput_items;

    memcpy(out, input_items[0], noi * sizeof(gr_complex));
    for (size_t i = 1; i < input_items.size(); i++)
        volk_32fc_x2_multiply_32fc(out, out, (gr_complex*)input_items[i], noi);

    return noutput_items;
}

template <class T>
multiply_impl<T>::multiply_impl(size_t vlen)
    : sync_block("multiply",
                 io_signature::make(1, -1, sizeof(T) * vlen),
                 io_signature::make(1, 1, sizeof(T) * vlen)),
      d_vlen(vlen)
{
}

template <class T>
int multiply_impl<T>::work(int noutput_items,
                           gr_vector_const_void_star& input_items,
                           gr_vector_void_star& output_items)
{
    T* optr = (T*)output_items[0];

    int ninputs = input_items.size();

    for (size_t i = 0; i < noutput_items * d_vlen; i++) {
        T acc = ((T*)input_items[0])[i];
        for (int j = 1; j < ninputs; j++)
            acc *= ((T*)input_items[j])[i];

        *optr++ = (T)acc;
    }

    return noutput_items;
}

template class multiply<std::int16_t>;
template class multiply<std::int32_t>;
template class multiply<gr_complex>;
template class multiply<float>;
} /* namespace blocks */
} /* namespace gr */

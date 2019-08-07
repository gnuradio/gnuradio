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

#include "divide_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

namespace gr {
namespace blocks {

template <class T>
typename divide<T>::sptr divide<T>::make(size_t vlen)
{
    return gnuradio::get_initial_sptr(new divide_impl<T>(vlen));
}

template <>
divide_impl<float>::divide_impl(size_t vlen)
    : sync_block("divide",
                 io_signature::make(2, -1, sizeof(float) * vlen),
                 io_signature::make(1, 1, sizeof(float) * vlen)),
      d_vlen(vlen)
{
}

template <>
int divide_impl<float>::work(int noutput_items,
                             gr_vector_const_void_star& input_items,
                             gr_vector_void_star& output_items)
{
    float* optr = (float*)output_items[0];
    size_t ninputs = input_items.size();
    float* numerator = (float*)input_items[0];
    for (size_t inp = 1; inp < ninputs; ++inp) {
        volk_32f_x2_divide_32f(
            optr, numerator, (float*)input_items[inp], noutput_items * d_vlen);
        numerator = optr;
    }

    return noutput_items;
}

template <>
divide_impl<gr_complex>::divide_impl(size_t vlen)
    : sync_block("divide_cc",
                 io_signature::make(2, -1, sizeof(gr_complex) * vlen),
                 io_signature::make(1, 1, sizeof(gr_complex) * vlen)),
      d_vlen(vlen)
{
}

template <>
int divide_impl<gr_complex>::work(int noutput_items,
                                  gr_vector_const_void_star& input_items,
                                  gr_vector_void_star& output_items)
{
    gr_complex* optr = (gr_complex*)output_items[0];
    size_t ninputs = input_items.size();
    gr_complex* numerator = (gr_complex*)input_items[0];
    for (size_t inp = 1; inp < ninputs; ++inp) {
        volk_32fc_x2_divide_32fc(
            optr, numerator, (gr_complex*)input_items[inp], noutput_items * d_vlen);
        numerator = optr;
    }

    return noutput_items;
}

template <class T>
divide_impl<T>::divide_impl(size_t vlen)
    : sync_block("divide",
                 io_signature::make(1, -1, sizeof(T) * vlen),
                 io_signature::make(1, 1, sizeof(T) * vlen)),
      d_vlen(vlen)
{
}

template <class T>
int divide_impl<T>::work(int noutput_items,
                         gr_vector_const_void_star& input_items,
                         gr_vector_void_star& output_items)
{
    T* optr = (T*)output_items[0];

    int ninputs = input_items.size();

    for (size_t i = 0; i < noutput_items * d_vlen; i++) {
        T acc = ((T*)input_items[0])[i];
        for (int j = 1; j < ninputs; j++)
            acc /= ((T*)input_items[j])[i];

        *optr++ = (T)acc;
    }

    return noutput_items;
}

template class divide<std::int16_t>;
template class divide<std::int32_t>;
template class divide<float>;
template class divide<gr_complex>;
} /* namespace blocks */
} /* namespace gr */

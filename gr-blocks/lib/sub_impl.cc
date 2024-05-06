/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2010,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "sub_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

namespace gr {
namespace blocks {

template <class T>
typename sub<T>::sptr sub<T>::make(size_t vlen)
{
    return gnuradio::make_block_sptr<sub_impl<T>>(vlen);
}

template <>
sub_impl<float>::sub_impl(size_t vlen)
    : sync_block("sub_ff",
                 io_signature::make(1, -1, sizeof(float) * vlen),
                 io_signature::make(1, 1, sizeof(float) * vlen)),
      d_vlen(vlen)
{
    const int alignment_multiple = volk_get_alignment() / sizeof(float);
    set_alignment(std::max(1, alignment_multiple));
}

template <>
int sub_impl<float>::work(int noutput_items,
                          gr_vector_const_void_star& input_items,
                          gr_vector_void_star& output_items)
{
    float* out = (float*)output_items[0];

    int ninputs = input_items.size();
    int noi = noutput_items * d_vlen;

    const float* in = (const float*)input_items[0];
    memcpy(out, in, noi * sizeof(float));
    for (int i = 1; i < ninputs; i++) {
        in = (const float*)input_items[i];
        volk_32f_x2_subtract_32f(out, out, in, noi);
    }

    return noutput_items;
}

template <class T>
sub_impl<T>::sub_impl(size_t vlen)
    : sync_block("sub",
                 io_signature::make(1, -1, sizeof(T) * vlen),
                 io_signature::make(1, 1, sizeof(T) * vlen)),
      d_vlen(vlen)
{
}

template <class T>
int sub_impl<T>::work(int noutput_items,
                      gr_vector_const_void_star& input_items,
                      gr_vector_void_star& output_items)
{
    T* optr = (T*)output_items[0];

    int ninputs = input_items.size();

    for (size_t i = 0; i < noutput_items * d_vlen; i++) {
        T acc = ((const T*)input_items[0])[i];
        for (int j = 1; j < ninputs; j++)
            acc -= ((const T*)input_items[j])[i];

        *optr++ = (T)acc;
    }

    return noutput_items;
}

template class sub<std::int16_t>;
template class sub<std::int32_t>;
template class sub<gr_complex>;
template class sub<float>;
} /* namespace blocks */
} /* namespace gr */

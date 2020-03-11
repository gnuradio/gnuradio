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

#include "add_blk_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

namespace gr {
namespace blocks {

template <class T>
typename add_blk<T>::sptr add_blk<T>::make(size_t vlen)
{
    return gnuradio::get_initial_sptr(new add_blk_impl<T>(vlen));
}


template <>
add_blk_impl<float>::add_blk_impl(size_t vlen)
    : sync_block("add_ff",
                 io_signature::make(1, -1, sizeof(float) * vlen),
                 io_signature::make(1, 1, sizeof(float) * vlen)),
      d_vlen(vlen)
{
    const int alignment_multiple = volk_get_alignment() / sizeof(float);
    set_alignment(std::max(1, alignment_multiple));
}

template <>
int add_blk_impl<float>::work(int noutput_items,
                              gr_vector_const_void_star& input_items,
                              gr_vector_void_star& output_items)
{
    float* out = (float*)output_items[0];
    int noi = d_vlen * noutput_items;

    memcpy(out, input_items[0], noi * sizeof(float));
    for (size_t i = 1; i < input_items.size(); i++)
        volk_32f_x2_add_32f(out, out, (const float*)input_items[i], noi);
    return noutput_items;
}


template <class T>
add_blk_impl<T>::add_blk_impl(size_t vlen)
    : sync_block("add_blk",
                 io_signature::make(1, -1, sizeof(T) * vlen),
                 io_signature::make(1, 1, sizeof(T) * vlen)),
      d_vlen(vlen)
{
}

template <class T>
int add_blk_impl<T>::work(int noutput_items,
                          gr_vector_const_void_star& input_items,
                          gr_vector_void_star& output_items)
{
    T* optr = (T*)output_items[0];

    int ninputs = input_items.size();

    for (size_t i = 0; i < noutput_items * d_vlen; i++) {
        T acc = ((T*)input_items[0])[i];
        for (int j = 1; j < ninputs; j++)
            acc += ((T*)input_items[j])[i];

        *optr++ = (T)acc;
    }

    return noutput_items;
}

template class add_blk<std::int16_t>;
template class add_blk<std::int32_t>;
template class add_blk<gr_complex>;
template class add_blk<float>;
} /* namespace blocks */
} /* namespace gr */

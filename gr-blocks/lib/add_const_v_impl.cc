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

#include "add_const_v_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace blocks {

template <class T>
typename add_const_v<T>::sptr add_const_v<T>::make(std::vector<T> k)
{
    return gnuradio::make_block_sptr<add_const_v_impl<T>>(k);
}

template <class T>
add_const_v_impl<T>::add_const_v_impl(std::vector<T> k)
    : sync_block("add_const_v",
                 io_signature::make(1, 1, sizeof(T) * k.size()),
                 io_signature::make(1, 1, sizeof(T) * k.size())),
      d_k(k)
{
}

template <class T>
int add_const_v_impl<T>::work(int noutput_items,
                              gr_vector_const_void_star& input_items,
                              gr_vector_void_star& output_items)
{
    const T* iptr = (const T*)input_items[0];
    T* optr = (T*)output_items[0];

    int nitems_per_block = this->output_signature()->sizeof_stream_item(0) / sizeof(T);

    for (int i = 0; i < noutput_items; i++)
        for (int j = 0; j < nitems_per_block; j++)
            *optr++ = *iptr++ + d_k[j];

    return noutput_items;
}

template class add_const_v<std::uint8_t>;
template class add_const_v<std::int16_t>;
template class add_const_v<std::int32_t>;
template class add_const_v<float>;
template class add_const_v<gr_complex>;
} /* namespace blocks */
} /* namespace gr */

/* -*- c++ -*- */
/*
 * Copyright 2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "and_blk_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace blocks {

template <class T>
typename and_blk<T>::sptr and_blk<T>::make(size_t vlen)
{
    return gnuradio::make_block_sptr<and_blk_impl<T>>(vlen);
}

template <class T>
and_blk_impl<T>::and_blk_impl(size_t vlen)
    : sync_block("and_blk",
                 io_signature::make(1, -1, sizeof(T) * vlen),
                 io_signature::make(1, 1, sizeof(T) * vlen)),
      d_vlen(vlen)
{
}

template <class T>
int and_blk_impl<T>::work(int noutput_items,
                          gr_vector_const_void_star& input_items,
                          gr_vector_void_star& output_items)
{
    T* optr = (T*)output_items[0];

    int ninputs = input_items.size();

    for (size_t i = 0; i < noutput_items * d_vlen; i++) {
        T acc = ((const T*)input_items[0])[i];
        for (int j = 1; j < ninputs; j++)
            acc &= ((const T*)input_items[j])[i];

        *optr++ = (T)acc;
    }

    return noutput_items;
}

template class and_blk<std::uint8_t>;
template class and_blk<std::int16_t>;
template class and_blk<std::int32_t>;
} /* namespace blocks */
} /* namespace gr */

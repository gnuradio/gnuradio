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

#include "abs_blk_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace blocks {

template <class T>
typename abs_blk<T>::sptr abs_blk<T>::make(size_t vlen)
{
    return gnuradio::make_block_sptr<abs_blk_impl<T>>(vlen);
}

template <class T>
abs_blk_impl<T>::abs_blk_impl(size_t vlen)
    : sync_block("abs_blk",
                 io_signature::make(1, 1, sizeof(T) * vlen),
                 io_signature::make(1, 1, sizeof(T) * vlen)),
      d_vlen(vlen)
{
}

template <class T>
int abs_blk_impl<T>::work(int noutput_items,
                          gr_vector_const_void_star& input_items,
                          gr_vector_void_star& output_items)
{
    const T* iptr = (const T*)input_items[0];
    T* optr = (T*)output_items[0];

    for (size_t i = 0; i < noutput_items * d_vlen; i++) {
        T val = iptr[i];
        optr[i] = ((val < ((T)0)) ? -val : val);
    }

    return noutput_items;
}

template class abs_blk<std::int16_t>;
template class abs_blk<std::int32_t>;
template class abs_blk<float>;
} /* namespace blocks */
} /* namespace gr */

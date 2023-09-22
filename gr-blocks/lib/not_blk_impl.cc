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

#include "not_blk_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace blocks {

template <class T>
typename not_blk<T>::sptr not_blk<T>::make(size_t vlen)
{
    return gnuradio::make_block_sptr<not_blk_impl<T>>(vlen);
}

template <class T>
not_blk_impl<T>::not_blk_impl(size_t vlen)
    : sync_block("not_blk",
                 io_signature::make(1, 1, sizeof(T) * vlen),
                 io_signature::make(1, 1, sizeof(T) * vlen)),
      d_vlen(vlen)
{
}

template <class T>
int not_blk_impl<T>::work(int noutput_items,
                          gr_vector_const_void_star& input_items,
                          gr_vector_void_star& output_items)
{
    T* optr = (T*)output_items[0];
    const T* inptr = (const T*)input_items[0];

    int noi = noutput_items * d_vlen;

    for (int i = 0; i < noi; i++)
        *optr++ = ~(inptr[i]);

    return noutput_items;
}

template class not_blk<std::uint8_t>;
template class not_blk<std::int16_t>;
template class not_blk<std::int32_t>;
} /* namespace blocks */
} /* namespace gr */

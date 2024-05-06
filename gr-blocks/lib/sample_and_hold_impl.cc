/* -*- c++ -*- */
/*
 * Copyright 2007,2010,2013,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "sample_and_hold_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace blocks {

template <class T>
typename sample_and_hold<T>::sptr sample_and_hold<T>::make()
{
    return gnuradio::make_block_sptr<sample_and_hold_impl<T>>();
}

template <class T>
sample_and_hold_impl<T>::sample_and_hold_impl()
    : sync_block("sample_and_hold",
                 io_signature::make2(2, 2, sizeof(T), sizeof(char)),
                 io_signature::make(1, 1, sizeof(T))),
      d_data(0)
{
}

template <class T>
sample_and_hold_impl<T>::~sample_and_hold_impl()
{
}

template <class T>
int sample_and_hold_impl<T>::work(int noutput_items,
                                  gr_vector_const_void_star& input_items,
                                  gr_vector_void_star& output_items)
{
    const T* iptr = (const T*)input_items[0];
    const char* ctrl = (const char*)input_items[1];
    T* optr = (T*)output_items[0];

    for (int i = 0; i < noutput_items; i++) {
        if (ctrl[i]) {
            d_data = iptr[i];
        }
        optr[i] = d_data;
    }
    return noutput_items;
}

template class sample_and_hold<std::uint8_t>;
template class sample_and_hold<std::int16_t>;
template class sample_and_hold<std::int32_t>;
template class sample_and_hold<float>;
} /* namespace blocks */
} /* namespace gr */

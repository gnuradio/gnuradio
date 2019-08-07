/* -*- c++ -*- */
/*
 * Copyright 2007,2010,2013,2018 Free Software Foundation, Inc.
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

#include "sample_and_hold_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace blocks {

template <class T>
typename sample_and_hold<T>::sptr sample_and_hold<T>::make()
{
    return gnuradio::get_initial_sptr(new sample_and_hold_impl<T>());
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
    T* iptr = (T*)input_items[0];
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

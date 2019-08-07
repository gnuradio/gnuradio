/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
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
#include <config.h>
#endif

#include "random_uniform_source_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace analog {

template <class T>
typename random_uniform_source<T>::sptr
random_uniform_source<T>::make(int minimum, int maximum, int seed)
{
    return gnuradio::get_initial_sptr(
        new random_uniform_source_impl<T>(minimum, maximum, seed));
}

template <class T>
random_uniform_source_impl<T>::random_uniform_source_impl(int minimum,
                                                          int maximum,
                                                          int seed)
    : sync_block("random_uniform_source",
                 io_signature::make(0, 0, 0),
                 io_signature::make(1, 1, sizeof(T)))
{
    d_rng = new gr::random(seed, minimum, maximum);
}

template <class T>
random_uniform_source_impl<T>::~random_uniform_source_impl()
{
    delete d_rng;
}

template <class T>
int random_uniform_source_impl<T>::random_value()
{
    return d_rng->ran_int();
}

template <class T>
int random_uniform_source_impl<T>::work(int noutput_items,
                                        gr_vector_const_void_star& input_items,
                                        gr_vector_void_star& output_items)
{
    T* out = (T*)output_items[0];

    for (int i = 0; i < noutput_items; i++) {
        *out++ = (T)random_value();
    }

    // Tell runtime system how many output items we produced.
    return noutput_items;
}

template class random_uniform_source<std::uint8_t>;
template class random_uniform_source<std::int16_t>;
template class random_uniform_source<std::int32_t>;
} /* namespace analog */
} /* namespace gr */

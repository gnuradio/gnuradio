/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
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
    return gnuradio::make_block_sptr<random_uniform_source_impl<T>>(
        minimum, maximum, seed);
}

template <class T>
random_uniform_source_impl<T>::random_uniform_source_impl(int minimum,
                                                          int maximum,
                                                          int seed)
    : sync_block("random_uniform_source",
                 io_signature::make(0, 0, 0),
                 io_signature::make(1, 1, sizeof(T))),
      d_rng(seed, minimum, maximum)
{
}

template <class T>
random_uniform_source_impl<T>::~random_uniform_source_impl()
{
}

template <class T>
int random_uniform_source_impl<T>::random_value()
{
    return d_rng.ran_int();
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

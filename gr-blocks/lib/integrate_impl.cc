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

#include "integrate_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace blocks {

template <class T>
typename integrate<T>::sptr integrate<T>::make(int decim, unsigned int vlen)
{
    return gnuradio::make_block_sptr<integrate_impl<T>>(decim, vlen);
}

template <class T>
integrate_impl<T>::integrate_impl(int decim, unsigned int vlen)
    : sync_decimator("integrate",
                     io_signature::make(1, 1, sizeof(T) * vlen),
                     io_signature::make(1, 1, sizeof(T) * vlen),
                     decim),
      d_decim(decim),
      d_vlen(vlen)
{
}

template <class T>
int integrate_impl<T>::work(int noutput_items,
                            gr_vector_const_void_star& input_items,
                            gr_vector_void_star& output_items)
{
    const T* in = (const T*)input_items[0];
    T* out = (T*)output_items[0];

    for (int i = 0; i < noutput_items; i++) {
        for (unsigned int j = 0; j < d_vlen; ++j) {
            out[i * d_vlen + j] = (T)0;
        }
        for (int j = 0; j < d_decim; j++) {
            for (unsigned int k = 0; k < d_vlen; ++k) {
                out[i * d_vlen + k] += in[i * d_decim * d_vlen + j * d_vlen + k];
            }
        }
    }

    return noutput_items;
}

template class integrate<std::int16_t>;
template class integrate<std::int32_t>;
template class integrate<float>;
template class integrate<gr_complex>;
} /* namespace blocks */
} /* namespace gr */

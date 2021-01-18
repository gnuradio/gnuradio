/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2013,2018,2019 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "mute_impl.h"
#include <gnuradio/io_signature.h>
#include <algorithm>
#include <cstring>

namespace gr {
namespace blocks {

template <class T>
typename mute_blk<T>::sptr mute_blk<T>::make(bool mute)
{
    return gnuradio::make_block_sptr<mute_impl<T>>(mute);
}

template <class T>
mute_impl<T>::mute_impl(bool mute)
    : sync_block("mute",
                 io_signature::make(1, 1, sizeof(T)),
                 io_signature::make(1, 1, sizeof(T))),
      d_mute(mute)
{
    this->message_port_register_in(pmt::intern("set_mute"));
    this->set_msg_handler(pmt::intern("set_mute"),
                          [this](pmt::pmt_t msg) { this->set_mute_pmt(msg); });
}

template <class T>
mute_impl<T>::~mute_impl()
{
}

template <class T>
int mute_impl<T>::work(int noutput_items,
                       gr_vector_const_void_star& input_items,
                       gr_vector_void_star& output_items)
{
    T* iptr = (T*)input_items[0];
    T* optr = (T*)output_items[0];

    int size = noutput_items;

    if (d_mute) {
        std::fill_n(optr, noutput_items, 0);
    } else {
        while (size >= 8) {
            *optr++ = *iptr++;
            *optr++ = *iptr++;
            *optr++ = *iptr++;
            *optr++ = *iptr++;
            *optr++ = *iptr++;
            *optr++ = *iptr++;
            *optr++ = *iptr++;
            *optr++ = *iptr++;
            size -= 8;
        }

        while (size-- > 0)
            *optr++ = *iptr++;
    }

    return noutput_items;
}

template class mute_blk<std::int16_t>;
template class mute_blk<std::int32_t>;
template class mute_blk<float>;
template class mute_blk<gr_complex>;
} /* namespace blocks */
} /* namespace gr */

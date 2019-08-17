/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2013,2018,2019 Free Software Foundation, Inc.
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

#include "mute_impl.h"
#include <gnuradio/io_signature.h>
#include <string.h>
#include <algorithm>

namespace gr {
namespace blocks {

template <class T>
typename mute_blk<T>::sptr mute_blk<T>::make(bool mute)
{
    return gnuradio::get_initial_sptr(new mute_impl<T>(mute));
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
                          boost::bind(&mute_impl<T>::set_mute_pmt, this, _1));
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

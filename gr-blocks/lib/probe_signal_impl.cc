/* -*- c++ -*- */
/*
 * Copyright 2005,2010,2012-2013,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "probe_signal_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace blocks {

template <class T>
typename probe_signal<T>::sptr probe_signal<T>::make()
{
    return gnuradio::make_block_sptr<probe_signal_impl<T>>();
}

template <class T>
probe_signal_impl<T>::probe_signal_impl()
    : sync_block("probe_signal",
                 io_signature::make(1, 1, sizeof(T)),
                 io_signature::make(0, 0, 0)),
      d_level(0)
{
}

template <class T>
probe_signal_impl<T>::~probe_signal_impl()
{
}

template <class T>
int probe_signal_impl<T>::work(int noutput_items,
                               gr_vector_const_void_star& input_items,
                               gr_vector_void_star& output_items)
{
    const T* in = (const T*)input_items[0];

    gr::thread::scoped_lock guard(d_mutex);
    if (noutput_items > 0)
        d_level = in[noutput_items - 1];

    return noutput_items;
}

template class probe_signal<std::uint8_t>;
template class probe_signal<std::int16_t>;
template class probe_signal<std::int32_t>;
template class probe_signal<float>;
template class probe_signal<gr_complex>;

template <class T>
T probe_signal_impl<T>::level() const
{
    gr::thread::scoped_lock guard(d_mutex);
    return d_level;
}

} /* namespace blocks */
} /* namespace gr */

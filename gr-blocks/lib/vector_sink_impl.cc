/* -*- c++ -*- */
/*
 * Copyright 2004,2008,2010,2013,2017-2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "vector_sink_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/thread/thread.h>
#include <algorithm>

namespace gr {
namespace blocks {

template <class T>
typename vector_sink<T>::sptr vector_sink<T>::make(unsigned int vlen,
                                                   const int reserve_items)
{
    return gnuradio::make_block_sptr<vector_sink_impl<T>>(vlen, reserve_items);
}

template <class T>
vector_sink_impl<T>::vector_sink_impl(unsigned int vlen, const int reserve_items)
    : sync_block("vector_sink",
                 io_signature::make(1, 1, sizeof(T) * vlen),
                 io_signature::make(0, 0, 0)),
      d_vlen(vlen)
{
    gr::thread::scoped_lock guard(d_data_mutex);
    d_data.reserve(d_vlen * reserve_items);
}

template <class T>
vector_sink_impl<T>::~vector_sink_impl()
{
}

template <class T>
std::vector<T> vector_sink_impl<T>::data() const
{
    gr::thread::scoped_lock guard(d_data_mutex);
    return d_data;
}

template <class T>
std::vector<tag_t> vector_sink_impl<T>::tags() const
{
    gr::thread::scoped_lock guard(d_data_mutex);
    return d_tags;
}


template <class T>
void vector_sink_impl<T>::reset()
{
    gr::thread::scoped_lock guard(d_data_mutex);
    d_tags.clear();
    d_data.clear();
}

template <class T>
int vector_sink_impl<T>::work(int noutput_items,
                              gr_vector_const_void_star& input_items,
                              gr_vector_void_star& output_items)
{
    const T* iptr = (const T*)input_items[0];

    // can't touch this (as long as work() is working, the accessors shall not
    // read the data
    gr::thread::scoped_lock guard(d_data_mutex);
    for (unsigned int i = 0; i < noutput_items * d_vlen; i++)
        d_data.push_back(iptr[i]);
    std::vector<tag_t> tags;
    this->get_tags_in_range(
        tags, 0, this->nitems_read(0), this->nitems_read(0) + noutput_items);
    d_tags.insert(d_tags.end(), tags.begin(), tags.end());
    return noutput_items;
}

template class vector_sink<std::uint8_t>;
template class vector_sink<std::int16_t>;
template class vector_sink<std::int32_t>;
template class vector_sink<float>;
template class vector_sink<gr_complex>;
} /* namespace blocks */
} /* namespace gr */

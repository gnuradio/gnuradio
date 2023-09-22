/* -*- c++ -*- */
/*
 * Copyright 2014,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "tsb_vector_sink_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace blocks {

template <class T>
typename tsb_vector_sink<T>::sptr tsb_vector_sink<T>::make(unsigned int vlen,
                                                           const std::string& tsb_key)
{
    return gnuradio::make_block_sptr<tsb_vector_sink_impl<T>>(vlen, tsb_key);
}

template <class T>
tsb_vector_sink_impl<T>::tsb_vector_sink_impl(unsigned int vlen,
                                              const std::string& tsb_key)
    : gr::tagged_stream_block("tsb_vector_sink",
                              gr::io_signature::make(1, 1, vlen * sizeof(T)),
                              gr::io_signature::make(0, 0, 0),
                              tsb_key),
      d_vlen(vlen)
{
}

template <class T>
tsb_vector_sink_impl<T>::~tsb_vector_sink_impl()
{
}

template <class T>
std::vector<std::vector<T>> tsb_vector_sink_impl<T>::data() const
{
    return d_data;
}

template <class T>
std::vector<tag_t> tsb_vector_sink_impl<T>::tags() const
{
    return d_tags;
}

template <class T>
int tsb_vector_sink_impl<T>::work(int noutput_items,
                                  gr_vector_int& ninput_items,
                                  gr_vector_const_void_star& input_items,
                                  gr_vector_void_star& output_items)
{
    const T* in = (const T*)input_items[0];

    std::vector<T> new_data(in, in + (ninput_items[0] * d_vlen));
    d_data.push_back(new_data);

    std::vector<tag_t> tags;
    this->get_tags_in_window(tags, 0, 0, ninput_items[0]);
    d_tags.insert(d_tags.end(), tags.begin(), tags.end());

    return ninput_items[0];
}

template class tsb_vector_sink<std::uint8_t>;
template class tsb_vector_sink<std::int16_t>;
template class tsb_vector_sink<std::int32_t>;
template class tsb_vector_sink<float>;
template class tsb_vector_sink<gr_complex>;
} /* namespace blocks */
} /* namespace gr */

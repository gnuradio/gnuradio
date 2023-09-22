/* -*- c++ -*- */
/*
 * Copyright 2004,2008,2010,2013,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "vector_source_impl.h"
#include <gnuradio/io_signature.h>
#include <algorithm>
#include <stdexcept>

namespace gr {
namespace blocks {

template <class T>
typename vector_source<T>::sptr vector_source<T>::make(const std::vector<T>& data,
                                                       bool repeat,
                                                       unsigned int vlen,
                                                       const std::vector<tag_t>& tags)
{
    return gnuradio::make_block_sptr<vector_source_impl<T>>(data, repeat, vlen, tags);
}

template <class T>
vector_source_impl<T>::vector_source_impl(const std::vector<T>& data,
                                          bool repeat,
                                          unsigned int vlen,
                                          const std::vector<tag_t>& tags)
    : sync_block("vector_source",
                 io_signature::make(0, 0, 0),
                 io_signature::make(1, 1, sizeof(T) * vlen)),
      d_data(data),
      d_repeat(repeat),
      d_offset(0),
      d_vlen(vlen),
      d_tags(tags)
{
    if (tags.empty()) {
        d_settags = 0;
    } else {
        d_settags = 1;
        this->set_output_multiple(data.size() / vlen);
    }
    if ((data.size() % vlen) != 0)
        throw std::invalid_argument("data length must be a multiple of vlen");
}

template <class T>
vector_source_impl<T>::~vector_source_impl()
{
}

template <class T>
void vector_source_impl<T>::set_data(const std::vector<T>& data,
                                     const std::vector<tag_t>& tags)
{
    d_data = data;
    d_tags = tags;
    rewind();
    if (tags.empty()) {
        d_settags = false;
    } else {
        d_settags = true;
    }
}

template <class T>
int vector_source_impl<T>::work(int noutput_items,
                                gr_vector_const_void_star& input_items,
                                gr_vector_void_star& output_items)
{
    T* optr = (T*)output_items[0];

    if (d_repeat) {
        unsigned int size = d_data.size();
        unsigned int offset = d_offset;
        if (size == 0)
            return -1;

        if (d_settags) {
            int n_outputitems_per_vector = d_data.size() / d_vlen;
            for (int i = 0; i < noutput_items; i += n_outputitems_per_vector) {
                // FIXME do proper vector copy
                memcpy((void*)optr, (const void*)&d_data[0], size * sizeof(T));
                optr += size;
                for (unsigned t = 0; t < d_tags.size(); t++) {
                    this->add_item_tag(0,
                                       this->nitems_written(0) + i + d_tags[t].offset,
                                       d_tags[t].key,
                                       d_tags[t].value,
                                       d_tags[t].srcid);
                }
            }
        } else {
            for (int i = 0; i < static_cast<int>(noutput_items * d_vlen); i++) {
                optr[i] = d_data[offset++];
                if (offset >= size) {
                    offset = 0;
                }
            }
        }

        d_offset = offset;
        return noutput_items;
    } else {
        if (d_offset >= d_data.size())
            return -1; // Done!

        unsigned n = std::min((unsigned)d_data.size() - d_offset,
                              (unsigned)noutput_items * d_vlen);
        for (unsigned i = 0; i < n; i++) {
            optr[i] = d_data[d_offset + i];
        }
        for (unsigned t = 0; t < d_tags.size(); t++) {
            if ((d_tags[t].offset >= d_offset) && (d_tags[t].offset < d_offset + n))
                this->add_item_tag(
                    0, d_tags[t].offset, d_tags[t].key, d_tags[t].value, d_tags[t].srcid);
        }
        d_offset += n;
        return n / d_vlen;
    }
}

template class vector_source<std::uint8_t>;
template class vector_source<std::int16_t>;
template class vector_source<std::int32_t>;
template class vector_source<float>;
template class vector_source<gr_complex>;
} /* namespace blocks */
} /* namespace gr */

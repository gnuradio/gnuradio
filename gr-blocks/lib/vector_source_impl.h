/* -*- c++ -*- */
/*
 * Copyright 2004,2008,2012-2013,2018 Free Software Foundation, Inc.
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


#ifndef VECTOR_SOURCE_IMPL_H
#define VECTOR_SOURCE_IMPL_H

#include <gnuradio/blocks/vector_source.h>

namespace gr {
namespace blocks {

template <class T>
class vector_source_impl : public vector_source<T>
{
private:
    std::vector<T> d_data;
    bool d_repeat;
    unsigned int d_offset;
    unsigned int d_vlen;
    bool d_settags;
    std::vector<tag_t> d_tags;

public:
    vector_source_impl(const std::vector<T>& data,
                       bool repeat,
                       unsigned int vlen,
                       const std::vector<tag_t>& tags);
    ~vector_source_impl();

    void rewind() { d_offset = 0; }
    void set_data(const std::vector<T>& data, const std::vector<tag_t>& tags);
    void set_repeat(bool repeat) { d_repeat = repeat; };

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* VECTOR_SOURCE_IMPL_H */

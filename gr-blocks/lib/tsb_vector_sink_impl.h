/* -*- c++ -*- */
/*
 * Copyright 2014,2018 Free Software Foundation, Inc.
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


#ifndef TSB_VECTOR_SINK_IMPL_H
#define TSB_VECTOR_SINK_IMPL_H

#include <gnuradio/blocks/tsb_vector_sink.h>

namespace gr {
namespace blocks {

template <class T>
class tsb_vector_sink_impl : public tsb_vector_sink<T>
{
private:
    std::vector<std::vector<T>> d_data;
    std::vector<tag_t> d_tags;
    unsigned int d_vlen;

public:
    tsb_vector_sink_impl(unsigned int vlen, const std::string& tsb_key);
    ~tsb_vector_sink_impl();

    void reset() { d_data.clear(); }
    std::vector<std::vector<T>> data() const;
    std::vector<tag_t> tags() const;

    int work(int noutput_items,
             gr_vector_int& ninput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace blocks
} // namespace gr

#endif /* TSB_VECTOR_SINK_IMPL_H */

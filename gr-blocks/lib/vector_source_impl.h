/* -*- c++ -*- */
/*
 * Copyright 2004,2008,2012-2013,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
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
    const unsigned int d_vlen;
    bool d_settags;
    std::vector<tag_t> d_tags;

public:
    vector_source_impl(const std::vector<T>& data,
                       bool repeat,
                       unsigned int vlen,
                       const std::vector<tag_t>& tags);
    ~vector_source_impl() override;

    void rewind() override { d_offset = 0; }
    void set_data(const std::vector<T>& data, const std::vector<tag_t>& tags) override;
    void set_repeat(bool repeat) override { d_repeat = repeat; };

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* VECTOR_SOURCE_IMPL_H */

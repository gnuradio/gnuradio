/* -*- c++ -*- */
/*
 * Copyright 2012,2013,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef VECTOR_INSERT_IMPL_H
#define VECTOR_INSERT_IMPL_H

#include <gnuradio/blocks/vector_insert.h>

namespace gr {
namespace blocks {

template <class T>
class vector_insert_impl : public vector_insert<T>
{
private:
    std::vector<T> d_data;
    int d_offset;
    const int d_periodicity;

public:
    vector_insert_impl(const std::vector<T>& data, int periodicity, int offset);
    ~vector_insert_impl() override;

    void rewind() override { d_offset = 0; }
    void set_data(const std::vector<T>& data) override
    {
        d_data = data;
        rewind();
    }

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* VECTOR_INSERT_IMPL_H */

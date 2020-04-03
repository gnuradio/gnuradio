/* -*- c++ -*- */
/*
 * Copyright 2008,2013,2017-2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef MOVING_AVERAGE_IMPL_H
#define MOVING_AVERAGE_IMPL_H

#include <gnuradio/blocks/moving_average.h>
#include <algorithm>
#include <vector>

namespace gr {
namespace blocks {

template <class T>
class moving_average_impl : public moving_average<T>
{
private:
    int d_length;
    T d_scale;
    int d_max_iter;
    const unsigned int d_vlen;
    std::vector<T> d_sum;


    int d_new_length;
    T d_new_scale;
    bool d_updated;

public:
    moving_average_impl(int length, T scale, int max_iter = 4096, unsigned int vlen = 1);
    ~moving_average_impl();

    int length() const { return d_new_length; }
    T scale() const { return d_new_scale; }
    unsigned int vlen() const { return d_vlen; }

    void set_length_and_scale(int length, T scale);
    void set_length(int length);
    void set_scale(T scale);

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* MOVING_AVERAGE_IMPL_H */

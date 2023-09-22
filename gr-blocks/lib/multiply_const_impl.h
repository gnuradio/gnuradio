/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef MULTIPLY_CONST_IMPL_H
#define MULTIPLY_CONST_IMPL_H

#include <gnuradio/blocks/multiply_const.h>

namespace gr {
namespace blocks {

template <class T>
class BLOCKS_API multiply_const_impl : public multiply_const<T>
{
    T d_k;
    const size_t d_vlen;

public:
    multiply_const_impl(T k, size_t vlen);

    T k() const override { return d_k; }
    void set_k(T k) override { d_k = k; }

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* MULTIPLY_CONST_IMPL_H */

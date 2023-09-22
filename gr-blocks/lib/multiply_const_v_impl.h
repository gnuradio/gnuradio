/* -*- c++ -*- */
/*
 * Copyright 2004,2006,2010,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef MULTIPLY_CONST_V_IMPL_H
#define MULTIPLY_CONST_V_IMPL_H

#include <gnuradio/blocks/multiply_const_v.h>

namespace gr {
namespace blocks {

template <class T>
class BLOCKS_API multiply_const_v_impl : public multiply_const_v<T>
{
    std::vector<T> d_k;

public:
    multiply_const_v_impl(std::vector<T> k);

    std::vector<T> k() const override { return d_k; }
    void set_k(std::vector<T> k) override { d_k = k; }

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* MULTIPLY_CONST_V_IMPL_H */

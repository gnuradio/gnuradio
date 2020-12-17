/* -*- c++ -*- */
/*
 * Copyright 2004,2006,2010,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef ADD_CONST_V_IMPL_H
#define ADD_CONST_V_IMPL_H

#include <gnuradio/blocks/add_const_v.h>

namespace gr {
namespace blocks {

template <class T>
class BLOCKS_API add_const_v_impl : public add_const_v<T>
{
    std::vector<T> d_k;

public:
    add_const_v_impl(std::vector<T> k);

    std::vector<T> k() const override { return d_k; }
    void set_k(std::vector<T> k) override { d_k = k; }

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* ADD_CONST_V_IMPL_H */

/* -*- c++ -*- */
/*
 * Copyright 2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef AND_CONST_IMPL_H
#define AND_CONST_IMPL_H

#include <gnuradio/blocks/and_const.h>

namespace gr {
namespace blocks {

template <class T>
class BLOCKS_API and_const_impl : public and_const<T>
{
    T d_k;

public:
    and_const_impl(T k);

    T k() const override { return d_k; }
    void set_k(T k) override { d_k = k; }

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* AND_CONST_IMPL_H */

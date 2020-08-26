/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef SUB_IMPL_H
#define SUB_IMPL_H

#include <gnuradio/blocks/sub.h>

namespace gr {
namespace blocks {

template <class T>
class BLOCKS_API sub_impl : public sub<T>
{
    const size_t d_vlen;

public:
    sub_impl(size_t vlen);

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* SUB_IMPL_H */

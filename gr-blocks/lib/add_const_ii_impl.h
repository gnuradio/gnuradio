/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2012,2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef ADD_CONST_II_IMPL
#define ADD_CONST_II_IMPL

#include <gnuradio/blocks/add_const_ii.h>

namespace gr {
namespace blocks {

class BLOCKS_API add_const_ii_impl : public add_const_ii
{
private:
    int d_k;

public:
    add_const_ii_impl(int k);

    void setup_rpc() override;

    int k() const override { return d_k; }
    void set_k(int k) override { d_k = k; }

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* ADD_CONST_II_IMPL */

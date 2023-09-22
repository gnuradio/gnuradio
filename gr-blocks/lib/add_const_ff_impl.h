/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2012,2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef ADD_CONST_FF_IMPL
#define ADD_CONST_FF_IMPL

#include <gnuradio/blocks/add_const_ff.h>

namespace gr {
namespace blocks {

class BLOCKS_API add_const_ff_impl : public add_const_ff
{
private:
    float d_k;
    volk::vector<float> d_k_copy;

public:
    add_const_ff_impl(float k);

    void setup_rpc() override;

    float k() const override { return d_k; }

    void set_k(float k) override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* ADD_CONST_FF_IMPL */

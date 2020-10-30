/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2012,2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef ADD_CONST_CC_IMPL
#define ADD_CONST_CC_IMPL

#include <gnuradio/blocks/add_const_cc.h>

namespace gr {
namespace blocks {

class BLOCKS_API add_const_cc_impl : public add_const_cc
{
private:
    gr_complex d_k;
    volk::vector<gr_complex> d_k_copy;

public:
    add_const_cc_impl(gr_complex k);

    void setup_rpc() override;

    gr_complex k() const override { return d_k; }

    void set_k(gr_complex k) override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* ADD_CONST_CC_IMPL */

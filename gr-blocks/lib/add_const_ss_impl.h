/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2012,2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef ADD_CONST_SS_IMPL
#define ADD_CONST_SS_IMPL

#include <gnuradio/blocks/add_const_ss.h>

namespace gr {
namespace blocks {

class BLOCKS_API add_const_ss_impl : public add_const_ss
{
private:
    short d_k;

public:
    add_const_ss_impl(short k);
    ~add_const_ss_impl();

    void setup_rpc();

    short k() const { return d_k; }
    void set_k(short k) { d_k = k; }

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* ADD_CONST_SS_IMPL */

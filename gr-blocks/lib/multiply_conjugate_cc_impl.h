/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_MULTIPLY_CONJUGATE_CC_IMPL_H
#define INCLUDED_BLOCKS_MULTIPLY_CONJUGATE_CC_IMPL_H

#include <gnuradio/blocks/multiply_conjugate_cc.h>

namespace gr {
namespace blocks {

class BLOCKS_API multiply_conjugate_cc_impl : public multiply_conjugate_cc
{
    const size_t d_vlen;

public:
    multiply_conjugate_cc_impl(size_t vlen);

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */


#endif /* INCLUDED_BLOCKS_MULTIPLY_CONJUGATE_CC_IMPL_H */

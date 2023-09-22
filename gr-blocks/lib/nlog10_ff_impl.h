/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_NLOG10_FF_IMPL_H
#define INCLUDED_NLOG10_FF_IMPL_H

#include <gnuradio/blocks/nlog10_ff.h>

namespace gr {
namespace blocks {

class BLOCKS_API nlog10_ff_impl : public nlog10_ff
{
    const float d_n_log2_10;
    const float d_10_k_n;
    const size_t d_vlen;

public:
    nlog10_ff_impl(float n, size_t vlen, float k);

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */


#endif /* INCLUDED_NLOG10_FF_IMPL_H */

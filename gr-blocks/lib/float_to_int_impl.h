/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FLOAT_TO_INT_IMPL_H
#define INCLUDED_FLOAT_TO_INT_IMPL_H

#include <gnuradio/blocks/float_to_int.h>

namespace gr {
namespace blocks {

class BLOCKS_API float_to_int_impl : public float_to_int
{
    const size_t d_vlen;
    float d_scale;

public:
    float_to_int_impl(size_t vlen, float scale);

    float scale() const override { return d_scale; }
    void set_scale(float scale) override { d_scale = scale; }

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */


#endif /* INCLUDED_FLOAT_TO_INT_IMPL_H */

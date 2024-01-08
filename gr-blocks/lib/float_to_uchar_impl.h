/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 * Copyright 2024 Daniel Estevez <daniel@destevez.net>
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FLOAT_TO_UCHAR_IMPL_H
#define INCLUDED_FLOAT_TO_UCHAR_IMPL_H

#include <gnuradio/blocks/float_to_uchar.h>

namespace gr {
namespace blocks {

class BLOCKS_API float_to_uchar_impl : public float_to_uchar
{
private:
    const size_t d_vlen;
    float d_scale;
    float d_bias;

public:
    float_to_uchar_impl(size_t vlen, float scale, float bias);

    float scale() const override { return d_scale; }
    float bias() const override { return d_bias; }
    void set_scale(float scale) override { d_scale = scale; }
    void set_bias(float bias) override { d_bias = bias; }

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */


#endif /* INCLUDED_FLOAT_TO_UCHAR_IMPL_H */

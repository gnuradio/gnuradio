/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_CHAR_TO_FLOAT_IMPL_H
#define INCLUDED_CHAR_TO_FLOAT_IMPL_H

#include <gnuradio/blocks/char_to_float.h>

namespace gr {
namespace blocks {

class BLOCKS_API char_to_float_impl : public char_to_float
{
    const size_t d_vlen;
    float d_scale;

public:
    char_to_float_impl(size_t vlen, float scale);

    float scale() const override { return d_scale; }
    void set_scale(float scale) override { d_scale = scale; }

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */


#endif /* INCLUDED_CHAR_TO_FLOAT_IMPL_H */

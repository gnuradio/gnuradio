/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_COMPLEX_TO_INTERLEAVED_SHORT_IMPL_H
#define INCLUDED_COMPLEX_TO_INTERLEAVED_SHORT_IMPL_H

#include <gnuradio/blocks/complex_to_interleaved_short.h>

namespace gr {
namespace blocks {

class BLOCKS_API complex_to_interleaved_short_impl : public complex_to_interleaved_short
{
private:
    float d_scalar;
    const bool d_vector;

public:
    complex_to_interleaved_short_impl(bool vector, float scale_factor);

    void set_scale_factor(float new_value) override { d_scalar = new_value; };

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */


#endif /* INCLUDED_COMPLEX_TO_INTERLEAVED_SHORT_IMPL_H */

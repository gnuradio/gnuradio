/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FLOAT_TO_COMPLEX_IMPL_H
#define INCLUDED_FLOAT_TO_COMPLEX_IMPL_H

#include <gnuradio/blocks/float_to_complex.h>

namespace gr {
namespace blocks {

class BLOCKS_API float_to_complex_impl : public float_to_complex
{
    const size_t d_vlen;

public:
    float_to_complex_impl(size_t vlen);

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */


#endif /* INCLUDED_FLOAT_TO_COMPLEX_IMPL_H */

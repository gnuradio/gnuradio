/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_COMPLEX_TO_REAL_IMPL_H
#define INCLUDED_COMPLEX_TO_REAL_IMPL_H

#include <gnuradio/blocks/complex_to_real.h>

namespace gr {
namespace blocks {

class BLOCKS_API complex_to_real_impl : public complex_to_real
{
    const size_t d_vlen;

public:
    complex_to_real_impl(size_t vlen);

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */


#endif /* INCLUDED_COMPLEX_TO_REAL_IMPL_H */

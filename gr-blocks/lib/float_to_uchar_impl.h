/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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
public:
    float_to_uchar_impl();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace blocks */
} /* namespace gr */


#endif /* INCLUDED_FLOAT_TO_UCHAR_IMPL_H */

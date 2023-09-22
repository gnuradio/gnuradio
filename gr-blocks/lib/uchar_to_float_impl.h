/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_UCHAR_TO_FLOAT_IMPL_H
#define INCLUDED_UCHAR_TO_FLOAT_IMPL_H

#include <gnuradio/blocks/uchar_to_float.h>

namespace gr {
namespace blocks {

class BLOCKS_API uchar_to_float_impl : public uchar_to_float
{
public:
    uchar_to_float_impl();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */


#endif /* INCLUDED_UCHAR_TO_FLOAT_IMPL_H */

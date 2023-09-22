/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_CONJUGATE_CC_IMPL_H
#define INCLUDED_CONJUGATE_CC_IMPL_H

#include <gnuradio/blocks/conjugate_cc.h>

namespace gr {
namespace blocks {

class BLOCKS_API conjugate_cc_impl : public conjugate_cc
{
public:
    conjugate_cc_impl();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_CONJUGATE_CC_IMPL_H */

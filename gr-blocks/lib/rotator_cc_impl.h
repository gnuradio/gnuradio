/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_ROTATOR_CC_IMPL_H
#define INCLUDED_BLOCKS_ROTATOR_CC_IMPL_H

#include <gnuradio/blocks/rotator.h>
#include <gnuradio/blocks/rotator_cc.h>

namespace gr {
namespace blocks {

/*!
 * \brief Complex rotator
 * \ingroup math_blk
 */
class rotator_cc_impl : public rotator_cc
{
private:
    rotator d_r;

public:
    rotator_cc_impl(double phase_inc = 0.0);
    ~rotator_cc_impl();

    void set_phase_inc(double phase_inc);

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_ROTATOR_CC_IMPL_H */

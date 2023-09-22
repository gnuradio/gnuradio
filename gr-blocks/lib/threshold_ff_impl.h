/* -*- c++ -*- */
/*
 * Copyright 2004,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_THRESHOLD_FF_IMPL_H
#define INCLUDED_GR_THRESHOLD_FF_IMPL_H

#include <gnuradio/blocks/threshold_ff.h>

namespace gr {
namespace blocks {

class threshold_ff_impl : public threshold_ff
{
private:
    float d_lo, d_hi; // the constant
    float d_last_state;

public:
    threshold_ff_impl(float lo, float hi, float initial_state = 0);
    ~threshold_ff_impl() override;

    float lo() const override { return d_lo; }
    void set_lo(float lo) override { d_lo = lo; }
    float hi() const override { return d_hi; }
    void set_hi(float hi) override { d_hi = hi; }
    float last_state() const override { return d_last_state; }
    void set_last_state(float last_state) override { d_last_state = last_state; }

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_THRESHOLD_FF_IMPL_H */

/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_TEST_TAG_VARIABLE_RATE_FF_IMPL_H
#define INCLUDED_GR_TEST_TAG_VARIABLE_RATE_FF_IMPL_H

#include <gnuradio/blocks/test_tag_variable_rate_ff.h>

namespace gr {
namespace blocks {

class test_tag_variable_rate_ff_impl : public test_tag_variable_rate_ff
{
protected:
    const bool d_update_once;
    const double d_update_step;
    double d_accum;
    double d_rrate;
    uint64_t d_old_in, d_new_in, d_last_out;
    uint64_t d_rng_state[2];

public:
    test_tag_variable_rate_ff_impl(bool update_once = false, double update_step = 0.001);
    ~test_tag_variable_rate_ff_impl() override;

    // void setup_rpc();

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_TEST_TAG_VARIABLE_RATE_FF_IMPL_H */

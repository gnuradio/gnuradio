/* -*- c++ -*- */
/*
 * Copyright 2011,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef INCLUDED_FILTER_DC_BLOCKER_FF_IMPL_H
#define INCLUDED_FILTER_DC_BLOCKER_FF_IMPL_H

#include <gnuradio/filter/dc_blocker_ff.h>
#include <deque>

namespace gr {
namespace filter {

class moving_averager_f
{
public:
    moving_averager_f(int D);

    float filter(float x);
    float delayed_sig() { return d_out; }

private:
    int d_length;
    float d_out, d_out_d1, d_out_d2;
    std::deque<float> d_delay_line;
};

class FILTER_API dc_blocker_ff_impl : public dc_blocker_ff
{
private:
    int d_length;
    bool d_long_form;
    moving_averager_f d_ma_0;
    moving_averager_f d_ma_1;
    std::unique_ptr<moving_averager_f> d_ma_2;
    std::unique_ptr<moving_averager_f> d_ma_3;
    std::deque<float> d_delay_line;

public:
    dc_blocker_ff_impl(int D, bool long_form);

    int group_delay() override;

    // int set_length(int D);

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_FILTER_DC_BLOCKER_FF_IMPL_H */

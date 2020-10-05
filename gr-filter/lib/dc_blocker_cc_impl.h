/* -*- c++ -*- */
/*
 * Copyright 2011,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef INCLUDED_FILTER_DC_BLOCKER_CC_IMPL_H
#define INCLUDED_FILTER_DC_BLOCKER_CC_IMPL_H

#include <gnuradio/filter/dc_blocker_cc.h>
#include <deque>

namespace gr {
namespace filter {

class moving_averager_c
{
public:
    moving_averager_c(int D);

    gr_complex filter(gr_complex x);
    gr_complex delayed_sig() { return d_out; }

private:
    const int d_length;
    gr_complex d_out, d_out_d1, d_out_d2;
    std::deque<gr_complex> d_delay_line;
};

class FILTER_API dc_blocker_cc_impl : public dc_blocker_cc
{
private:
    int d_length;
    bool d_long_form;
    moving_averager_c d_ma_0;
    moving_averager_c d_ma_1;
    std::unique_ptr<moving_averager_c> d_ma_2;
    std::unique_ptr<moving_averager_c> d_ma_3;
    std::deque<gr_complex> d_delay_line;

public:
    dc_blocker_cc_impl(int D, bool long_form);

    int group_delay() override;

    // int set_length(int D);

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_FILTER_DC_BLOCKER_CC_IMPL_H */

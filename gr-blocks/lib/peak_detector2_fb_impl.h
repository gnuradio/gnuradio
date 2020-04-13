/* -*- c++ -*- */
/*
 * Copyright 2007,2013,2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_PEAK_DETECTOR2_FB_IMPL_H
#define INCLUDED_GR_PEAK_DETECTOR2_FB_IMPL_H

#include <gnuradio/blocks/peak_detector2_fb.h>

namespace gr {
namespace blocks {

class peak_detector2_fb_impl : public peak_detector2_fb
{
private:
    float d_threshold_factor_rise;
    int d_look_ahead;
    float d_alpha;
    float d_avg;
    bool d_found;

    void invalidate();

public:
    peak_detector2_fb_impl(float threshold_factor_rise, int look_ahead, float alpha);
    ~peak_detector2_fb_impl() override;

    void set_threshold_factor_rise(float thr) override;
    void set_look_ahead(int look) override;
    void set_alpha(float alpha) override;

    float threshold_factor_rise() override { return d_threshold_factor_rise; }
    int look_ahead() override { return d_look_ahead; }
    float alpha() override { return d_alpha; }

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_PEAK_DETECTOR2_FB_IMPL_H */

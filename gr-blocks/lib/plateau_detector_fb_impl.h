/* -*- c++ -*- */
/*
 * Copyright 2012-2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_PLATEAU_DETECTOR_FB_IMPL_H
#define INCLUDED_BLOCKS_PLATEAU_DETECTOR_FB_IMPL_H

#include <gnuradio/blocks/plateau_detector_fb.h>

namespace gr {
namespace blocks {

class plateau_detector_fb_impl : public plateau_detector_fb
{
private:
    const int d_max_len;
    float d_threshold;

public:
    plateau_detector_fb_impl(int max_len, float threshold);
    ~plateau_detector_fb_impl() override;

    void forecast(int noutput_items, gr_vector_int& ninput_items_required) override;

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;

    void set_threshold(float threshold) override;
    float threshold() const override;
};

} // namespace blocks
} // namespace gr

#endif /* INCLUDED_BLOCKS_PLATEAU_DETECTOR_FB_IMPL_H */

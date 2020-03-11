/* -*- c++ -*- */
/*
 * Copyright 2007,2013,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef PEAK_DETECTOR_IMPL_H
#define PEAK_DETECTOR_IMPL_H

#include <gnuradio/blocks/peak_detector.h>

namespace gr {
namespace blocks {

template <class T>
class peak_detector_impl : public peak_detector<T>
{
private:
    float d_threshold_factor_rise;
    float d_threshold_factor_fall;
    int d_look_ahead;
    float d_avg_alpha;
    float d_avg;

public:
    peak_detector_impl(float threshold_factor_rise,
                       float threshold_factor_fall,
                       int look_ahead,
                       float alpha);
    ~peak_detector_impl();

    void set_threshold_factor_rise(float thr) { d_threshold_factor_rise = thr; }
    void set_threshold_factor_fall(float thr) { d_threshold_factor_fall = thr; }
    void set_look_ahead(int look) { d_look_ahead = look; }
    void set_alpha(float alpha) { d_avg_alpha = alpha; }
    float threshold_factor_rise() { return d_threshold_factor_rise; }
    float threshold_factor_fall() { return d_threshold_factor_fall; }
    int look_ahead() { return d_look_ahead; }
    float alpha() { return d_avg_alpha; }

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* PEAK_DETECTOR_IMPL_H */

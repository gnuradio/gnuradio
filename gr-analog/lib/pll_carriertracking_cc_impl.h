/* -*- c++ -*- */
/*
 * Copyright 2004,2006,2011,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ANALOG_PLL_CARRIERTRACKING_CC_IMPL_H
#define INCLUDED_ANALOG_PLL_CARRIERTRACKING_CC_IMPL_H

#include <gnuradio/analog/pll_carriertracking_cc.h>
#include <gnuradio/math.h>

namespace gr {
namespace analog {

class pll_carriertracking_cc_impl : public pll_carriertracking_cc
{
private:
    float d_locksig, d_lock_threshold;
    bool d_squelch_enable;

    float mod_2pi(float in)
    {
        if (in > GR_M_PI)
            return in - (2.0 * GR_M_PI);
        else if (in < -GR_M_PI)
            return in + (2.0 * GR_M_PI);
        else
            return in;
    }

    float phase_detector(gr_complex sample, float ref_phase)
    {
        float sample_phase;
        //  sample_phase = atan2(sample.imag(),sample.real());
        sample_phase = gr::fast_atan2f(sample.imag(), sample.real());
        return mod_2pi(sample_phase - ref_phase);
    }

public:
    pll_carriertracking_cc_impl(float loop_bw, float max_freq, float min_freq);
    ~pll_carriertracking_cc_impl() override;

    bool lock_detector(void) override { return (fabsf(d_locksig) > d_lock_threshold); }

    bool squelch_enable(bool) override;
    float set_lock_threshold(float) override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_PLL_CARRIERTRACKING_CC_IMPL_H */

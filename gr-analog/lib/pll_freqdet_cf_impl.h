/* -*- c++ -*- */
/*
 * Copyright 2004,2011,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ANALOG_PLL_FREQDET_CF_IMPL_H
#define INCLUDED_ANALOG_PLL_FREQDET_CF_IMPL_H

#include <gnuradio/analog/pll_freqdet_cf.h>
#include <gnuradio/math.h>

namespace gr {
namespace analog {

class pll_freqdet_cf_impl : public pll_freqdet_cf
{
private:
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
        sample_phase = gr::fast_atan2f(sample.imag(), sample.real());
        return mod_2pi(sample_phase - ref_phase);
    }


public:
    pll_freqdet_cf_impl(float loop_bw, float max_freq, float min_freq);
    ~pll_freqdet_cf_impl();

    void set_loop_bandwidth(float bw);
    void set_damping_factor(float df);
    void set_alpha(float alpha);
    void set_beta(float beta);
    void set_frequency(float freq);
    void set_phase(float phase);
    void set_min_freq(float freq);
    void set_max_freq(float freq);

    float get_loop_bandwidth() const;
    float get_damping_factor() const;
    float get_alpha() const;
    float get_beta() const;
    float get_frequency() const;
    float get_phase() const;
    float get_min_freq() const;
    float get_max_freq() const;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_PLL_FREQDET_CF_IMPL_H */

/* -*- c++ -*- */
/*
 * Copyright 2002,2004,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_FXPT_NCO_H
#define INCLUDED_GR_FXPT_NCO_H

#include <gnuradio/api.h>
#include <gnuradio/fxpt.h>
#include <gnuradio/gr_complex.h>
#include <cstdint>

namespace gr {

/*!
 * \brief Numerically Controlled Oscillator (NCO)
 * \ingroup misc
 */
class /*GR_RUNTIME_API*/ fxpt_nco
{
    uint32_t d_phase;
    int32_t d_phase_inc;

public:
    fxpt_nco() : d_phase(0), d_phase_inc(0) {}

    ~fxpt_nco() {}

    // radians
    void set_phase(float angle) { d_phase = gr::fxpt::float_to_fixed(angle); }

    void adjust_phase(float delta_phase)
    {
        d_phase += gr::fxpt::float_to_fixed(delta_phase);
    }

    // angle_rate is in radians / step
    void set_freq(float angle_rate)
    {
        d_phase_inc = gr::fxpt::float_to_fixed(angle_rate);
    }

    // angle_rate is a delta in radians / step
    void adjust_freq(float delta_angle_rate)
    {
        d_phase_inc += (uint32_t)gr::fxpt::float_to_fixed(delta_angle_rate);
    }

    // increment current phase angle

    void step() { d_phase += d_phase_inc; }

    void step(int n) { d_phase += (uint32_t)d_phase_inc * n; }

    // units are radians / step
    float get_phase() const { return gr::fxpt::fixed_to_float(d_phase); }
    float get_freq() const { return gr::fxpt::fixed_to_float(d_phase_inc); }

    // compute sin and cos for current phase angle
    void sincos(float* sinx, float* cosx) const
    {
        *sinx = gr::fxpt::sin(d_phase);
        *cosx = gr::fxpt::cos(d_phase);
    }

    // compute cos and sin for a block of phase angles
    void sincos(gr_complex* output, int noutput_items, double ampl = 1.0)
    {
        for (int i = 0; i < noutput_items; i++) {
            output[i] =
                gr_complex(gr::fxpt::cos(d_phase) * ampl, gr::fxpt::sin(d_phase) * ampl);
            step();
        }
    }

    // compute sin for a block of phase angles
    void sin(float* output, int noutput_items, double ampl = 1.0)
    {
        for (int i = 0; i < noutput_items; i++) {
            output[i] = (float)(gr::fxpt::sin(d_phase) * ampl);
            step();
        }
    }

    // compute cos for a block of phase angles
    void cos(float* output, int noutput_items, double ampl = 1.0)
    {
        for (int i = 0; i < noutput_items; i++) {
            output[i] = (float)(gr::fxpt::cos(d_phase) * ampl);
            step();
        }
    }

    // compute sin for a block of phase angles
    void sin(std::int8_t* output, int noutput_items, double ampl = 1.0)
    {
        for (int i = 0; i < noutput_items; i++) {
            output[i] = (std::int8_t)(gr::fxpt::sin(d_phase) * ampl);
            step();
        }
    }

    // compute cos for a block of phase angles
    void cos(std::int8_t* output, int noutput_items, double ampl = 1.0)
    {
        for (int i = 0; i < noutput_items; i++) {
            output[i] = (std::int8_t)(gr::fxpt::cos(d_phase) * ampl);
            step();
        }
    }

    // compute sin for a block of phase angles
    void sin(short* output, int noutput_items, double ampl = 1.0)
    {
        for (int i = 0; i < noutput_items; i++) {
            output[i] = (short)(gr::fxpt::sin(d_phase) * ampl);
            step();
        }
    }

    // compute cos for a block of phase angles
    void cos(short* output, int noutput_items, double ampl = 1.0)
    {
        for (int i = 0; i < noutput_items; i++) {
            output[i] = (short)(gr::fxpt::cos(d_phase) * ampl);
            step();
        }
    }

    // compute sin for a block of phase angles
    void sin(int* output, int noutput_items, double ampl = 1.0)
    {
        for (int i = 0; i < noutput_items; i++) {
            output[i] = (int)(gr::fxpt::sin(d_phase) * ampl);
            step();
        }
    }

    // compute cos for a block of phase angles
    void cos(int* output, int noutput_items, double ampl = 1.0)
    {
        for (int i = 0; i < noutput_items; i++) {
            output[i] = (int)(gr::fxpt::cos(d_phase) * ampl);
            step();
        }
    }

    // compute cos or sin for current phase angle
    float cos() const { return gr::fxpt::cos(d_phase); }
    float sin() const { return gr::fxpt::sin(d_phase); }
};

} /* namespace gr */

#endif /* INCLUDED_GR_FXPT_NCO_H */

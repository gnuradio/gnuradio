/* -*- c++ -*- */
/*
 * Copyright 2002,2004,2005,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_FXPT_VCO_H
#define INCLUDED_GR_FXPT_VCO_H

#include <gnuradio/api.h>
#include <gnuradio/fxpt.h>
#include <gnuradio/gr_complex.h>

namespace gr {

/*!
 * \brief Voltage Controlled Oscillator (VCO)
 * \ingroup misc
 */
class /*GR_RUNTIME_API*/ fxpt_vco
{
    int32_t d_phase;

public:
    fxpt_vco() : d_phase(0) {}

    ~fxpt_vco() {}

    // radians
    void set_phase(float angle) { d_phase = fxpt::float_to_fixed(angle); }

    void adjust_phase(float delta_phase)
    {
        d_phase += (uint32_t)fxpt::float_to_fixed(delta_phase);
    }

    float get_phase() const { return fxpt::fixed_to_float(d_phase); }

    // compute sin and cos for current phase angle
    void sincos(float* sinx, float* cosx) const
    {
        *sinx = fxpt::sin(d_phase);
        *cosx = fxpt::cos(d_phase);
    }

    // compute complex sine a block at a time
    void sincos(gr_complex* output,
                const float* input,
                int noutput_items,
                float k,
                float ampl = 1.0)
    {
        for (int i = 0; i < noutput_items; i++) {
            output[i] = gr_complex((float)(fxpt::cos(d_phase) * ampl),
                                   (float)(fxpt::sin(d_phase) * ampl));
            adjust_phase(input[i] * k);
        }
    }

    // compute a block at a time
    void
    cos(float* output, const float* input, int noutput_items, float k, float ampl = 1.0)
    {
        for (int i = 0; i < noutput_items; i++) {
            output[i] = (float)(fxpt::cos(d_phase) * ampl);
            adjust_phase(input[i] * k);
        }
    }

    // compute cos or sin for current phase angle
    float cos() const { return fxpt::cos(d_phase); }
    float sin() const { return fxpt::sin(d_phase); }
};

} /* namespace gr */

#endif /* INCLUDED_GR_FXPT_VCO_H */

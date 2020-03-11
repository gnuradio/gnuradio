/* -*- c++ -*- */
/*
 * Copyright 2005,2013,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef _GR_VCO_H_
#define _GR_VCO_H_

#include <gnuradio/gr_complex.h>
#include <gnuradio/math.h>
#include <gnuradio/sincos.h>

#include <cmath>
#include <vector>

namespace gr {

/*!
 * \brief base class template for Voltage Controlled Oscillator (VCO)
 * \ingroup misc
 */
template <class o_type, class i_type>
class vco
{
public:
    vco() : d_phase(0) {}

    virtual ~vco() {}

    // radians
    void set_phase(double angle) { d_phase = angle; }

    void adjust_phase(double delta_phase)
    {
        d_phase += delta_phase;
        if (fabs(d_phase) > GR_M_PI) {

            while (d_phase > GR_M_PI)
                d_phase -= 2 * GR_M_PI;

            while (d_phase < -GR_M_PI)
                d_phase += 2 * GR_M_PI;
        }
    }

    double get_phase() const { return d_phase; }

    // compute sin and cos for current phase angle
    void sincos(float* sinx, float* cosx) const;

    void sincos(gr_complex* output,
                const float* input,
                int noutput_items,
                double k,
                double ampl = 1.0);

    // compute cos or sin for current phase angle
    float cos() const { return std::cos(d_phase); }
    float sin() const { return std::sin(d_phase); }

    // compute a block at a time
    void cos(float* output,
             const float* input,
             int noutput_items,
             double k,
             double ampl = 1.0);

protected:
    double d_phase;
};

template <class o_type, class i_type>
void vco<o_type, i_type>::sincos(float* sinx, float* cosx) const
{
    gr::sincosf(d_phase, sinx, cosx);
}

template <class o_type, class i_type>
void vco<o_type, i_type>::sincos(
    gr_complex* output, const float* input, int noutput_items, double k, double ampl)
{
    for (int i = 0; i < noutput_items; i++) {
        output[i] = gr_complex(cos() * ampl, sin() * ampl);
        adjust_phase(input[i] * k);
    }
}

template <class o_type, class i_type>
void vco<o_type, i_type>::cos(
    float* output, const float* input, int noutput_items, double k, double ampl)
{
    for (int i = 0; i < noutput_items; i++) {
        output[i] = cos() * ampl;
        adjust_phase(input[i] * k);
    }
}

} /* namespace gr */

#endif /* _GR_VCO_H_ */

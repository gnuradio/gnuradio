/* -*- c++ -*- */
/*
 * Copyright 2002,2013,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef _GR_NCO_H_
#define _GR_NCO_H_

#include <gnuradio/gr_complex.h>
#include <gnuradio/math.h>
#include <gnuradio/sincos.h>

#include <cmath>
#include <vector>

namespace gr {

/*!
 * \brief base class template for Numerically Controlled Oscillator (NCO)
 * \ingroup misc
 */
template <class o_type, class i_type>
class nco
{
public:
    nco() : phase(0), phase_inc(0) {}

    virtual ~nco() {}

    // radians
    void set_phase(double angle) { phase = angle; }

    void adjust_phase(double delta_phase) { phase += delta_phase; }

    // angle_rate is in radians / step
    void set_freq(double angle_rate) { phase_inc = angle_rate; }

    // angle_rate is a delta in radians / step
    void adjust_freq(double delta_angle_rate) { phase_inc += delta_angle_rate; }

    // increment current phase angle
    void step(int n = 1)
    {
        phase += phase_inc * n;
        if (fabs(phase) > GR_M_PI) {
            while (phase > GR_M_PI)
                phase -= 2 * GR_M_PI;

            while (phase < -GR_M_PI)
                phase += 2 * GR_M_PI;
        }
    }

    // units are radians / step
    double get_phase() const { return phase; }
    double get_freq() const { return phase_inc; }

    // compute sin and cos for current phase angle
    void sincos(float* sinx, float* cosx) const { gr::sincosf(phase, sinx, cosx); }

    // compute cos or sin for current phase angle
    float cos() const { return std::cos(phase); }
    float sin() const { return std::sin(phase); }

    // compute a block at a time
    void sin(float* output, int noutput_items, double ampl = 1.0);
    void cos(float* output, int noutput_items, double ampl = 1.0);
    void sincos(gr_complex* output, int noutput_items, double ampl = 1.0);
    void sin(short* output, int noutput_items, double ampl = 1.0);
    void cos(short* output, int noutput_items, double ampl = 1.0);
    void sin(int* output, int noutput_items, double ampl = 1.0);
    void cos(int* output, int noutput_items, double ampl = 1.0);

protected:
    double phase;
    double phase_inc;
};

template <class o_type, class i_type>
void nco<o_type, i_type>::sin(float* output, int noutput_items, double ampl)
{
    for (int i = 0; i < noutput_items; i++) {
        output[i] = (float)(sin() * ampl);
        step();
    }
}

template <class o_type, class i_type>
void nco<o_type, i_type>::cos(float* output, int noutput_items, double ampl)
{
    for (int i = 0; i < noutput_items; i++) {
        output[i] = (float)(cos() * ampl);
        step();
    }
}

template <class o_type, class i_type>
void nco<o_type, i_type>::sin(short* output, int noutput_items, double ampl)
{
    for (int i = 0; i < noutput_items; i++) {
        output[i] = (short)(sin() * ampl);
        step();
    }
}

template <class o_type, class i_type>
void nco<o_type, i_type>::cos(short* output, int noutput_items, double ampl)
{
    for (int i = 0; i < noutput_items; i++) {
        output[i] = (short)(cos() * ampl);
        step();
    }
}

template <class o_type, class i_type>
void nco<o_type, i_type>::sin(int* output, int noutput_items, double ampl)
{
    for (int i = 0; i < noutput_items; i++) {
        output[i] = (int)(sin() * ampl);
        step();
    }
}

template <class o_type, class i_type>
void nco<o_type, i_type>::cos(int* output, int noutput_items, double ampl)
{
    for (int i = 0; i < noutput_items; i++) {
        output[i] = (int)(cos() * ampl);
        step();
    }
}

template <class o_type, class i_type>
void nco<o_type, i_type>::sincos(gr_complex* output, int noutput_items, double ampl)
{
    for (int i = 0; i < noutput_items; i++) {
        float cosx, sinx;
        nco::sincos(&sinx, &cosx);
        output[i] = gr_complex(cosx * ampl, sinx * ampl);
        step();
    }
}

} /* namespace gr */

#endif /* _NCO_H_ */

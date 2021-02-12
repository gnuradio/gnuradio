/* -*- c++ -*- */
/*
 * Copyright 2011,2013,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/blocks/control_loop.h>
#include <gnuradio/math.h>
#include <stdexcept>

namespace gr {
namespace blocks {

#define M_TWOPI (2.0f * GR_M_PI)

control_loop::control_loop(float loop_bw, float max_freq, float min_freq)
    : d_phase(0), d_freq(0), d_max_freq(max_freq), d_min_freq(min_freq)
{
    // Set the damping factor for a critically damped system
    d_damping = sqrtf(2.0f) / 2.0f;

    // Set the bandwidth, which will then call update_gains()
    set_loop_bandwidth(loop_bw);
}

control_loop::~control_loop() {}

void control_loop::update_gains()
{
    float denom = (1.0 + 2.0 * d_damping * d_loop_bw + d_loop_bw * d_loop_bw);
    d_alpha = (4 * d_damping * d_loop_bw) / denom;
    d_beta = (4 * d_loop_bw * d_loop_bw) / denom;
}

/*******************************************************************
 * SET FUNCTIONS
 *******************************************************************/

void control_loop::set_loop_bandwidth(float bw)
{
    if (bw < 0) {
        throw std::out_of_range("control_loop: invalid bandwidth. Must be >= 0.");
    }

    d_loop_bw = bw;
    update_gains();
}

void control_loop::set_damping_factor(float df)
{
    if (df <= 0) {
        throw std::out_of_range("control_loop: invalid damping factor. Must be > 0.");
    }

    d_damping = df;
    update_gains();
}

void control_loop::set_alpha(float alpha)
{
    if (alpha < 0 || alpha > 1.0) {
        throw std::out_of_range("control_loop: invalid alpha. Must be in [0,1].");
    }
    d_alpha = alpha;
}

void control_loop::set_beta(float beta)
{
    if (beta < 0 || beta > 1.0) {
        throw std::out_of_range("control_loop: invalid beta. Must be in [0,1].");
    }
    d_beta = beta;
}

void control_loop::set_frequency(float freq)
{
    if (freq > d_max_freq)
        d_freq = d_max_freq;
    else if (freq < d_min_freq)
        d_freq = d_min_freq;
    else
        d_freq = freq;
}

void control_loop::set_phase(float phase)
{
    d_phase = phase;
    while (d_phase > M_TWOPI)
        d_phase -= M_TWOPI;
    while (d_phase < -M_TWOPI)
        d_phase += M_TWOPI;
}

void control_loop::set_max_freq(float freq) { d_max_freq = freq; }

void control_loop::set_min_freq(float freq) { d_min_freq = freq; }

/*******************************************************************
 * GET FUNCTIONS
 *******************************************************************/

float control_loop::get_loop_bandwidth() const { return d_loop_bw; }

float control_loop::get_damping_factor() const { return d_damping; }

float control_loop::get_alpha() const { return d_alpha; }

float control_loop::get_beta() const { return d_beta; }

float control_loop::get_frequency() const { return d_freq; }

float control_loop::get_phase() const { return d_phase; }

float control_loop::get_max_freq() const { return d_max_freq; }

float control_loop::get_min_freq() const { return d_min_freq; }

} /* namespace blocks */
} /* namespace gr */

/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef FLAT_FADER_IMPL_H
#define FLAT_FADER_IMPL_H

#include "sincostable.h"
#include <gnuradio/fxpt.h>
#include <gnuradio/io_signature.h>

#include <cstdint>
#include <random>

// FASTSINCOS:  0 = slow native,  1 = gr::fxpt impl,  2 = sincostable.h
#define FASTSINCOS 2

namespace gr {
namespace channels {

class flat_fader_impl
{
private:
    // initial theta variate generator
    std::mt19937 rng_1;
    std::uniform_real_distribution<double> dist_1; // U(-pi,pi)

    // random walk variate
    std::mt19937 rng_2;
    std::uniform_real_distribution<double> dist_2; // U(0,1)

public:
    int d_N;        // number of sinusoids
    float d_fDTs;   // normalized maximum doppler frequency
    double d_theta; // random walk variable (RWP)
    float d_theta_los;
    float d_step; // maximum random walk step size
    uint64_t d_m; // sample counter

    float d_K;  // Rician factor (ratio of the specular power to the scattered power)
    bool d_LOS; // LOS path exists? chooses Rician (LOS) vs Rayleigh (NLOS) model.

    std::vector<float> d_psi; // in-phase initial phase
    std::vector<float> d_phi; // quadrature initial phase

    std::vector<float> d_costable;

    sincostable d_table;

    float scale_sin, scale_los, scale_nlos;

    void update_theta();

    flat_fader_impl(uint32_t N, float fDTs, bool LOS, float K, uint32_t seed);
    gr_complex next_sample();
    void next_samples(std::vector<gr_complex>& HVec, int n_samples);

}; /* class flat_fader_impl */
} /* namespace channels */
} /* namespace gr */

#endif // FLAT_FADER_IMPL_H

/* -*- c++ -*- */
/*
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "flat_fader_impl.h"
#include "sincostable.h"
#include <gnuradio/math.h>
#include <boost/test/unit_test.hpp>

#include <cmath>
#include <stdexcept>

using gr::channels::flat_fader_impl;

BOOST_AUTO_TEST_CASE(sincostable_matches_libm)
{
    const size_t sz = 8 * 1024;
    sincostable table(sz);
    // truncated lookup error is at most one table step
    const double tol = 2 * GR_M_PI / sz + 1e-6;
    for (double x = -8 * GR_M_PI; x < 8 * GR_M_PI; x += 0.0123) {
        BOOST_CHECK_SMALL(table.sin(x) - std::sin(x), tol);
        BOOST_CHECK_SMALL(table.cos(x) - std::cos(x), tol);
    }
    BOOST_CHECK_THROW(sincostable(1000), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(fader_streams_are_distinct_across_seeds)
{
    // seeds s and s + 1 (adjacent paths) yield independent streams
    for (uint32_t seed = 0; seed < 20; seed++) {
        flat_fader_impl a(8, 0.01, false, 0, seed);
        flat_fader_impl b(8, 0.01, false, 0, seed + 1);
        const double theta = a.d_theta;
        a.update_theta();
        const double u_a = (a.d_theta - theta) / a.d_step;
        const double u_b = (b.d_theta + GR_M_PI) / (2 * GR_M_PI);
        BOOST_CHECK_GT(std::abs(u_a - u_b), 1e-4);
    }
}

BOOST_AUTO_TEST_CASE(fader_streams_are_distinct_across_paths)
{
    // path 1 of seed s vs. path 0 of seed s + 1
    for (uint32_t seed = 0; seed < 20; seed++) {
        flat_fader_impl a(8, 0.01, false, 0, seed, 1);
        flat_fader_impl b(8, 0.01, false, 0, seed + 1, 0);
        BOOST_CHECK_NE(a.d_theta, b.d_theta);
        BOOST_CHECK_NE(a.d_psi[1], b.d_psi[1]);
    }
}

BOOST_AUTO_TEST_CASE(quadrature_doppler_uses_sin_alpha)
{
    // Alimohammad et al., Model III eqs. (2)-(3)
    const float fDTs = 0.01;
    flat_fader_impl f(8, fDTs, false, 0, 3);
    f.set_step(0); // freeze theta
    const auto psi0 = f.d_psi, phi0 = f.d_phi;
    f.next_sample();
    for (int n = 1; n <= f.d_N; n++) {
        const double alpha = (2 * GR_M_PI * n - GR_M_PI + f.d_theta) / (4 * f.d_N);
        const double dpsi = std::remainder(f.d_psi[n] - psi0[n], 2 * GR_M_PI);
        const double dphi = std::remainder(f.d_phi[n] - phi0[n], 2 * GR_M_PI);
        BOOST_CHECK_SMALL(dpsi - 2 * GR_M_PI * fDTs * std::cos(alpha), 1e-3);
        BOOST_CHECK_SMALL(dphi - 2 * GR_M_PI * fDTs * std::sin(alpha), 1e-3);
    }
}

BOOST_AUTO_TEST_CASE(low_doppler_phase_precision)
{
    const float fDTs = 1e-7;
    const int M = 1000;
    flat_fader_impl f(8, fDTs, false, 0, 3);
    f.set_step(0);
    const auto psi0 = f.d_psi;
    std::vector<gr_complex> h;
    f.next_samples(h, M);
    for (int n = 1; n <= f.d_N; n++) {
        const double alpha = (2 * GR_M_PI * n - GR_M_PI + f.d_theta) / (4 * f.d_N);
        const double expected = M * 2 * GR_M_PI * fDTs * std::cos(alpha);
        const double dpsi = std::remainder(f.d_psi[n] - psi0[n], 2 * GR_M_PI);
        // cos(alpha_n) table resolution: 1e-3 of fD
        BOOST_CHECK_SMALL(dpsi - expected, 1e-3 * M * 2 * GR_M_PI * fDTs);
    }
}

BOOST_AUTO_TEST_CASE(random_walk_step)
{
    // proportional to fD*Ts; Table II value at fD*Ts = 0.01
    flat_fader_impl f(8, 0.01, false, 0, 0);
    BOOST_CHECK_CLOSE(f.d_step, 1e-5f, 1e-3);
    f.set_fDTs(0.002);
    BOOST_CHECK_CLOSE(f.d_step, 2e-6f, 1e-3);
}

BOOST_AUTO_TEST_CASE(setters_keep_random_walk_direction)
{
    flat_fader_impl f(8, 0.01, true, 4, 0);
    f.d_step = -f.d_step;
    f.set_fDTs(0.005);
    BOOST_CHECK_CLOSE(f.d_step, -5e-6f, 1e-3);
    f.set_step(2e-6);
    BOOST_CHECK_CLOSE(f.d_step, -2e-6f, 1e-3);
    f.set_K(1);
    BOOST_CHECK_CLOSE(f.scale_los, std::sqrt(0.5f), 1e-3);
    BOOST_CHECK_CLOSE(f.scale_nlos, std::sqrt(0.5f), 1e-3);
}

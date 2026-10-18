/* -*- c++ -*- */
/*
 * Copyright 2016,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "flat_fader_impl.h"
#include <gnuradio/math.h>
#include <cmath>

namespace gr {
namespace channels {

namespace {
// Random walk step proportional to fD*Ts, so theta sweeps [-pi, pi] in a fixed
// number of Doppler periods. Equals Table II at fD*Ts = 0.01.
float max_step(float fDTs) { return 1e-3f * fDTs; }
// fmod(x, 2 pi) without the libm call for the common case; identical result
inline double wrap_phase(double x)
{
    constexpr double two_pi = 2 * GR_M_PI;
    if (x >= two_pi) {
        return x < 2 * two_pi ? x - two_pi : fmod(x, two_pi);
    }
    if (x <= -two_pi) {
        return x > -2 * two_pi ? x + two_pi : fmod(x, two_pi);
    }
    return x;
}
// independent 64-bit seed per (seed, stream, purpose)
uint64_t derive_seed(uint32_t seed, uint32_t stream, uint32_t purpose)
{
    std::seed_seq seq{ seed, stream, purpose };
    uint32_t words[2];
    seq.generate(words, words + 2);
    return (uint64_t(words[0]) << 32) | words[1];
}
} // namespace

flat_fader_impl::flat_fader_impl(
    uint32_t N, float fDTs, bool LOS, float K, uint32_t seed, uint32_t stream)
    : d_walk_rng(derive_seed(seed, stream, 1)),

      d_N(N),
      d_fDTs(fDTs),

      d_step(max_step(fDTs)),
      d_m(0),
      d_K(K),
      d_LOS(LOS),

      d_psi(d_N + 1, 0),
      d_phi(d_N + 1, 0),

      d_table(8 * 1024),

      scale_sin(sqrtf(1.0 / d_N)),
      scale_los(sqrtf(d_K) / sqrtf(d_K + 1)),
      scale_nlos(1 / sqrtf(d_K + 1))
{
    // initial theta and phases, U(-pi, pi)
    gr::random init_rng(derive_seed(seed, stream, 0));
    auto uniform_angle = [&init_rng] { return GR_M_PI * (2 * init_rng.ran1() - 1); };
    d_theta = uniform_angle();
    d_theta_los = uniform_angle();
    for (int i = 0; i < d_N + 1; i++) {
        d_psi[i] = uniform_angle();
        d_phi[i] = uniform_angle();
    }
}

#if FASTSINCOS == 1
#define _GRFASTSIN(x) gr::fxpt::sin(gr::fxpt::float_to_fixed(x))
#define _GRFASTCOS(x) gr::fxpt::cos(gr::fxpt::float_to_fixed(x))
#elif FASTSINCOS == 2
#define _GRFASTSIN(x) d_table.sin(x)
#define _GRFASTCOS(x) d_table.cos(x)
#else
#define _GRFASTSIN(x) sin(x)
#define _GRFASTCOS(x) cos(x)
#endif

void flat_fader_impl::next_samples(std::vector<gr_complex>& Hvec, int n_samples)
{
    Hvec.resize(n_samples);
    const double inv_4N = 1.0 / (4 * d_N);
    for (int i = 0; i < n_samples; i++) {
        gr_complex H(0, 0);
        for (int n = 1; n < d_N + 1; n++) {
            // Model III, eqs. (2)-(3): I uses cos(alpha_n), Q uses sin(alpha_n).
            // Phases are accumulated per sample rather than computed as
            // m * cos(alpha_n[m]); amplitudes are scaled for unit power.
            float alpha_n = (2 * GR_M_PI * n - GR_M_PI + d_theta) * inv_4N;
            d_psi[n] = wrap_phase(d_psi[n] + 2 * GR_M_PI * d_fDTs * _GRFASTCOS(alpha_n));
            d_phi[n] = wrap_phase(d_phi[n] + 2 * GR_M_PI * d_fDTs * _GRFASTSIN(alpha_n));
            float s_i = scale_sin * _GRFASTCOS(d_psi[n]);
            float s_q = scale_sin * _GRFASTSIN(d_phi[n]);
            H += gr_complex(s_i, s_q);
        }

        if (d_LOS) {
            d_psi[0] =
                wrap_phase(d_psi[0] + 2 * GR_M_PI * d_fDTs * _GRFASTCOS(d_theta_los));
            float los_i = scale_los * _GRFASTCOS(d_psi[0]);
            float los_q = scale_los * _GRFASTSIN(d_psi[0]);
            H = H * scale_nlos + gr_complex(los_i, los_q);
        }

        update_theta();
        Hvec[i] = H;
    }
}

gr_complex flat_fader_impl::next_sample()
{
    std::vector<gr_complex> v(1);
    next_samples(v, 1);
    return v[0];
}

void flat_fader_impl::set_fDTs(float fDTs)
{
    d_fDTs = fDTs;
    d_step = std::copysign(max_step(fDTs), d_step);
}

void flat_fader_impl::set_K(float K)
{
    d_K = K;
    scale_los = sqrtf(d_K) / sqrtf(d_K + 1);
    scale_nlos = 1 / sqrtf(d_K + 1);
}

void flat_fader_impl::set_step(float step) { d_step = std::copysign(step, d_step); }

void flat_fader_impl::update_theta()
{
    d_theta += d_step * d_walk_rng.ran1();
    if (d_theta > GR_M_PI) {
        d_theta = GR_M_PI;
        d_step = -d_step;
    } else if (d_theta < -GR_M_PI) {
        d_theta = -GR_M_PI;
        d_step = -d_step;
    }
}

} /* namespace channels */
} /* namespace gr */

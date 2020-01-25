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

namespace gr {
namespace channels {

flat_fader_impl::flat_fader_impl(uint32_t N, float fDTs, bool LOS, float K, uint32_t seed)
    : rng_1(seed),
      dist_1(-GR_M_PI, GR_M_PI),
      rng_2(seed + 1),
      dist_2(0, 1),

      d_N(N),
      d_fDTs(fDTs),
      d_theta(dist_1(rng_1)),
      d_theta_los(dist_1(rng_1)),
      d_step(powf(0.00125 * fDTs, 1.1)), // max step size approximated from Table 2
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
    // generate initial phase values
    for (int i = 0; i < d_N + 1; i++) {
        d_psi[i] = dist_1(rng_1);
        d_phi[i] = dist_1(rng_1);
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
    for (int i = 0; i < n_samples; i++) {
        gr_complex H(0, 0);
        for (int n = 1; n < d_N + 1; n++) {
            float alpha_n = (2 * GR_M_PI * n - GR_M_PI + d_theta) / (4 * d_N);
            d_psi[n] =
                fmod(d_psi[n] + 2 * GR_M_PI * d_fDTs * _GRFASTCOS(alpha_n), 2 * GR_M_PI);
            d_phi[n] =
                fmod(d_phi[n] + 2 * GR_M_PI * d_fDTs * _GRFASTCOS(alpha_n), 2 * GR_M_PI);
            float s_i = scale_sin * _GRFASTCOS(d_psi[n]);
            float s_q = scale_sin * _GRFASTSIN(d_phi[n]);
            H += gr_complex(s_i, s_q);
        }

        if (d_LOS) {
            d_psi[0] = fmod(d_psi[0] + 2 * GR_M_PI * d_fDTs * _GRFASTCOS(d_theta_los),
                            2 * GR_M_PI);
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

void flat_fader_impl::update_theta()
{
    d_theta += (d_step * dist_2(rng_2));
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

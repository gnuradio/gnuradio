/* -*- c++ -*- */
/*
 * Copyright (C) 2011,2013,2016-2017 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "clock_tracking_loop.h"
#include <gnuradio/math.h>
#include <stdexcept>

namespace gr {
  namespace digital {

    clock_tracking_loop::clock_tracking_loop(float loop_bw,
                                             float max_period, float min_period,
                                             float nominal_period,
                                             float damping,
                                             float ted_gain)
      : d_avg_period(nominal_period),
        d_max_avg_period(max_period),
        d_min_avg_period(min_period),
        d_nom_avg_period(nominal_period),
        d_inst_period(nominal_period),
        d_phase(0.0f),
        d_zeta(damping),
        d_omega_n_norm(loop_bw),
        d_ted_gain(ted_gain),
        d_alpha(0.0f),
        d_beta(0.0f),
        d_prev_avg_period(nominal_period),
        d_prev_inst_period(nominal_period),
        d_prev_phase(0.0f)
    {
        set_max_avg_period(max_period);
        set_min_avg_period(min_period);
        set_nom_avg_period(nominal_period);

        set_avg_period(d_nom_avg_period);
        set_inst_period(d_nom_avg_period);

        if (d_zeta < 0.0f)
            throw std::out_of_range (
                           "clock_tracking_loop: damping factor must be > 0.0");

        if (d_omega_n_norm < 0.0f)
            throw std::out_of_range (
             "clock_tracking_loop: loop bandwidth must be greater than 0.0");

        if (d_ted_gain <= 0.0f)
            throw std::out_of_range (
             "clock_tracking_loop: expected ted gain must be greater than 0.0");

        update_gains();
    }

    clock_tracking_loop::~clock_tracking_loop()
    {
    }

    void
    clock_tracking_loop::update_gains()
    {
        float omega_n_T, omega_d_T, zeta_omega_n_T, cosx_omega_d_T;
        float k0, k1, sinh_zeta_omega_n_T;
        float alpha, beta;

        omega_n_T = d_omega_n_norm;
        zeta_omega_n_T = d_zeta * omega_n_T;
        k0 = 2.0f/d_ted_gain;
        k1 = expf(-zeta_omega_n_T);
        sinh_zeta_omega_n_T = sinhf(zeta_omega_n_T);

        if (d_zeta > 1.0f) { // Over-damped (or critically-damped too)

            omega_d_T = omega_n_T * sqrtf(d_zeta * d_zeta - 1.0f);
            cosx_omega_d_T = coshf(omega_d_T);
            // cosh ---------^^^^

        } else if (d_zeta == 1.0f) { // Critically-damped

            omega_d_T = 0.0f;
            cosx_omega_d_T = 1.0f;
            // cosh(omega_d_T) & cos(omega_d_T) are both 1 for omega_d_T == 0

        } else { // Under-damped (or critically-damped too)

            omega_d_T = omega_n_T * sqrtf(1.0 - d_zeta * d_zeta);
            cosx_omega_d_T = cosf(omega_d_T);
            // cos ----------^^^
        }

        alpha = k0 * k1 * sinh_zeta_omega_n_T;
        beta  = k0 * (1 - k1*(sinh_zeta_omega_n_T + cosx_omega_d_T));

        set_alpha(alpha);
        set_beta(beta);
    }

    void
    clock_tracking_loop::advance_loop(float error)
    {
        // So the loop can be reverted one step, if needed.
        d_prev_avg_period = d_avg_period;
        d_prev_inst_period = d_inst_period;
        d_prev_phase = d_phase;

        // Integral arm of PI filter
        d_avg_period = d_avg_period + d_beta * error;
        // Proportional arm of PI filter and final sum of PI filter arms
        d_inst_period = d_avg_period + d_alpha * error;
        // Compute the new, unwrapped clock phase
        d_phase = d_phase + d_inst_period;
    }

    void
    clock_tracking_loop::revert_loop()
    {
        d_avg_period = d_prev_avg_period;
        d_inst_period = d_prev_inst_period;
        d_phase = d_prev_phase;
    }

    void
    clock_tracking_loop::phase_wrap()
    {
        float period = d_avg_period; // One could argue d_inst_period instead
        float limit = period/2.0f;

        while (d_phase > limit)
            d_phase -= period;

        while (d_phase <= -limit)
            d_phase += period;
    }

    void
    clock_tracking_loop::period_limit()
    {
        if (d_avg_period > d_max_avg_period)
            d_avg_period = d_max_avg_period;
        else if (d_avg_period < d_min_avg_period)
            d_avg_period = d_min_avg_period;
    }

    /*******************************************************************
     * SET FUNCTIONS
     *******************************************************************/

    void
    clock_tracking_loop::set_loop_bandwidth(float bw)
    {
        if (bw < 0.0f)
            throw std::out_of_range (
             "clock_tracking_loop: loop bandwidth must be greater than 0.0");

        d_omega_n_norm = bw;
        update_gains();
    }

    void
    clock_tracking_loop::set_damping_factor(float df)
    {
        if (df < 0.0f)
            throw std::out_of_range (
                           "clock_tracking_loop: damping factor must be > 0.0");

        d_zeta = df;
        update_gains();
    }

    void
    clock_tracking_loop::set_ted_gain(float ted_gain)
    {
        if (ted_gain <= 0.0f)
            throw std::out_of_range (
                        "clock_tracking_loop: expected ted gain must be > 0.0");

        d_ted_gain = ted_gain;
        update_gains();
    }

    void
    clock_tracking_loop::set_alpha(float alpha)
    {
        d_alpha = alpha;
    }

    void
    clock_tracking_loop::set_beta(float beta)
    {
        d_beta = beta;
    }

    void
    clock_tracking_loop::set_avg_period(float period)
    {
        d_avg_period = period;
        d_prev_avg_period = period;
    }

    void
    clock_tracking_loop::set_inst_period(float period)
    {
        d_inst_period = period;
        d_prev_inst_period = period;
    }

    void
    clock_tracking_loop::set_phase(float phase)
    {
        // This previous phase is likely inconsistent with the tracking,
        // but if the caller is setting the phase, the odds of
        // revert_loop() being called are slim.
        d_prev_phase = phase;

        d_phase = phase;
    }

    void
    clock_tracking_loop::set_max_avg_period(float period)
    {
        d_max_avg_period = period;
    }

    void
    clock_tracking_loop::set_min_avg_period(float period)
    {
        d_min_avg_period = period;
    }

    void
    clock_tracking_loop::set_nom_avg_period(float period)
    {
        if (period < d_min_avg_period ||
            period > d_max_avg_period   ) {
            d_nom_avg_period = (d_max_avg_period + d_min_avg_period)/2.0f;
        } else {
            d_nom_avg_period = period;
        }
    }

    /*******************************************************************
     * GET FUNCTIONS
     *******************************************************************/

    float
    clock_tracking_loop::get_loop_bandwidth() const
    {
        return d_omega_n_norm;
    }

    float
    clock_tracking_loop::get_damping_factor() const
    {
        return d_zeta;
    }

    float
    clock_tracking_loop::get_ted_gain() const
    {
        return d_ted_gain;
    }

    float
    clock_tracking_loop::get_alpha() const
    {
        return d_alpha;
    }

    float
    clock_tracking_loop::get_beta() const
    {
        return d_beta;
    }

    float
    clock_tracking_loop::get_avg_period() const
    {
        return d_avg_period;
    }

    float
    clock_tracking_loop::get_inst_period() const
    {
        return d_inst_period;
    }

    float
    clock_tracking_loop::get_phase() const
    {
        return d_phase;
    }

    float
    clock_tracking_loop::get_max_avg_period() const
    {
        return d_max_avg_period;
    }

    float
    clock_tracking_loop::get_min_avg_period() const
    {
        return d_min_avg_period;
    }

    float
    clock_tracking_loop::get_nom_avg_period() const
    {
        return d_nom_avg_period;
    }

  } /* namespace digital */
} /* namespace gr */

/* -*- c++ -*- */
/*
 * Copyright (C) 2017 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "timing_error_detector.h"
#include <gnuradio/math.h>
#include <memory>
#include <stdexcept>

namespace gr {
namespace digital {

std::unique_ptr<timing_error_detector>
timing_error_detector::make(enum ted_type type, constellation_sptr constellation)
{
    switch (type) {
    case TED_NONE:
        return nullptr;
    case TED_MUELLER_AND_MULLER:
        return std::make_unique<ted_mueller_and_muller>(constellation);
    case TED_MOD_MUELLER_AND_MULLER:
        return std::make_unique<ted_mod_mueller_and_muller>(constellation);
    case TED_ZERO_CROSSING:
        return std::make_unique<ted_zero_crossing>(constellation);
    case TED_GARDNER:
        return std::make_unique<ted_gardner>();
    case TED_EARLY_LATE:
        return std::make_unique<ted_early_late>();
    case TED_DANDREA_AND_MENGALI_GEN_MSK:
        return std::make_unique<ted_generalized_msk>();
    case TED_SIGNAL_TIMES_SLOPE_ML:
        return std::make_unique<ted_signal_times_slope_ml>();
    case TED_SIGNUM_TIMES_SLOPE_ML:
        return std::make_unique<ted_signum_times_slope_ml>();
    case TED_MENGALI_AND_DANDREA_GMSK:
        return std::make_unique<ted_gaussian_msk>();
    }
    return nullptr;
}

timing_error_detector::timing_error_detector(enum ted_type type,
                                             int inputs_per_symbol,
                                             int error_computation_depth,
                                             bool needs_lookahead,
                                             bool needs_derivative,
                                             constellation_sptr constellation)
    : d_constellation(constellation),
      d_error(0.0f),
      d_prev_error(0.0f),
      d_inputs_per_symbol(inputs_per_symbol),
      d_error_depth(error_computation_depth),
      d_input(),
      d_decision(),
      d_input_derivative(),
      d_needs_lookahead(needs_lookahead),
      d_needs_derivative(needs_derivative)
{
    if (d_constellation && d_constellation->dimensionality() != 1)
        throw std::invalid_argument("timing_error_detector: constellation dimensionality "
                                    "(complex numbers per symbol) must be 1.");

    switch (type) {
    case TED_MUELLER_AND_MULLER:
    case TED_MOD_MUELLER_AND_MULLER:
    case TED_ZERO_CROSSING:
        if (!d_constellation)
            throw std::invalid_argument(
                "timing_error_detector: slicer constellation required.");
        break;
    case TED_GARDNER:
    case TED_EARLY_LATE:
    case TED_DANDREA_AND_MENGALI_GEN_MSK:
    case TED_SIGNAL_TIMES_SLOPE_ML:
    case TED_SIGNUM_TIMES_SLOPE_ML:
    case TED_MENGALI_AND_DANDREA_GMSK:
        break;
    case TED_NONE:
    default:
        throw std::invalid_argument(
            "timing_error_detector: invalid timing error detector type.");
        break;
    }

    sync_reset();
}

void timing_error_detector::input(const gr_complex& x, const gr_complex& dx)
{
    d_input.push_front(x);
    d_input.pop_back();

    if (d_constellation) {
        d_decision.push_front(slice(d_input[0]));
        d_decision.pop_back();
    }

    if (d_needs_derivative) {
        d_input_derivative.push_front(dx);
        d_input_derivative.pop_back();
    }

    advance_input_clock();
    if (d_input_clock == 0 && d_needs_lookahead == false) {
        d_prev_error = d_error;
        d_error = compute_error_cf();
    }
}

void timing_error_detector::input(float x, float dx)
{
    d_input.push_front(gr_complex(x, 0.0f));
    d_input.pop_back();

    if (d_constellation) {
        d_decision.push_front(slice(d_input[0]));
        d_decision.pop_back();
    }

    if (d_needs_derivative) {
        d_input_derivative.push_front(gr_complex(dx, 0.0f));
        d_input_derivative.pop_back();
    }

    advance_input_clock();
    if (d_input_clock == 0 && d_needs_lookahead == false) {
        d_prev_error = d_error;
        d_error = compute_error_ff();
    }
}

void timing_error_detector::input_lookahead(const gr_complex& x, const gr_complex& dx)
{
    if (d_input_clock != 0 || d_needs_lookahead == false)
        return;

    d_input.push_front(x);
    if (d_constellation)
        d_decision.push_front(slice(d_input[0]));
    if (d_needs_derivative)
        d_input_derivative.push_front(dx);

    d_prev_error = d_error;
    d_error = compute_error_cf();

    if (d_needs_derivative)
        d_input_derivative.pop_front();
    if (d_constellation)
        d_decision.pop_front();
    d_input.pop_front();
}

void timing_error_detector::input_lookahead(float x, float dx)
{
    if (d_input_clock != 0 || d_needs_lookahead == false)
        return;

    d_input.push_front(gr_complex(x, 0.0f));
    if (d_constellation)
        d_decision.push_front(slice(d_input[0]));
    if (d_needs_derivative)
        d_input_derivative.push_front(gr_complex(dx, 0.0f));

    d_prev_error = d_error;
    d_error = compute_error_ff();

    if (d_needs_derivative)
        d_input_derivative.pop_front();
    if (d_constellation)
        d_decision.pop_front();
    d_input.pop_front();
}

void timing_error_detector::revert(bool preserve_error)
{
    if (d_input_clock == 0 && preserve_error != true)
        d_error = d_prev_error;
    revert_input_clock();

    if (d_needs_derivative) {
        d_input_derivative.push_back(d_input_derivative.back());
        d_input_derivative.pop_front();
    }

    if (d_constellation) {
        d_decision.push_back(d_decision.back());
        d_decision.pop_front();
    }

    d_input.push_back(d_input.back());
    d_input.pop_front();
}

void timing_error_detector::sync_reset()
{
    d_error = 0.0f;
    d_prev_error = 0.0f;

    d_input.assign(d_error_depth, gr_complex(0.0f, 0.0f));
    d_input_derivative.assign(d_error_depth, gr_complex(0.0f, 0.0f));

    if (d_constellation) {
        d_decision.assign(d_input.size(), gr_complex(0.0f, 0.0f));
        // for (it = d_input.begin(); it != d_input.end(); ++it)
        //   d_decision.push_back(slice(*it));
    }

    sync_reset_input_clock();
}

gr_complex timing_error_detector::slice(const gr_complex& x)
{
    unsigned int index;
    /* Passing a length 1 array is OK since we only accept 1D constellations */
    gr_complex z[1];

    index = d_constellation->decision_maker(&x);
    d_constellation->map_to_points(index, z);
    return z[0];
}

/*************************************************************************/

float ted_mueller_and_muller::compute_error_cf()
{
    return (d_decision[1].real() * d_input[0].real() -
            d_decision[0].real() * d_input[1].real()) +
           (d_decision[1].imag() * d_input[0].imag() -
            d_decision[0].imag() * d_input[1].imag());
}

float ted_mueller_and_muller::compute_error_ff()
{
    return (d_decision[1].real() * d_input[0].real() -
            d_decision[0].real() * d_input[1].real());
}

/*************************************************************************/

float ted_mod_mueller_and_muller::compute_error_cf()
{
    gr_complex u;

    u = ((d_input[0] - d_input[2]) * conj(d_decision[1])) -
        ((d_decision[0] - d_decision[2]) * conj(d_input[1]));

    return gr::branchless_clip(u.real(), 1.0f);
}

float ted_mod_mueller_and_muller::compute_error_ff()
{
    float u;

    u = ((d_input[0].real() - d_input[2].real()) * d_decision[1].real()) -
        ((d_decision[0].real() - d_decision[2].real()) * d_input[1].real());

    return gr::branchless_clip(u / 2.0f, 1.0f);
}

/*************************************************************************/

float ted_zero_crossing::compute_error_cf()
{
    return ((d_decision[2].real() - d_decision[0].real()) * d_input[1].real()) +
           ((d_decision[2].imag() - d_decision[0].imag()) * d_input[1].imag());
}

float ted_zero_crossing::compute_error_ff()
{
    return ((d_decision[2].real() - d_decision[0].real()) * d_input[1].real());
}

/*************************************************************************/

float ted_gardner::compute_error_cf()
{
    return ((d_input[2].real() - d_input[0].real()) * d_input[1].real()) +
           ((d_input[2].imag() - d_input[0].imag()) * d_input[1].imag());
}

float ted_gardner::compute_error_ff()
{
    return ((d_input[2].real() - d_input[0].real()) * d_input[1].real());
}

/*************************************************************************/

float ted_early_late::compute_error_cf()
{
    return ((d_input[0].real() - d_input[2].real()) * d_input[1].real()) +
           ((d_input[0].imag() - d_input[2].imag()) * d_input[1].imag());
}

float ted_early_late::compute_error_ff()
{
    return ((d_input[0].real() - d_input[2].real()) * d_input[1].real());
}

/*************************************************************************/

float ted_generalized_msk::compute_error_cf()
{
    gr_complex u;

    u = (d_input[1] * d_input[1] * conj(d_input[5] * d_input[5])) -
        (d_input[3] * d_input[3] * conj(d_input[7] * d_input[7]));

    return gr::branchless_clip(u.real(), 3.0f);
}

float ted_generalized_msk::compute_error_ff()
{
    float u;

    u = (d_input[1].real() * d_input[1].real() * d_input[5].real() * d_input[5].real()) -
        (d_input[3].real() * d_input[3].real() * d_input[7].real() * d_input[7].real());

    return gr::branchless_clip(u, 3.0f);
}

/*************************************************************************/

float ted_gaussian_msk::compute_error_cf()
{
    gr_complex u;

    u = -(d_input[0] * d_input[0] * conj(d_input[2] * d_input[2])) +
        (d_input[1] * d_input[1] * conj(d_input[3] * d_input[3]));

    return gr::branchless_clip(u.real(), 3.0f);
}

float ted_gaussian_msk::compute_error_ff()
{
    float u;

    u = -(d_input[0].real() * d_input[0].real() * d_input[2].real() * d_input[2].real()) +
        (d_input[1].real() * d_input[1].real() * d_input[3].real() * d_input[3].real());

    return gr::branchless_clip(u, 3.0f);
}

/*************************************************************************/

float ted_signal_times_slope_ml::compute_error_cf()
{
    return (d_input[0].real() * d_input_derivative[0].real() +
            d_input[0].imag() * d_input_derivative[0].imag()) /
           2.0f;
}

float ted_signal_times_slope_ml::compute_error_ff()
{
    return (d_input[0].real() * d_input_derivative[0].real());
}

/*************************************************************************/

float ted_signum_times_slope_ml::compute_error_cf()
{
    return ((d_input[0].real() < 0.0f ? -d_input_derivative[0].real()
                                      : d_input_derivative[0].real()) +
            (d_input[0].imag() < 0.0f ? -d_input_derivative[0].imag()
                                      : d_input_derivative[0].imag())) /
           2.0f;
}

float ted_signum_times_slope_ml::compute_error_ff()
{
    return (d_input[0].real() < 0.0f ? -d_input_derivative[0].real()
                                     : d_input_derivative[0].real());
}

} /* namespace digital */
} /* namespace gr */

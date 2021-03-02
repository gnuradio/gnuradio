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

#include "interpolating_resampler.h"
#include <gnuradio/math.h>
#include <deque>
#include <memory>
#include <stdexcept>

namespace gr {
namespace digital {

interpolating_resampler::interpolating_resampler(enum ir_type type, bool derivative)
    : d_type(type),
      d_derivative(derivative),
      d_phase(0.0f),
      d_phase_wrapped(0.0f),
      d_phase_n(0),
      d_prev_phase(0.0f),
      d_prev_phase_wrapped(0.0f),
      d_prev_phase_n(0)
{
    switch (d_type) {
    case IR_MMSE_8TAP:
        break;
    case IR_PFB_NO_MF:
        break;
    case IR_PFB_MF:
        break;
    case IR_NONE:
    default:
        throw std::invalid_argument(
            "interpolating_resampler: invalid interpolating resampler type.");
        break;
    }

    sync_reset(0.0f);
}

void interpolating_resampler::next_phase(float increment,
                                         float& phase,
                                         int& phase_n,
                                         float& phase_wrapped)
{
    float n;

    phase = d_phase_wrapped + increment;
    n = floorf(phase);
    phase_wrapped = phase - n;
    phase_n = static_cast<int>(n);
}

void interpolating_resampler::advance_phase(float increment)
{
    d_prev_phase = d_phase;
    d_prev_phase_wrapped = d_phase_wrapped;
    d_prev_phase_n = d_phase_n;

    next_phase(increment, d_phase, d_phase_n, d_phase_wrapped);
}

void interpolating_resampler::revert_phase()
{
    d_phase = d_prev_phase;
    d_phase_wrapped = d_prev_phase_wrapped;
    d_phase_n = d_prev_phase_n;
}

void interpolating_resampler::sync_reset(float phase)
{
    float n;

    d_phase = phase;
    n = floorf(d_phase);
    d_phase_wrapped = d_phase - n;
    d_phase_n = static_cast<int>(n);

    d_prev_phase = d_phase;
    d_prev_phase_wrapped = d_phase_wrapped;
    d_prev_phase_n = d_phase_n;
}

/*************************************************************************/

std::unique_ptr<interpolating_resampler_ccf> interpolating_resampler_ccf::make(
    enum ir_type type, bool derivative, int nfilts, const std::vector<float>& taps)
{
    switch (type) {
    case IR_MMSE_8TAP:
        return std::make_unique<interp_resampler_mmse_8tap_cc>(derivative);
    case IR_PFB_NO_MF:
        return std::make_unique<interp_resampler_pfb_no_mf_cc>(derivative, nfilts);
    case IR_PFB_MF:
        return std::make_unique<interp_resampler_pfb_mf_ccf>(taps, nfilts, derivative);
    case IR_NONE:
        return nullptr;
    }
    throw std::invalid_argument("interpolating_resampler_ccf: invalid "
                                "interpolating resampler type.");
}

/*************************************************************************/

std::unique_ptr<interpolating_resampler_fff> interpolating_resampler_fff::make(
    enum ir_type type, bool derivative, int nfilts, const std::vector<float>& taps)
{
    switch (type) {
    case IR_MMSE_8TAP:
        return std::make_unique<interp_resampler_mmse_8tap_ff>(derivative);
    case IR_PFB_NO_MF:
        return std::make_unique<interp_resampler_pfb_no_mf_ff>(derivative, nfilts);
    case IR_PFB_MF:
        return std::make_unique<interp_resampler_pfb_mf_fff>(taps, nfilts, derivative);
    case IR_NONE:
        return nullptr;
    }
    throw std::invalid_argument("interpolating_resampler_fff: invalid "
                                "interpolating resampler type.");
}

/*************************************************************************/

interp_resampler_mmse_8tap_cc::interp_resampler_mmse_8tap_cc(bool derivative)
    : interpolating_resampler_ccf(IR_MMSE_8TAP, derivative)
{
    if (d_derivative) {
        d_interp_diff = std::make_unique<filter::mmse_interp_differentiator_cc>();
    }
}

interp_resampler_mmse_8tap_cc::~interp_resampler_mmse_8tap_cc() {}

gr_complex interp_resampler_mmse_8tap_cc::interpolate(const gr_complex input[],
                                                      float mu) const
{
    return d_interp.interpolate(input, mu);
}

gr_complex interp_resampler_mmse_8tap_cc::differentiate(const gr_complex input[],
                                                        float mu) const
{
    return d_interp_diff->differentiate(input, mu);
}

unsigned int interp_resampler_mmse_8tap_cc::ntaps() const { return d_interp.ntaps(); }

/*************************************************************************/

interp_resampler_mmse_8tap_ff::interp_resampler_mmse_8tap_ff(bool derivative)
    : interpolating_resampler_fff(IR_MMSE_8TAP, derivative)
{
    if (d_derivative) {
        d_interp_diff = std::make_unique<filter::mmse_interp_differentiator_ff>();
    }
}

interp_resampler_mmse_8tap_ff::~interp_resampler_mmse_8tap_ff() {}

float interp_resampler_mmse_8tap_ff::interpolate(const float input[], float mu) const
{
    return d_interp.interpolate(input, mu);
}

float interp_resampler_mmse_8tap_ff::differentiate(const float input[], float mu) const
{
    return d_interp_diff->differentiate(input, mu);
}

unsigned int interp_resampler_mmse_8tap_ff::ntaps() const { return d_interp.ntaps(); }

/*************************************************************************/

#include "gnuradio/filter/interp_differentiator_taps.h"
#include "gnuradio/filter/interpolator_taps.h"

interp_resampler_pfb_no_mf_cc::interp_resampler_pfb_no_mf_cc(bool derivative, int nfilts)
    : interpolating_resampler_ccf(IR_PFB_NO_MF, derivative)
{
    if (nfilts <= 1)
        throw std::invalid_argument("interpolating_resampler_pfb_no_mf_cc: "
                                    "number of polyphase filter arms "
                                    "must be greater than 1");

    // Round up the number of filter arms to the current or next power of 2
    d_nfilters = 1 << (static_cast<int>(log2f(static_cast<float>(nfilts - 1))) + 1);

    // N.B. We assume in this class: NSTEPS == DNSTEPS and NTAPS == DNTAPS

    // Limit to the maximum number of precomputed MMSE tap sets
    if (d_nfilters <= 0 || d_nfilters > NSTEPS)
        d_nfilters = NSTEPS;

    // Create our polyphase filter arms for the steps from 0.0 to 1.0 from
    // the MMSE interpolating filter and MMSE interpolating differentiator
    // taps rows.
    // N.B. We create an extra final row for an offset of 1.0, because it's
    // easier than dealing with wrap around from 0.99... to 0.0 shifted
    // by 1 tap.
    d_filters.reserve(d_nfilters + 1);
    d_diff_filters.reserve(d_nfilters + 1);

    std::vector<float> t(NTAPS, 0);
    int incr = NSTEPS / d_nfilters;
    for (int src = 0; src <= NSTEPS; src += incr) {

        t.assign(&taps[src][0], &taps[src][NTAPS]);
        d_filters.emplace_back(t);
        if (d_derivative) {
            t.assign(&Dtaps[src][0], &Dtaps[src][DNTAPS]);
            d_diff_filters.emplace_back(t);
        }
    }
}

interp_resampler_pfb_no_mf_cc::~interp_resampler_pfb_no_mf_cc() {}

gr_complex interp_resampler_pfb_no_mf_cc::interpolate(const gr_complex input[],
                                                      float mu) const
{
    int arm = static_cast<int>(rint(mu * d_nfilters));

    if (arm < 0 || arm > d_nfilters)
        throw std::runtime_error("interp_resampler_pfb_no_mf_cc: mu is not "
                                 "in the range [0.0, 1.0]");

    return d_filters[arm].filter(input);
}

gr_complex interp_resampler_pfb_no_mf_cc::differentiate(const gr_complex input[],
                                                        float mu) const
{
    int arm = static_cast<int>(rint(mu * d_nfilters));

    if (arm < 0 || arm > d_nfilters)
        throw std::runtime_error("interp_resampler_pfb_no_mf_cc: mu is not "
                                 "in the range [0.0, 1.0]");

    return d_diff_filters[arm].filter(input);
}

unsigned int interp_resampler_pfb_no_mf_cc::ntaps() const { return NTAPS; }

/*************************************************************************/

interp_resampler_pfb_no_mf_ff::interp_resampler_pfb_no_mf_ff(bool derivative, int nfilts)
    : interpolating_resampler_fff(IR_PFB_NO_MF, derivative), d_nfilters(0)
{
    if (nfilts <= 1)
        throw std::invalid_argument("interpolating_resampler_pfb_no_mf_ff: "
                                    "number of polyphase filter arms "
                                    "must be greater than 1");

    // Round up the number of filter arms to the current or next power of 2
    d_nfilters = 1 << (static_cast<int>(log2f(static_cast<float>(nfilts - 1))) + 1);

    // N.B. We assume in this class: NSTEPS == DNSTEPS and NTAPS == DNTAPS

    // Limit to the maximum number of precomputed MMSE tap sets
    if (d_nfilters <= 0 || d_nfilters > NSTEPS)
        d_nfilters = NSTEPS;

    // Create our polyphase filter arms for the steps from 0.0 to 1.0 from
    // the MMSE interpolating filter and MMSE interpolating differentiator
    // taps rows.
    // N.B. We create an extra final row for an offset of 1.0, because it's
    // easier than dealing with wrap around from 0.99... to 0.0 shifted
    // by 1 tap.
    d_filters.reserve(d_nfilters + 1);
    d_diff_filters.reserve(d_nfilters + 1);

    std::vector<float> t(NTAPS, 0);
    int incr = NSTEPS / d_nfilters;
    for (int src = 0; src <= NSTEPS; src += incr) {

        t.assign(&taps[src][0], &taps[src][NTAPS]);
        d_filters.emplace_back(t);
        if (d_derivative) {
            t.assign(&Dtaps[src][0], &Dtaps[src][DNTAPS]);
            d_diff_filters.emplace_back(t);
        }
    }
}

interp_resampler_pfb_no_mf_ff::~interp_resampler_pfb_no_mf_ff() {}

float interp_resampler_pfb_no_mf_ff::interpolate(const float input[], float mu) const
{
    int arm = static_cast<int>(rint(mu * d_nfilters));

    if (arm < 0 || arm > d_nfilters)
        throw std::runtime_error("interp_resampler_pfb_no_mf_ff: mu is not "
                                 "in the range [0.0, 1.0]");

    return d_filters[arm].filter(input);
}

float interp_resampler_pfb_no_mf_ff::differentiate(const float input[], float mu) const
{
    int arm = static_cast<int>(rint(mu * d_nfilters));

    if (arm < 0 || arm > d_nfilters)
        throw std::runtime_error("interp_resampler_pfb_no_mf_ff: mu is not "
                                 "in the range [0.0, 1.0]");

    return d_diff_filters[arm].filter(input);
}

unsigned int interp_resampler_pfb_no_mf_ff::ntaps() const { return NTAPS; }

/*************************************************************************/

interp_resampler_pfb_mf_ccf::interp_resampler_pfb_mf_ccf(const std::vector<float>& taps,
                                                         int nfilts,
                                                         bool derivative)
    : interpolating_resampler_ccf(IR_PFB_MF, derivative),
      d_nfilters(nfilts),
      d_taps_per_filter(static_cast<unsigned int>(
          ceil(static_cast<double>(taps.size()) / static_cast<double>(nfilts))))
{
    if (d_nfilters <= 1)
        throw std::invalid_argument("interpolating_resampler_pfb_mf_ccf: "
                                    "number of polyphase filter arms "
                                    "must be greater than 1");
    if (taps.size() < static_cast<unsigned int>(d_nfilters))
        throw std::invalid_argument("interpolating_resampler_pfb_mf_ccf: "
                                    "length of the prototype filter taps"
                                    " must be greater than or equal to "
                                    "the number of polyphase filter arms.");

    // Create a derivative filter from the provided taps

    // First create a truncated ideal differentiator filter
    int ideal_diff_filt_len = 3; // Must be odd; rest of init assumes odd.
    std::vector<float> ideal_diff_taps(ideal_diff_filt_len, 0.0f);
    int i, n;
    n = ideal_diff_taps.size() / 2;
    for (i = -n; i < 0; i++) {
        ideal_diff_taps[i + n] = (-i & 1) == 1 ? -1.0f / i : 1.0f / i;
        ideal_diff_taps[n - i] = -ideal_diff_taps[i + n];
    }
    ideal_diff_taps[n] = 0.0f;

    // Perform linear convolution of prototype filter taps and the truncated
    // ideal differentiator taps to generate a derivative matched filter.
    // N.B. the truncated ideal differentiator taps must have an odd length
    int j, k, l, m;
    m = ideal_diff_taps.size();
    n = taps.size();
    l = m + n - 1; // length of convolution
    std::deque<float> diff_taps(l, 0.0f);
    for (i = 0; i < l; i++) {
        for (j = 0; j < m; j++) {
            k = i + j - (m - 1);
            if (k < 0 || k >= n)
                continue;
            diff_taps[i] += ideal_diff_taps[(m - 1) - j] * taps[k];
        }
    }

    // Trim the convolution so the prototype derivative filter is the same
    // length as the passed in prototype filter taps.
    n = ideal_diff_taps.size() / 2;
    for (i = 0; i < n; i++) {
        diff_taps.pop_back();
        diff_taps.pop_front();
    }

    // Squash the differentiation noise spikes at the filter ends.
    diff_taps[0] = 0.0f;
    diff_taps[diff_taps.size() - 1] = 0.0f;

    // Normalize the prototype derviative filter gain to the number of
    // filter arms
    n = diff_taps.size();
    float mag = 0.0f;
    for (i = 0; i < n; i++)
        mag += fabsf(diff_taps[i]);
    for (i = 0; i < n; i++) {
        diff_taps[i] *= d_nfilters / mag;
        if (d_derivative && std::isnan(diff_taps[i]))
            throw std::runtime_error("interpolating_resampler_pfb_mf_ccf: "
                                     "NaN error creating derivative filter.");
    }

    // Create our polyphase filter arms for the steps from 0.0 to 1.0 from
    // the prototype matched filter.
    // N.B. We create an extra final row for an offset of 1.0, because it's
    // easier than dealing with wrap around from 0.99... to 0.0 shifted
    // by 1 tap.
    d_filters.reserve(d_nfilters + 1);
    d_diff_filters.reserve(d_nfilters + 1);

    m = taps.size();
    n = diff_taps.size();
    d_taps.resize(d_nfilters + 1);
    d_diff_taps.resize(d_nfilters + 1);
    signed int taps_per_filter = static_cast<signed int>(d_taps_per_filter);

    for (i = 0; i <= d_nfilters; i++) {
        d_taps[i] = std::vector<float>(d_taps_per_filter, 0.0f);
        for (j = 0; j < taps_per_filter; j++) {
            k = i + j * d_nfilters;
            if (k < m)
                d_taps[i][j] = taps[k];
        }
        d_filters.emplace_back(d_taps[i]);
        if (!d_derivative)
            continue;

        d_diff_taps[i] = std::vector<float>(d_taps_per_filter, 0.0f);
        for (j = 0; j < taps_per_filter; j++) {
            k = i + j * d_nfilters;
            if (k < n)
                d_diff_taps[i][j] = diff_taps[k];
        }
        d_diff_filters.emplace_back(d_diff_taps[i]);
    }
}

interp_resampler_pfb_mf_ccf::~interp_resampler_pfb_mf_ccf() {}

gr_complex interp_resampler_pfb_mf_ccf::interpolate(const gr_complex input[],
                                                    float mu) const
{
    int arm = static_cast<int>(rint(mu * d_nfilters));

    if (arm < 0 || arm > d_nfilters)
        throw std::runtime_error("interp_resampler_pfb_mf_ccf: mu is not "
                                 "in the range [0.0, 1.0]");

    return d_filters[arm].filter(input);
}

gr_complex interp_resampler_pfb_mf_ccf::differentiate(const gr_complex input[],
                                                      float mu) const
{
    int arm = static_cast<int>(rint(mu * d_nfilters));

    if (arm < 0 || arm > d_nfilters)
        throw std::runtime_error("interp_resampler_pfb_mf_ccf: mu is not "
                                 "in the range [0.0, 1.0]");

    return d_diff_filters[arm].filter(input);
}

unsigned int interp_resampler_pfb_mf_ccf::ntaps() const { return d_taps_per_filter; }

/*************************************************************************/

interp_resampler_pfb_mf_fff::interp_resampler_pfb_mf_fff(const std::vector<float>& taps,
                                                         int nfilts,
                                                         bool derivative)
    : interpolating_resampler_fff(IR_PFB_MF, derivative),
      d_nfilters(nfilts),
      d_taps_per_filter(static_cast<unsigned int>(
          ceil(static_cast<double>(taps.size()) / static_cast<double>(nfilts))))
{
    if (d_nfilters <= 1)
        throw std::invalid_argument("interpolating_resampler_pfb_mf_fff: "
                                    "number of polyphase filter arms "
                                    "must be greater than 1");
    if (taps.size() < static_cast<unsigned int>(d_nfilters))
        throw std::invalid_argument("interpolating_resampler_pfb_mf_fff: "
                                    "length of the prototype filter taps"
                                    " must be greater than or equal to "
                                    "the number of polyphase filter arms.");

    // Create a derivative filter from the provided taps

    // First create a truncated ideal differentiator filter
    int ideal_diff_filt_len = 3; // Must be odd; rest of init assumes odd.
    std::vector<float> ideal_diff_taps(ideal_diff_filt_len, 0.0f);
    int i, n;
    n = ideal_diff_taps.size() / 2;
    for (i = -n; i < 0; i++) {
        ideal_diff_taps[i + n] = (-i & 1) == 1 ? -1.0f / i : 1.0f / i;
        ideal_diff_taps[n - i] = -ideal_diff_taps[i + n];
    }
    ideal_diff_taps[n] = 0.0f;

    // Perform linear convolution of prototype filter taps and the truncated
    // ideal differentiator taps to generate a derivative matched filter.
    // N.B. the truncated ideal differentiator taps must have an odd length
    int j, k, l, m;
    m = ideal_diff_taps.size();
    n = taps.size();
    l = m + n - 1; // length of convolution
    std::deque<float> diff_taps(l, 0.0f);
    for (i = 0; i < l; i++) {
        for (j = 0; j < m; j++) {
            k = i + j - (m - 1);
            if (k < 0 || k >= n)
                continue;
            diff_taps[i] += ideal_diff_taps[(m - 1) - j] * taps[k];
        }
    }

    // Trim the convolution so the prototype derivative filter is the same
    // length as the passed in prototype filter taps.
    n = ideal_diff_taps.size() / 2;
    for (i = 0; i < n; i++) {
        diff_taps.pop_back();
        diff_taps.pop_front();
    }

    // Squash the differentiation noise spikes at the filter ends.
    diff_taps[0] = 0.0f;
    diff_taps[diff_taps.size() - 1] = 0.0f;

    // Normalize the prototype derviative filter gain to the number of
    // filter arms
    n = diff_taps.size();
    float mag = 0.0f;
    for (i = 0; i < n; i++)
        mag += fabsf(diff_taps[i]);
    for (i = 0; i < n; i++) {
        diff_taps[i] *= d_nfilters / mag;
        if (d_derivative && std::isnan(diff_taps[i]))
            throw std::runtime_error("interpolating_resampler_pfb_mf_fff: "
                                     "NaN error creating derivative filter.");
    }

    // Create our polyphase filter arms for the steps from 0.0 to 1.0 from
    // the prototype matched filter.
    // N.B. We create an extra final row for an offset of 1.0, because it's
    // easier than dealing with wrap around from 0.99... to 0.0 shifted
    // by 1 tap.
    d_filters.reserve(d_nfilters + 1);
    d_diff_filters.reserve(d_nfilters + 1);

    m = taps.size();
    n = diff_taps.size();
    d_taps.resize(d_nfilters + 1);
    d_diff_taps.resize(d_nfilters + 1);
    signed int taps_per_filter = static_cast<signed int>(d_taps_per_filter);

    for (i = 0; i <= d_nfilters; i++) {
        d_taps[i] = std::vector<float>(d_taps_per_filter, 0.0f);
        for (j = 0; j < taps_per_filter; j++) {
            k = i + j * d_nfilters;
            if (k < m)
                d_taps[i][j] = taps[k];
        }
        d_filters.emplace_back(d_taps[i]);
        if (!d_derivative)
            continue;

        d_diff_taps[i] = std::vector<float>(d_taps_per_filter, 0.0f);
        for (j = 0; j < taps_per_filter; j++) {
            k = i + j * d_nfilters;
            if (k < n)
                d_diff_taps[i][j] = diff_taps[k];
        }
        d_diff_filters.emplace_back(d_diff_taps[i]);
    }
}

interp_resampler_pfb_mf_fff::~interp_resampler_pfb_mf_fff() {}

float interp_resampler_pfb_mf_fff::interpolate(const float input[], float mu) const
{
    int arm = static_cast<int>(rint(mu * d_nfilters));

    if (arm < 0 || arm > d_nfilters)
        throw std::runtime_error("interp_resampler_pfb_mf_fff: mu is not "
                                 "in the range [0.0, 1.0]");

    return d_filters[arm].filter(input);
}

float interp_resampler_pfb_mf_fff::differentiate(const float input[], float mu) const
{
    int arm = static_cast<int>(rint(mu * d_nfilters));

    if (arm < 0 || arm > d_nfilters)
        throw std::runtime_error("interp_resampler_pfb_mf_fff: mu is not "
                                 "in the range [0.0, 1.0]");

    return d_diff_filters[arm].filter(input);
}

unsigned int interp_resampler_pfb_mf_fff::ntaps() const { return d_taps_per_filter; }

} /* namespace digital */
} /* namespace gr */

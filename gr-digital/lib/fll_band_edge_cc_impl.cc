/* -*- c++ -*- */
/*
 * Copyright 2009-2012,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "fll_band_edge_cc_impl.h"
#include <gnuradio/expj.h>
#include <gnuradio/io_signature.h>
#include <gnuradio/thread/thread.h>
#include <type_traits>
#include <cmath>
#include <complex>
#include <iomanip>
#include <memory>
#include <sstream>

namespace gr {
namespace digital {

#define M_TWOPI (2 * M_PI)

float sinc(float x)
{
    // TODO: this seems to be numerically problematic for extremely long filters
    // TODO: investigate (-epsilon < x < epsilon)
    const float arg = static_cast<float>(M_PI) * x;
    return x == 0.0f ? 1.0f : std::sin(arg) / arg;
}

fll_band_edge_cc::sptr fll_band_edge_cc::make(float samps_per_sym,
                                              float rolloff,
                                              int filter_size,
                                              float bandwidth)
{
    return gnuradio::make_block_sptr<fll_band_edge_cc_impl>(
        samps_per_sym, rolloff, filter_size, bandwidth);
}

static int ios[] = { sizeof(gr_complex), sizeof(float), sizeof(float), sizeof(float) };
static std::vector<int> iosig(ios, ios + sizeof(ios) / sizeof(int));
fll_band_edge_cc_impl::fll_band_edge_cc_impl(float samps_per_sym,
                                             float rolloff,
                                             int filter_size,
                                             float bandwidth)
    : sync_block("fll_band_edge_cc",
                 io_signature::make(1, 1, sizeof(gr_complex)),
                 io_signature::makev(1, 4, iosig)),
      blocks::control_loop(
          bandwidth, M_TWOPI * (2.0 / samps_per_sym), -M_TWOPI * (2.0 / samps_per_sym)),
      d_sps(samps_per_sym),
      d_rolloff(rolloff),
      d_filter_size(filter_size)
{
    // Value-check samples per symbol
    if (samps_per_sym <= 0) {
        throw std::out_of_range("fll_band_edge_cc: invalid number of sps. Must be > 0.");
    }

    // Value-check rolloff factor
    if (rolloff < 0 || rolloff > 1.0) {
        throw std::out_of_range(
            "fll_band_edge_cc: invalid rolloff factor. Must be in [0,1].");
    }

    // Value-check filter length
    if (filter_size <= 0) {
        throw std::out_of_range("fll_band_edge_cc: invalid filter size. Must be > 0.");
    }

    // Value-check bandwidth
    if (samps_per_sym <= 0) {
        throw std::out_of_range("fll_band_edge_cc: invalid bandwidth. Must be > 0.");
    }

    // Build the band edge filters
    design_filter();
}

/*******************************************************************
 SET FUNCTIONS
*******************************************************************/

void fll_band_edge_cc_impl::set_samples_per_symbol(float sps)
{
    if (sps <= 0) {
        throw std::out_of_range("fll_band_edge_cc: invalid number of sps. Must be > 0.");
    }
    {
        /* local lock - release before calling design_filter, such that other setter calls
         * might also called before the large mutex'ed section in design_filter. */
        gr::thread::scoped_lock lock(d_setlock);
        d_sps = sps;
    }
    set_max_freq(M_TWOPI * (2.0 / sps));
    set_min_freq(-M_TWOPI * (2.0 / sps));
    design_filter();
}

void fll_band_edge_cc_impl::set_rolloff(float rolloff)
{
    if (rolloff < 0 || rolloff > 1.0) {
        throw std::out_of_range(
            "fll_band_edge_cc: invalid rolloff factor. Must be in [0,1].");
    }
    {
        /* local lock - release before calling design_filter, such that other setter calls
         * might also called before the large mutex'ed section in design_filter. */
        gr::thread::scoped_lock lock(d_setlock);
        d_rolloff = rolloff;
    }
    design_filter();
}

void fll_band_edge_cc_impl::set_filter_size(int filter_size)
{
    if (filter_size <= 0) {
        throw std::out_of_range("fll_band_edge_cc: invalid filter size. Must be > 0.");
    }
    {
        /* local lock - release before calling design_filter, such that other setter calls
         * might also called before the large mutex'ed section in design_filter. */
        gr::thread::scoped_lock lock(d_setlock);
        d_filter_size = filter_size;
    }
    design_filter();
}

/*******************************************************************
 GET FUNCTIONS
*******************************************************************/

float fll_band_edge_cc_impl::samples_per_symbol() const
{
    gr::thread::scoped_lock lock(d_setlock);
    return d_sps;
}

float fll_band_edge_cc_impl::rolloff() const
{
    gr::thread::scoped_lock lock(d_setlock);
    return d_rolloff;
}

int fll_band_edge_cc_impl::filter_size() const
{
    gr::thread::scoped_lock lock(d_setlock);
    return d_filter_size;
}

/*******************************************************************
 *******************************************************************/

void fll_band_edge_cc_impl::design_filter()
{
    gr::thread::scoped_lock lock(d_setlock);
    const int M = rintf(static_cast<float>(d_filter_size) / d_sps);
    float power = 0.0f;

    // Create the baseband filter by adding two sincs together
    std::vector<float> bb_taps;
    bb_taps.reserve(d_filter_size);
    const float half_sps_inv = 2.0f / d_sps;
    for (size_t i = 0; i < d_filter_size; i++) {
        const float k = -M + i * half_sps_inv;
        const float position = d_rolloff * k;
        const float tap = sinc(position - 0.5f) + sinc(position + 0.5f);
        power += tap * tap;

        bb_taps.push_back(tap);
    }

    d_taps_lower.resize(d_filter_size);
    d_taps_upper.resize(d_filter_size);

    // Create the band edge filters by spinning the baseband
    // filter up and down to the right places in frequency.
    // Also, normalize the power in the filters
    using signed_type = std::make_signed<decltype(d_filter_size)>::type;
    const signed_type N = (bb_taps.size() - 1) / 2;
    const float invpower = 1.0f / power;
    const float inv_twice_sps = 0.5f / d_sps;
    for (decltype(d_filter_size) i = 0; i < d_filter_size; i++) {
        const float tap = bb_taps[i] * invpower;
        const float k = (static_cast<signed_type>(i) - N) * inv_twice_sps;

        const size_t index = d_filter_size - i - 1;
        d_taps_lower[index] = tap * gr_expj(-M_TWOPI * (1 + d_rolloff) * k);
        d_taps_upper[index] = std::conj(d_taps_lower[d_filter_size - i - 1]);
    }

    d_updated = true;

    // Set the history to ensure enough input items for each filter
    set_history(d_filter_size + 1);
    d_filter_upper =
        std::make_unique<gr::filter::kernel::fir_filter_with_buffer_ccc>(d_taps_upper);
    d_filter_lower =
        std::make_unique<gr::filter::kernel::fir_filter_with_buffer_ccc>(d_taps_lower);
}

void fll_band_edge_cc_impl::print_taps()
{
    gr::thread::scoped_lock lock(d_setlock);
    std::stringstream ss;
    ss << std::setprecision(4) << std::scientific;
    ss << "Upper Band-edge: [";
    for (const auto& tap : d_taps_upper)
        ss << " " << tap.real() << " + " << tap.imag() << "j,";
    ss << "]\n\n";

    ss << "Lower Band-edge: [";
    for (const auto& tap : d_taps_lower)
        ss << " " << tap.real() << " + " << tap.imag() << "j,";
    ss << "]\n";

    d_logger->info("{:s}", ss.str());
}

int fll_band_edge_cc_impl::work(int noutput_items,
                                gr_vector_const_void_star& input_items,
                                gr_vector_void_star& output_items)
{
    gr::thread::scoped_lock lock(d_setlock);
    if (d_updated) {
        d_updated = false;
        return 0; // history requirements may have changed.
    }

    const auto* in = reinterpret_cast<const gr_complex*>(input_items[0]);
    auto* out = reinterpret_cast<gr_complex*>(output_items[0]);

    for (int i = 0; i < noutput_items; i++) {
        const gr_complex nco_out = gr_expj(d_phase);
        out[i] = in[i] * nco_out;

        // Perform the dot product of the output with the filters
        const gr_complex out_upper = d_filter_lower->filter(out[i]);
        const gr_complex out_lower = d_filter_upper->filter(out[i]);

        const float error = norm(out_lower) - norm(out_upper);

        advance_loop(error);
        phase_wrap();
        frequency_limit();

        if (output_items.size() == 4) {
            reinterpret_cast<float*>(output_items[1])[i] = d_freq;
            reinterpret_cast<float*>(output_items[2])[i] = d_phase;
            reinterpret_cast<float*>(output_items[3])[i] = error;
        }
    }

    return noutput_items;
}

} /* namespace digital */
} /* namespace gr */

/* -*- c++ -*- */
/* Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/digital/ofdm_equalizer_simpledfe.h>

namespace gr {
namespace digital {

ofdm_equalizer_simpledfe::sptr
ofdm_equalizer_simpledfe::make(int fft_len,
                               const gr::digital::constellation_sptr& constellation,
                               const std::vector<std::vector<int>>& occupied_carriers,
                               const std::vector<std::vector<int>>& pilot_carriers,
                               const std::vector<std::vector<gr_complex>>& pilot_symbols,
                               int symbols_skipped,
                               float alpha,
                               bool input_is_shifted,
                               bool enable_soft_output)
{
    return ofdm_equalizer_simpledfe::sptr(
        new ofdm_equalizer_simpledfe(fft_len,
                                     constellation,
                                     occupied_carriers,
                                     pilot_carriers,
                                     pilot_symbols,
                                     symbols_skipped,
                                     alpha,
                                     input_is_shifted,
                                     enable_soft_output));
}

ofdm_equalizer_simpledfe::ofdm_equalizer_simpledfe(
    int fft_len,
    const gr::digital::constellation_sptr& constellation,
    const std::vector<std::vector<int>>& occupied_carriers,
    const std::vector<std::vector<int>>& pilot_carriers,
    const std::vector<std::vector<gr_complex>>& pilot_symbols,
    int symbols_skipped,
    float alpha,
    bool input_is_shifted,
    bool enable_soft_output)
    : ofdm_equalizer_1d_pilots(fft_len,
                               occupied_carriers,
                               pilot_carriers,
                               pilot_symbols,
                               symbols_skipped,
                               input_is_shifted),
      d_constellation(constellation),
      d_alpha(alpha),
      d_enable_soft_output(enable_soft_output)
{
}


ofdm_equalizer_simpledfe::~ofdm_equalizer_simpledfe() {}


void ofdm_equalizer_simpledfe::equalize(gr_complex* frame,
                                        int n_sym,
                                        const std::vector<gr_complex>& initial_taps,
                                        const std::vector<tag_t>& tags)
{
    if (!initial_taps.empty()) {
        d_channel_state = initial_taps;
    }
    gr_complex sym_eq, sym_est;
    bool enable_soft_output = d_enable_soft_output;

    for (int i = 0; i < n_sym; i++) {
        for (int k = 0; k < d_fft_len; k++) {
            if (!d_occupied_carriers[k]) {
                continue;
            }
            if (!d_pilot_carriers.empty() && d_pilot_carriers[d_pilot_carr_set][k]) {
                d_channel_state[k] = d_alpha * d_channel_state[k] +
                                     (1 - d_alpha) * frame[i * d_fft_len + k] /
                                         d_pilot_symbols[d_pilot_carr_set][k];
                frame[i * d_fft_len + k] = d_pilot_symbols[d_pilot_carr_set][k];
            } else {
                sym_eq = frame[i * d_fft_len + k] / d_channel_state[k];
                // The `map_to_points` function will treat `sym_est` as an array
                // pointer.  This call is "safe" because `map_to_points` is limited
                // by the dimensionality of the constellation. This class calls the
                // `constellation` class default constructor, which initializes the
                // dimensionality value to `1`. Thus, Only the single `gr_complex`
                // value will be dereferenced.
                d_constellation->map_to_points(d_constellation->decision_maker(&sym_eq),
                                               &sym_est);
                d_channel_state[k] = d_alpha * d_channel_state[k] +
                                     (1 - d_alpha) * frame[i * d_fft_len + k] / sym_est;
                frame[i * d_fft_len + k] = enable_soft_output ? sym_eq : sym_est;
            }
        }
        if (!d_pilot_carriers.empty()) {
            d_pilot_carr_set = (d_pilot_carr_set + 1) % d_pilot_carriers.size();
        }
    }
}

} /* namespace digital */
} /* namespace gr */

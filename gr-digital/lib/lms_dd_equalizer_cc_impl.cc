/* -*- c++ -*- */
/*
 * Copyright 2011,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "lms_dd_equalizer_cc_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/misc.h>
#include <volk/volk.h>

namespace gr {
namespace digital {

using namespace filter::kernel;

lms_dd_equalizer_cc::sptr
lms_dd_equalizer_cc::make(int num_taps, float mu, int sps, constellation_sptr cnst)
{
    return gnuradio::make_block_sptr<lms_dd_equalizer_cc_impl>(num_taps, mu, sps, cnst);
}

lms_dd_equalizer_cc_impl::lms_dd_equalizer_cc_impl(int num_taps,
                                                   float mu,
                                                   int sps,
                                                   constellation_sptr cnst)
    : sync_decimator("lms_dd_equalizer_cc",
                     io_signature::make(1, 1, sizeof(gr_complex)),
                     io_signature::make(1, 1, sizeof(gr_complex)),
                     sps),
      fir_filter_ccc(std::vector<gr_complex>(num_taps, gr_complex(0, 0))),
      d_new_taps(num_taps, gr_complex(0, 0)),
      d_updated(false),
      d_cnst(cnst)
{
    set_gain(mu);
    if (num_taps > 0)
        d_new_taps[0] = 1.0;
    fir_filter_ccc::set_taps(d_new_taps);

    const int alignment_multiple = volk_get_alignment() / sizeof(gr_complex);
    set_alignment(std::max(1, alignment_multiple));

    set_history(num_taps);
}

lms_dd_equalizer_cc_impl::~lms_dd_equalizer_cc_impl() {}

void lms_dd_equalizer_cc_impl::set_taps(const std::vector<gr_complex>& taps)
{
    d_new_taps = taps;
    d_updated = true;
}

std::vector<gr_complex> lms_dd_equalizer_cc_impl::taps() const { return d_taps; }

gr_complex lms_dd_equalizer_cc_impl::error(const gr_complex& out)
{
    gr_complex decision, error;
    // The `map_to_points` function will treat `decision` as an array pointer.
    // This call is "safe" because `map_to_points` is limited by the
    // dimensionality of the constellation. This class calls the
    // `constellation` class default constructor, which initializes the
    // dimensionality value to `1`. Thus, Only the single `gr_complex` value
    // will be dereferenced.
    d_cnst->map_to_points(d_cnst->decision_maker(&out), &decision);
    error = decision - out;
    return error;
}

void lms_dd_equalizer_cc_impl::update_tap(gr_complex& tap, const gr_complex& in)
{
    tap += d_mu * conj(in) * d_error;
}

int lms_dd_equalizer_cc_impl::work(int noutput_items,
                                   gr_vector_const_void_star& input_items,
                                   gr_vector_void_star& output_items)
{
    const gr_complex* in = (const gr_complex*)input_items[0];
    gr_complex* out = (gr_complex*)output_items[0];

    if (d_updated) {
        d_taps = d_new_taps;
        set_history(d_taps.size());
        d_updated = false;
        return 0; // history requirements may have changed.
    }

    int j = 0;
    size_t k, l = d_taps.size();
    for (int i = 0; i < noutput_items; i++) {
        out[i] = filter(&in[j]);

        // Adjust taps
        d_error = error(out[i]);
        for (k = 0; k < l; k++) {
            // Update tap locally from error.
            update_tap(d_taps[k], in[j + k]);

            // Update aligned taps in filter object.
            fir_filter_ccc::update_tap(d_taps[k], k);
        }

        j += decimation();
    }

    return noutput_items;
}

} /* namespace digital */
} /* namespace gr */

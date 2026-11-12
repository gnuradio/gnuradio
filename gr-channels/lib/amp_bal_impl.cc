/* -*- c++ -*- */
/*
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "amp_bal_impl.h"
#include <gnuradio/io_signature.h>
#include <cmath>
#include <stdexcept>

namespace gr {
namespace channels {

amp_bal::sptr amp_bal::make(double alpha)
{
    return gnuradio::make_block_sptr<amp_bal_impl>(alpha);
}

amp_bal_impl::amp_bal_impl(double alpha)
    : sync_block("amp_bal",
                 io_signature::make(1, 1, sizeof(gr_complex)),
                 io_signature::make(1, 1, sizeof(gr_complex))),
      d_avg_i(0),
      d_avg_q(0)
{
    set_alpha(alpha);
}

void amp_bal_impl::set_alpha(double alpha)
{
    if (!(alpha > 0 && alpha <= 1)) {
        throw std::invalid_argument("amp_bal: alpha must be in (0, 1]");
    }
    d_alpha = alpha;
}

double amp_bal_impl::alpha() const { return d_alpha; }

int amp_bal_impl::work(int noutput_items,
                       gr_vector_const_void_star& input_items,
                       gr_vector_void_star& output_items)
{
    const gr_complex* in = (const gr_complex*)input_items[0];
    gr_complex* out = (gr_complex*)output_items[0];

    double avg_i = d_avg_i;
    double avg_q = d_avg_q;
    for (int i = 0; i < noutput_items; i++) {
        const float re = in[i].real();
        const float im = in[i].imag();
        avg_i = (1 - d_alpha) * avg_i + d_alpha * re * re;
        avg_q = (1 - d_alpha) * avg_q + d_alpha * im * im;
        // alpha in (0, 1]: averages are never negative, and avg_q is zero only
        // while Q has been zero, when the output Q is zero for any gain
        const float gain =
            static_cast<float>(avg_q) > 0
                ? std::sqrt(static_cast<float>(avg_i) / static_cast<float>(avg_q))
                : 0;
        out[i] = gr_complex(re, im * gain);
    }
    d_avg_i = avg_i;
    d_avg_q = avg_q;
    return noutput_items;
}

} /* namespace channels */
} /* namespace gr */

/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "meas_evm_cc_impl.h"
#include <gnuradio/io_signature.h>
#include <numeric>

namespace gr {
namespace digital {

meas_evm_cc::sptr meas_evm_cc::make(constellation_sptr cons, evm_measurement_t meas_type)
{
    return gnuradio::make_block_sptr<meas_evm_cc_impl>(cons, meas_type);
}

meas_evm_cc_impl::meas_evm_cc_impl(constellation_sptr cons, evm_measurement_t meas_type)
    : gr::sync_block("meas_evm_cc",
                     gr::io_signature::make(1, 1, sizeof(gr_complex)),
                     gr::io_signature::make(1, 1, sizeof(float))),
      d_cons(cons),
      d_cons_points(d_cons->points()),
      d_meas_type(meas_type)
{
    // Calculate the average power of the constellation
    float cons_mag_sq =
        std::accumulate(d_cons_points.begin(),
                        d_cons_points.end(),
                        0.0,
                        [](float s, gr_complex e) -> float { return s + std::norm(e); }) /
        d_cons_points.size();
    d_cons_mag = sqrt(cons_mag_sq);
}

meas_evm_cc_impl::~meas_evm_cc_impl() {}

int meas_evm_cc_impl::work(int noutput_items,
                           gr_vector_const_void_star& input_items,
                           gr_vector_void_star& output_items)
{
    auto in = static_cast<const gr_complex*>(input_items[0]);
    auto out = static_cast<float*>(output_items[0]);

    // Compare incoming symbols to the constellation decision points
    for (int s = 0; s < noutput_items; s++) {
        gr_complex u_n = in[s];
        gr_complex decision;
        d_cons->map_to_points(d_cons->decision_maker(&u_n), &decision);
        float err_mag = std::abs(decision - u_n);

        if (d_meas_type == evm_measurement_t::EVM_PERCENT) {
            out[s] = (err_mag / d_cons_mag) * 100.0;
        } else {
            out[s] = 20 * log10(err_mag / d_cons_mag);
        }
    }

    return noutput_items;
}

} // namespace digital
} /* namespace gr */

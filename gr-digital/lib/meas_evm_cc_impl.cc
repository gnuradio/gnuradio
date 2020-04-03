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

namespace gr {
namespace digital {

meas_evm_cc::sptr meas_evm_cc::make(constellation_sptr cons, evm_measurement_t meas_type)
{
    return gnuradio::get_initial_sptr(new meas_evm_cc_impl(cons, meas_type));
}

meas_evm_cc_impl::meas_evm_cc_impl(constellation_sptr cons, evm_measurement_t meas_type)
    : gr::sync_block("meas_evm_cc",
                     gr::io_signature::make(1, 1, sizeof(gr_complex)),
                     gr::io_signature::make(1, 1, sizeof(float))),
      d_cons(cons),
      d_meas_type(meas_type)
{

    d_cons_points = d_cons->points();

    // Calculate the average power of the constellation
    float sum = 0.0;
    int N = d_cons_points.size();
    for (int i = 0; i < N; i++) {
        sum += std::norm(d_cons_points[i]);
    }
    sum /= (float)N;
    d_cons_mag_sq = sum;
    d_cons_mag = sqrt(sum);
}

meas_evm_cc_impl::~meas_evm_cc_impl() {}

int meas_evm_cc_impl::work(int noutput_items,
                           gr_vector_const_void_star& input_items,
                           gr_vector_void_star& output_items)
{
    const gr_complex* in = (const gr_complex*)input_items[0];
    float* out = (float*)output_items[0];

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

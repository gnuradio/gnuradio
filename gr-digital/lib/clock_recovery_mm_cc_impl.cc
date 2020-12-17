/* -*- c++ -*- */
/*
 * Copyright 2005,2006,2010-2012,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "clock_recovery_mm_cc_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/math.h>
#include <gnuradio/prefs.h>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace gr {
namespace digital {

static const int FUDGE = 16;

clock_recovery_mm_cc::sptr clock_recovery_mm_cc::make(
    float omega, float gain_omega, float mu, float gain_mu, float omega_relative_limit)
{
    return gnuradio::make_block_sptr<clock_recovery_mm_cc_impl>(
        omega, gain_omega, mu, gain_mu, omega_relative_limit);
}

clock_recovery_mm_cc_impl::clock_recovery_mm_cc_impl(
    float omega, float gain_omega, float mu, float gain_mu, float omega_relative_limit)
    : block("clock_recovery_mm_cc",
            io_signature::make(1, 1, sizeof(gr_complex)),
            io_signature::make2(1, 2, sizeof(gr_complex), sizeof(float))),
      d_mu(mu),
      d_omega(omega),
      d_gain_omega(gain_omega),
      d_omega_relative_limit(omega_relative_limit),
      d_gain_mu(gain_mu),
      d_last_sample(0),
      d_verbose(prefs::singleton()->get_bool("clock_recovery_mm_cc", "verbose", false)),
      d_p_2T(0),
      d_p_1T(0),
      d_p_0T(0),
      d_c_2T(0),
      d_c_1T(0),
      d_c_0T(0)
{
    if (omega <= 0.0)
        throw std::out_of_range("clock rate must be > 0");
    if (gain_mu < 0 || gain_omega < 0)
        throw std::out_of_range("Gains must be non-negative");

    set_omega(omega); // also sets min and max omega
    set_inverse_relative_rate(omega);
    set_history(3);           // ensure 2 extra input samples are available
    enable_update_rate(true); // fixes tag propagation through variable rate block
}

clock_recovery_mm_cc_impl::~clock_recovery_mm_cc_impl() {}

void clock_recovery_mm_cc_impl::forecast(int noutput_items,
                                         gr_vector_int& ninput_items_required)
{
    unsigned ninputs = ninput_items_required.size();
    for (unsigned i = 0; i < ninputs; i++)
        ninput_items_required[i] =
            (int)ceil((noutput_items * d_omega) + d_interp.ntaps()) + FUDGE;
}

void clock_recovery_mm_cc_impl::set_omega(float omega)
{
    d_omega = omega;
    d_omega_mid = omega;
    d_omega_lim = d_omega_relative_limit * omega;
}

int clock_recovery_mm_cc_impl::general_work(int noutput_items,
                                            gr_vector_int& ninput_items,
                                            gr_vector_const_void_star& input_items,
                                            gr_vector_void_star& output_items)
{
    const gr_complex* in = (const gr_complex*)input_items[0];
    gr_complex* out = (gr_complex*)output_items[0];

    bool write_foptr = output_items.size() >= 2;

    int ii = 0;                                          // input index
    int oo = 0;                                          // output index
    int ni = ninput_items[0] - d_interp.ntaps() - FUDGE; // don't use more input than this

    assert(d_mu >= 0.0);
    assert(d_mu <= 1.0);

    float mm_val = 0;
    gr_complex u, x, y;

    // This loop writes the error to the second output, if it exists
    if (write_foptr) {
        float* foptr = (float*)output_items[1];
        while (oo < noutput_items && ii < ni) {
            d_p_2T = d_p_1T;
            d_p_1T = d_p_0T;
            d_p_0T = d_interp.interpolate(&in[ii], d_mu);

            d_c_2T = d_c_1T;
            d_c_1T = d_c_0T;
            d_c_0T = slicer_0deg(d_p_0T);

            fast_cc_multiply(x, d_c_0T - d_c_2T, conj(d_p_1T));
            fast_cc_multiply(y, d_p_0T - d_p_2T, conj(d_c_1T));
            u = y - x;
            mm_val = u.real();
            out[oo++] = d_p_0T;

            // limit mm_val
            mm_val = gr::branchless_clip(mm_val, 1.0);
            d_omega = d_omega + d_gain_omega * mm_val;
            d_omega =
                d_omega_mid + gr::branchless_clip(d_omega - d_omega_mid, d_omega_lim);

            d_mu = d_mu + d_omega + d_gain_mu * mm_val;
            ii += (int)floor(d_mu);
            d_mu -= floor(d_mu);

            // write the error signal to the second output
            foptr[oo - 1] = mm_val;

            if (ii < 0) // clamp it.  This should only happen with bogus input
                ii = 0;
        }
    }
    // This loop does not write to the second output (ugly, but faster)
    else {
        while (oo < noutput_items && ii < ni) {
            d_p_2T = d_p_1T;
            d_p_1T = d_p_0T;
            d_p_0T = d_interp.interpolate(&in[ii], d_mu);

            d_c_2T = d_c_1T;
            d_c_1T = d_c_0T;
            d_c_0T = slicer_0deg(d_p_0T);

            fast_cc_multiply(x, d_c_0T - d_c_2T, conj(d_p_1T));
            fast_cc_multiply(y, d_p_0T - d_p_2T, conj(d_c_1T));
            u = y - x;
            mm_val = u.real();
            out[oo++] = d_p_0T;

            // limit mm_val
            mm_val = gr::branchless_clip(mm_val, 1.0);

            d_omega = d_omega + d_gain_omega * mm_val;
            d_omega =
                d_omega_mid + gr::branchless_clip(d_omega - d_omega_mid, d_omega_lim);

            d_mu = d_mu + d_omega + d_gain_mu * mm_val;
            ii += (int)floor(d_mu);
            d_mu -= floor(d_mu);

            if (d_verbose) {
                std::stringstream tmp;
                tmp << std::setprecision(8) << std::fixed << d_omega << "\t" << d_mu
                    << std::endl;
                GR_LOG_INFO(d_logger, tmp.str());
            }

            if (ii < 0) // clamp it.  This should only happen with bogus input
                ii = 0;
        }
    }

    if (ii > 0) {
        assert(ii <= ninput_items[0]);
        consume_each(ii);
    }

    return oo;
}

} /* namespace digital */
} /* namespace gr */

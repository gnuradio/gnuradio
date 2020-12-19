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
#include <config.h>
#endif

#include "mpsk_snr_est_cc_impl.h"
#include <gnuradio/io_signature.h>
#include <cstdio>
#include <memory>

namespace gr {
namespace digital {

namespace {
std::unique_ptr<mpsk_snr_est> choose_type(snr_est_type_t t, double d_alpha)
{
    switch (t) {
    case (SNR_EST_SIMPLE):
        return std::make_unique<mpsk_snr_est_simple>(d_alpha);
    case (SNR_EST_SKEW):
        return std::make_unique<mpsk_snr_est_skew>(d_alpha);
    case (SNR_EST_M2M4):
        return std::make_unique<mpsk_snr_est_m2m4>(d_alpha);
    case (SNR_EST_SVR):
        return std::make_unique<mpsk_snr_est_svr>(d_alpha);
    }
    throw std::invalid_argument("mpsk_snr_est_cc_impl: unknown type specified.");
}
} // namespace

mpsk_snr_est_cc::sptr
mpsk_snr_est_cc::make(snr_est_type_t type, int tag_nsamples, double alpha)
{
    return gnuradio::make_block_sptr<mpsk_snr_est_cc_impl>(type, tag_nsamples, alpha);
}

mpsk_snr_est_cc_impl::mpsk_snr_est_cc_impl(snr_est_type_t type,
                                           int tag_nsamples,
                                           double alpha)
    : sync_block("mpsk_snr_est_cc",
                 io_signature::make(1, 1, sizeof(gr_complex)),
                 io_signature::make(1, 1, sizeof(gr_complex))),
      d_type(type),
      d_nsamples(tag_nsamples),
      d_count(0)
{
    set_alpha(alpha);

    set_type(type);

    // at least 1 estimator has to look back
    set_history(2);

    std::stringstream str;
    str << name() << unique_id();
    d_me = pmt::string_to_symbol(str.str());
    d_key = pmt::string_to_symbol("snr");
}

mpsk_snr_est_cc_impl::~mpsk_snr_est_cc_impl() {}

int mpsk_snr_est_cc_impl::work(int noutput_items,
                               gr_vector_const_void_star& input_items,
                               gr_vector_void_star& output_items)
{
    // This is a pass-through block; copy input to output
    memcpy(output_items[0], input_items[0], noutput_items * sizeof(gr_complex));

    const gr_complex* in = (const gr_complex*)input_items[0];

    // Update, calculate, and issue an SNR tag every d_nsamples
    int index = 0, x = 0;
    int64_t nwritten = nitems_written(0);
    while (index + (d_nsamples - d_count) < noutput_items) {
        x = d_nsamples - d_count;
        nwritten += x;

        // Update the SNR estimate registers from the current input
        d_snr_est->update(x, &in[index]);

        // Issue a tag with the SNR data
        pmt::pmt_t pmt_snr = pmt::from_double(d_snr_est->snr());
        add_item_tag(0,        // stream ID
                     nwritten, // tag's sample number
                     d_key,    // snr key
                     pmt_snr,  // SNR
                     d_me);    // block src id

        index += x;
        d_count = 0;
    }

    // Keep track of remaining items and update estimators
    x = noutput_items - index;
    d_count += x;
    d_snr_est->update(x, &in[index]);

    return noutput_items;
}

double mpsk_snr_est_cc_impl::snr()
{
    if (d_snr_est)
        return d_snr_est->snr();
    else
        throw std::runtime_error("mpsk_snr_est_cc_impl:: No SNR estimator defined.");
}

snr_est_type_t mpsk_snr_est_cc_impl::type() const { return d_type; }

int mpsk_snr_est_cc_impl::tag_nsample() const { return d_nsamples; }

double mpsk_snr_est_cc_impl::alpha() const { return d_alpha; }

void mpsk_snr_est_cc_impl::set_type(snr_est_type_t t)
{
    d_snr_est = choose_type(t, d_alpha);
    d_type = t;
}

void mpsk_snr_est_cc_impl::set_tag_nsample(int n)
{
    if (n > 0) {
        d_nsamples = n;
        d_count = 0; // reset state
    } else
        throw std::invalid_argument("mpsk_snr_est_cc_impl: tag_nsamples can't be <= 0");
}

void mpsk_snr_est_cc_impl::set_alpha(double alpha)
{
    if ((alpha >= 0) && (alpha <= 1.0)) {
        d_alpha = alpha;
        if (d_snr_est)
            d_snr_est->set_alpha(d_alpha);
    } else
        throw std::invalid_argument("mpsk_snr_est_cc_impl: alpha must be in [0,1]");
}

} /* namespace digital */
} /* namespace gr */

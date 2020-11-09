/* -*- c++ -*- */
/*
 * Copyright 2011,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_MPSK_SNR_EST_CC_IMPL_H
#define INCLUDED_DIGITAL_MPSK_SNR_EST_CC_IMPL_H

#include <gnuradio/digital/api.h>
#include <gnuradio/digital/mpsk_snr_est_cc.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace digital {

class mpsk_snr_est_cc_impl : public mpsk_snr_est_cc
{
private:
    snr_est_type_t d_type;
    int d_nsamples, d_count;
    double d_alpha;
    std::unique_ptr<mpsk_snr_est> d_snr_est;

    // d_key is the tag name, 'snr', d_me is the block name + unique ID
    pmt::pmt_t d_key, d_me;

public:
    mpsk_snr_est_cc_impl(snr_est_type_t type,
                         int tag_nsamples = 10000,
                         double alpha = 0.001);
    ~mpsk_snr_est_cc_impl() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;

    //! Return the estimated signal-to-noise ratio in decibels
    double snr() override;

    //! Return the type of estimator in use
    snr_est_type_t type() const override;

    //! Return how many samples between SNR tags
    int tag_nsample() const override;

    //! Get the running-average coefficient
    double alpha() const override;

    //! Set type of estimator to use
    void set_type(snr_est_type_t t) override;

    //! Set the number of samples between SNR tags
    void set_tag_nsample(int n) override;

    //! Set the running-average coefficient
    void set_alpha(double alpha) override;
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_MPSK_SNR_EST_CC_IMPL_H */

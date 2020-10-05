/* -*- c++ -*- */
/*
 * Copyright 2004,2007,2012-2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_MMSE_RESAMPLER_FF_IMPL_H
#define INCLUDED_MMSE_RESAMPLER_FF_IMPL_H

#include <gnuradio/filter/mmse_fir_interpolator_ff.h>
#include <gnuradio/filter/mmse_resampler_ff.h>

namespace gr {
namespace filter {

class FILTER_API mmse_resampler_ff_impl : public mmse_resampler_ff
{
private:
    double d_mu;
    double d_mu_inc;
    const mmse_fir_interpolator_ff d_resamp;

public:
    mmse_resampler_ff_impl(float phase_shift, float resamp_ratio);

    void handle_msg(pmt::pmt_t msg);

    void forecast(int noutput_items, gr_vector_int& ninput_items_required) override;
    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;

    float mu() const override;
    float resamp_ratio() const override;
    void set_mu(float mu) override;
    void set_resamp_ratio(float resamp_ratio) override;
};

} /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_MMSE_RESAMPLER_FF_IMPL_H */

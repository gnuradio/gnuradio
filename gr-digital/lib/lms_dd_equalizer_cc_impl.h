/* -*- c++ -*- */
/*
 * Copyright 2011,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_LMS_DD_EQUALIZER_CC_IMPL_H
#define INCLUDED_DIGITAL_LMS_DD_EQUALIZER_CC_IMPL_H

#include <gnuradio/digital/lms_dd_equalizer_cc.h>
#include <gnuradio/filter/fir_filter.h>
#include <stdexcept>

namespace gr {
namespace digital {

class lms_dd_equalizer_cc_impl : public lms_dd_equalizer_cc,
                                 filter::kernel::fir_filter_ccc
{
private:
    std::vector<gr_complex> d_new_taps;
    bool d_updated;
    gr_complex d_error;

    float d_mu;
    constellation_sptr d_cnst;

protected:
    gr_complex error(const gr_complex& out);
    void update_tap(gr_complex& tap, const gr_complex& in);

public:
    lms_dd_equalizer_cc_impl(int num_taps, float mu, int sps, constellation_sptr cnst);
    ~lms_dd_equalizer_cc_impl();

    void set_taps(const std::vector<gr_complex>& taps);
    std::vector<gr_complex> taps() const;

    float gain() const { return d_mu; }

    void set_gain(float mu)
    {
        if (mu < 0.0f || mu > 1.0f) {
            throw std::out_of_range(
                "lms_dd_equalizer_impl::set_mu: Gain value must in [0, 1]");
        } else {
            d_mu = mu;
        }
    }

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_LMS_DD_EQUALIZER_CC_IMPL_H */

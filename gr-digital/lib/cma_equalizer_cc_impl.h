/* -*- c++ -*- */
/*
 * Copyright 2011,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_CMA_EQUALIZER_CC_IMPL_H
#define INCLUDED_DIGITAL_CMA_EQUALIZER_CC_IMPL_H

#include <gnuradio/digital/cma_equalizer_cc.h>
#include <gnuradio/filter/fir_filter.h>
#include <gnuradio/math.h>
#include <stdexcept>

namespace gr {
namespace digital {

class cma_equalizer_cc_impl : public cma_equalizer_cc, filter::kernel::fir_filter_ccc
{
private:
    std::vector<gr_complex> d_new_taps;
    bool d_updated;
    gr_complex d_error;

    float d_modulus;
    float d_mu;

protected:
    gr_complex error(const gr_complex& out);
    void update_tap(gr_complex& tap, const gr_complex& in);

public:
    cma_equalizer_cc_impl(int num_taps, float modulus, float mu, int sps);
    ~cma_equalizer_cc_impl();

    void set_taps(const std::vector<gr_complex>& taps);
    std::vector<gr_complex> taps() const;

    float gain() const { return d_mu; }

    void set_gain(float mu)
    {
        if (mu < 0.0f || mu > 1.0f) {
            throw std::out_of_range(
                "cma_equalizer::set_gain: Gain value must be in [0,1]");
        }
        d_mu = mu;
    }

    float modulus() const { return d_modulus; }

    void set_modulus(float mod)
    {
        if (mod < 0)
            throw std::out_of_range(
                "cma_equalizer::set_modulus: Modulus value must be >= 0");
        d_modulus = mod;
    }

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_CMA_EQUALIZER_CC_IMPL_H */

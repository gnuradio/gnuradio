/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_ADAPTIVE_ALGORITHM_NLMS_H
#define INCLUDED_DIGITAL_ADAPTIVE_ALGORITHM_NLMS_H

#include <gnuradio/digital/adaptive_algorithm.h>
#include <volk/volk.h>

namespace gr {
namespace digital {

class DIGITAL_API adaptive_algorithm_nlms : public adaptive_algorithm
{
public:
    typedef std::shared_ptr<adaptive_algorithm_nlms> sptr;

private:
    const float d_step_size;

protected:
    adaptive_algorithm_nlms(constellation_sptr cons, float step_size)
        : adaptive_algorithm(adaptive_algorithm_t::NLMS, cons), d_step_size(step_size)
    {
    }

public:
    static sptr make(constellation_sptr cons, float step_size)
    {
        return adaptive_algorithm_nlms::sptr(
            new adaptive_algorithm_nlms(cons, step_size));
    }

    gr_complex update_tap(const gr_complex tap,
                          const gr_complex& in,
                          const gr_complex error,
                          const gr_complex decision) override
    {
        throw std::runtime_error(
            "NLMS can only update all taps at once, single tap update is not valid");
    }

    void update_taps(gr_complex* taps,
                     const gr_complex* in,
                     const gr_complex error,
                     const gr_complex decision,
                     unsigned int num_taps) override
    {
        gr_complex dp;
        volk_32fc_x2_conjugate_dot_prod_32fc(&dp, in, in, num_taps);
        float magsq = real(dp);
        float norm_step_size = d_step_size / magsq;

        for (unsigned i = 0; i < num_taps; i++) {
            taps[i] = conj(conj(taps[i]) + norm_step_size * in[i] * conj(error));
        }
    }

    ~adaptive_algorithm_nlms() override {}

    void initialize_taps(std::vector<gr_complex>& taps) override
    {
        std::fill(taps.begin(), taps.end(), gr_complex(0.0, 0.0));
    }
};

} // namespace digital
} // namespace gr

#endif
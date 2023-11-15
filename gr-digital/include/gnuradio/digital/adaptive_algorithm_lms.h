/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_ADAPTIVE_ALGORITHM_LMS_H
#define INCLUDED_DIGITAL_ADAPTIVE_ALGORITHM_LMS_H

#include <gnuradio/digital/adaptive_algorithm.h>
#include <volk/volk.h>
#include <volk/volk_alloc.hh>

namespace gr {
namespace digital {

class DIGITAL_API adaptive_algorithm_lms : public adaptive_algorithm
{
public:
    typedef std::shared_ptr<adaptive_algorithm_lms> sptr;

private:
    const float d_step_size;

protected:
    adaptive_algorithm_lms(constellation_sptr cons, float step_size)
        : adaptive_algorithm(adaptive_algorithm_t::LMS, cons), d_step_size(step_size)
    {
    }

public:
    static sptr make(constellation_sptr cons, float step_size)
    {
        return adaptive_algorithm_lms::sptr(new adaptive_algorithm_lms(cons, step_size));
    }

    ~adaptive_algorithm_lms() override {}

    void update_taps(gr_complex* taps,
                     const gr_complex* in,
                     const gr_complex error,
                     const gr_complex decision,
                     unsigned int num_taps) override
    {
        volk::vector<gr_complex> prod_vector(num_taps), conj_vector(num_taps);

        gr_complex err_x_mu = d_step_size * error;

        volk_32fc_conjugate_32fc(conj_vector.data(), in, num_taps);
#if VOLK_VERSION >= 030100
        volk_32fc_s32fc_multiply2_32fc(
            prod_vector.data(), conj_vector.data(), &err_x_mu, num_taps);
#else
        volk_32fc_s32fc_multiply_32fc(
            prod_vector.data(), conj_vector.data(), err_x_mu, num_taps);
#endif
        volk_32fc_x2_add_32fc(taps, taps, prod_vector.data(), num_taps);
    }

    gr_complex update_tap(const gr_complex tap,
                          const gr_complex& u_n,
                          const gr_complex err,
                          const gr_complex decision) override
    {
        return conj(conj(tap) + d_step_size * u_n * conj(err));
    }

    void initialize_taps(std::vector<gr_complex>& taps) override
    {
        std::fill(taps.begin(), taps.end(), gr_complex(0.0, 0.0));
    }
};

} // namespace digital
} // namespace gr

#endif

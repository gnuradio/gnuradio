/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_ADAPTIVE_ALGORITHM_CMA_H
#define INCLUDED_DIGITAL_ADAPTIVE_ALGORITHM_CMA_H

#include <gnuradio/digital/adaptive_algorithm.h>
#include <volk/volk.h>
#include <volk/volk_alloc.hh>

namespace gr {
namespace digital {
class DIGITAL_API adaptive_algorithm_cma : public adaptive_algorithm
{
public:
    typedef std::shared_ptr<adaptive_algorithm_cma> sptr;

private:
    const float d_step_size;
    const float d_modulus;

protected:
    adaptive_algorithm_cma(constellation_sptr cons, float step_size, float modulus)
        : adaptive_algorithm(adaptive_algorithm_t::CMA, cons),
          d_step_size(step_size),
          d_modulus(modulus)
    {
    }

public:
    static sptr make(constellation_sptr cons, float step_size, float modulus)
    {
        return adaptive_algorithm_cma::sptr(
            new adaptive_algorithm_cma(cons, step_size, modulus));
    }

    gr_complex error(const gr_complex& out) const
    {
        gr_complex error = out * (norm(out) - d_modulus);
        float re = gr::clip(error.real(), 1.0);
        float im = gr::clip(error.imag(), 1.0);
        return gr_complex(re, im);
    }

    gr_complex error_dd(gr_complex& u_n, gr_complex& decision) const override
    {
        return error(u_n);
    }

    gr_complex error_tr(const gr_complex& u_n, const gr_complex& d_n) const override
    {
        return error(u_n);
    }

    void update_taps(gr_complex* taps,
                     const gr_complex* in,
                     const gr_complex error,
                     const gr_complex decision,
                     unsigned int num_taps) override
    {
        volk::vector<gr_complex> prod_vector(num_taps), conj_vector(num_taps);

        gr_complex err_x_mu = -d_step_size * error;

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
        return conj(conj(tap) - d_step_size * u_n * conj(err));
    }

    ~adaptive_algorithm_cma() override {}
};

} // namespace digital
} // namespace gr

#endif

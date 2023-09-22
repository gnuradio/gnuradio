/* -*- c++ -*- */
/*
 * Copyright 2008,2010,2012,2018 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "cpfsk_bc_impl.h"
#include <gnuradio/expj.h>
#include <gnuradio/io_signature.h>
#include <gnuradio/math.h>

namespace gr {
namespace analog {

cpfsk_bc::sptr cpfsk_bc::make(float k, float ampl, int samples_per_sym)
{
    return gnuradio::make_block_sptr<cpfsk_bc_impl>(k, ampl, samples_per_sym);
}

cpfsk_bc_impl::cpfsk_bc_impl(float k, float ampl, int samples_per_sym)
    : sync_interpolator("cpfsk_bc",
                        io_signature::make(1, 1, sizeof(char)),
                        io_signature::make(1, 1, sizeof(gr_complex)),
                        samples_per_sym)
{
    d_samples_per_sym = samples_per_sym;
    d_freq = k * GR_M_PI / samples_per_sym;
    d_ampl = ampl;
    d_phase = 0.0;
}

cpfsk_bc_impl::~cpfsk_bc_impl() {}

int cpfsk_bc_impl::work(int noutput_items,
                        gr_vector_const_void_star& input_items,
                        gr_vector_void_star& output_items)
{
    const char* in = (const char*)input_items[0];
    gr_complex* out = (gr_complex*)output_items[0];

    for (int i = 0; i < noutput_items / d_samples_per_sym; i++) {
        for (int j = 0; j < d_samples_per_sym; j++) {
            if (in[i] == 1)
                d_phase += d_freq;
            else
                d_phase -= d_freq;

            while (d_phase > (2.0 * GR_M_PI))
                d_phase -= (2.0 * GR_M_PI);
            while (d_phase < -(2.0 * GR_M_PI))
                d_phase += (2.0 * GR_M_PI);

            *out++ = gr_expj(d_phase) * d_ampl;
        }
    }

    return noutput_items;
}

} /* namespace analog */
} /* namespace gr */

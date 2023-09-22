/* -*- c++ -*- */
/*
 * Copyright 2005,2006,2010,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "phase_modulator_fc_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/sincos.h>
#include <cmath>

namespace gr {
namespace analog {

phase_modulator_fc::sptr phase_modulator_fc::make(double sensitivity)
{
    return gnuradio::make_block_sptr<phase_modulator_fc_impl>(sensitivity);
}

phase_modulator_fc_impl::phase_modulator_fc_impl(double sensitivity)
    : sync_block("phase_modulator_fc",
                 io_signature::make(1, 1, sizeof(float)),
                 io_signature::make(1, 1, sizeof(gr_complex))),
      d_sensitivity(sensitivity),
      d_phase(0)
{
}

phase_modulator_fc_impl::~phase_modulator_fc_impl() {}

int phase_modulator_fc_impl::work(int noutput_items,
                                  gr_vector_const_void_star& input_items,
                                  gr_vector_void_star& output_items)
{
    const float* in = (const float*)input_items[0];
    gr_complex* out = (gr_complex*)output_items[0];

    for (int i = 0; i < noutput_items; i++) {
        d_phase = d_sensitivity * in[i];
        float oi, oq;
        gr::sincosf(d_phase, &oq, &oi);
        out[i] = gr_complex(oi, oq);
    }

    return noutput_items;
}

} /* namespace analog */
} /* namespace gr */

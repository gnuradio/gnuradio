/* -*- c++ -*- */
/*
 * Copyright 2007,2009,2010 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "dpll_bb_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace analog {

dpll_bb::sptr dpll_bb::make(float period, float gain)
{
    return gnuradio::make_block_sptr<dpll_bb_impl>(period, gain);
}

dpll_bb_impl::dpll_bb_impl(float period, float gain)
    : sync_block("dpll_bb",
                 io_signature::make(1, 1, sizeof(char)),
                 io_signature::make(1, 1, sizeof(char))),
      d_restart(0),
      d_pulse_phase(0)
{
    d_pulse_frequency = 1.0 / period;
    d_gain = gain;
    d_decision_threshold = 1.0 - 0.5 * d_pulse_frequency;
}

dpll_bb_impl::~dpll_bb_impl() {}

int dpll_bb_impl::work(int noutput_items,
                       gr_vector_const_void_star& input_items,
                       gr_vector_void_star& output_items)
{
    const char* iptr = (const char*)input_items[0];
    char* optr = (char*)output_items[0];

    for (int i = 0; i < noutput_items; i++) {
        optr[i] = 0;
        if (iptr[i] == 1) {
            if (d_restart == 0) {
                d_pulse_phase = 1;
            } else {
                if (d_pulse_phase > 0.5)
                    d_pulse_phase += d_gain * (1.0 - d_pulse_phase);
                else
                    d_pulse_phase -= d_gain * d_pulse_phase;
            }
            d_restart = 3;
        }
        if (d_pulse_phase > d_decision_threshold) {
            d_pulse_phase -= 1.0;
            if (d_restart > 0) {
                d_restart -= 1;
                optr[i] = 1;
            }
        }
        d_pulse_phase += d_pulse_frequency;
    }
    return noutput_items;
}

} /* namespace analog */
} /* namespace gr */

/* -*- c++ -*- */
/*
 * Copyright 2004,2006,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "squelch_base_ff_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/math.h>
#include <stdexcept>

namespace gr {
namespace analog {

squelch_base_ff_impl::squelch_base_ff_impl(const char* name, int ramp, bool gate)
    : block(name,
            io_signature::make(1, 1, sizeof(float)),
            io_signature::make(1, 1, sizeof(float))),
      d_sob_key(pmt::intern("squelch_sob")),
      d_eob_key(pmt::intern("squelch_eob")),
      d_tag_next_unmuted(true)
{
    if (ramp < 0) {
        throw std::invalid_argument("Ramp value must be non-negative");
    }

    set_ramp(ramp);
    set_gate(gate);
    d_state = ST_MUTED;
    d_envelope = d_ramp ? 0.0f : 1.0f;
    d_ramped = 0;
}

squelch_base_ff_impl::~squelch_base_ff_impl() {}

void squelch_base_ff_impl::precalculate_window()
{
    if (d_ramp <= 0) {
        d_window_table.clear();
        return;
    }

    const float increment = static_cast<float>(GR_M_PI) / d_ramp;
    d_window_table.resize(d_ramp + 1);

    for (int i = 0; i <= d_ramp; i++) {
        d_window_table[i] = 0.5f - std::cos(i * increment) / 2.0f;
    }
}

int squelch_base_ff_impl::ramp() const { return d_ramp; }

void squelch_base_ff_impl::set_ramp(int ramp)
{
    if (ramp < 0) {
        throw std::invalid_argument("Ramp value must be non-negative");
    }

    d_ramp = ramp;
    if (d_ramp == 0) {
        d_envelope = 1.0f;
    }
    precalculate_window();
}

bool squelch_base_ff_impl::gate() const { return d_gate; }

void squelch_base_ff_impl::set_gate(bool gate) { d_gate = gate; }

bool squelch_base_ff_impl::unmuted() const
{
    return (d_state == ST_UNMUTED || d_state == ST_ATTACK);
}

int squelch_base_ff_impl::general_work(int noutput_items,
                                       gr_vector_int& ninput_items,
                                       gr_vector_const_void_star& input_items,
                                       gr_vector_void_star& output_items)
{
    const float* in = static_cast<const float*>(input_items[0]);
    float* out = static_cast<float*>(output_items[0]);
    int j = 0;

    for (int i = 0; i < noutput_items; i++) {
        update_state(in[i]);

        // Adjust envelope based on current state
        switch (d_state) {
        case ST_MUTED:
            if (!mute()) {
                // If not ramping, go straight to unmuted
                d_state = d_ramp ? ST_ATTACK : ST_UNMUTED;
                if (d_state == ST_UNMUTED)
                    d_tag_next_unmuted = true;
            }
            break;

        case ST_UNMUTED:
            if (d_tag_next_unmuted) {
                d_tag_next_unmuted = false;
                add_item_tag(0, nitems_written(0) + j, d_sob_key, pmt::PMT_NIL);
            }
            if (mute()) {
                // If not ramping, go straight to muted
                d_state = d_ramp ? ST_DECAY : ST_MUTED;
                if (d_state == ST_MUTED)
                    add_item_tag(0, nitems_written(0) + j, d_eob_key, pmt::PMT_NIL);
            }
            break;

        case ST_ATTACK:
            if (!d_window_table.empty()) {
                d_envelope = d_window_table[++d_ramped];
            }
            if (d_ramped >= d_ramp) {
                d_state = ST_UNMUTED;
                d_tag_next_unmuted = true;
                d_envelope = 1.0f;
            }
            break;

        case ST_DECAY:
            if (!d_window_table.empty()) {
                d_envelope = d_window_table[--d_ramped];
            }
            if (d_ramped == 0) {
                d_state = ST_MUTED;
                add_item_tag(0, nitems_written(0) + j, d_eob_key, pmt::PMT_NIL);
            }
            break;
        }

        // If unmuted, copy input times envelope to output
        // Otherwise, if not gating, copy zero to output
        if (d_state != ST_MUTED)
            out[j++] = in[i] * d_envelope;
        else if (!d_gate)
            out[j++] = 0.0f;
    }

    consume_each(noutput_items); // Use all the inputs
    return j;                    // But only report outputs copied
}

} /* namespace analog */
} /* namespace gr */
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

#include "squelch_base_cc_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/math.h>

namespace gr {
namespace analog {

squelch_base_cc_impl::squelch_base_cc_impl(const char* name, int ramp, bool gate)
    : block(name,
            io_signature::make(1, 1, sizeof(float)),
            io_signature::make(1, 1, sizeof(float))),
      d_sob_key(pmt::intern("squelch_sob")),
      d_eob_key(pmt::intern("squelch_eob")),
      d_tag_next_unmuted(true)
{
    set_ramp(ramp);
    set_gate(gate);
    d_state = ST_MUTED;
    d_envelope = d_ramp ? 0.0 : 1.0;
    d_ramped = 0;
}

squelch_base_cc_impl::~squelch_base_cc_impl() {}

int squelch_base_cc_impl::ramp() const { return d_ramp; }

void squelch_base_cc_impl::set_ramp(int ramp)
{
    gr::thread::scoped_lock l(d_setlock);
    d_ramp = ramp;
}

bool squelch_base_cc_impl::gate() const { return d_gate; }

void squelch_base_cc_impl::set_gate(bool gate)
{
    gr::thread::scoped_lock l(d_setlock);
    d_gate = gate;
}

bool squelch_base_cc_impl::unmuted() const
{
    return (d_state == ST_UNMUTED || d_state == ST_ATTACK);
}

int squelch_base_cc_impl::general_work(int noutput_items,
                                       gr_vector_int& ninput_items,
                                       gr_vector_const_void_star& input_items,
                                       gr_vector_void_star& output_items)
{
    const gr_complex* in = (const gr_complex*)input_items[0];
    gr_complex* out = (gr_complex*)output_items[0];

    int j = 0;

    gr::thread::scoped_lock l(d_setlock);

    for (int i = 0; i < noutput_items; i++) {
        update_state(in[i]);

        // Adjust envelope based on current state
        switch (d_state) {
        case ST_MUTED:
            if (!mute()) {
                d_state = d_ramp ? ST_ATTACK
                                 : ST_UNMUTED; // If not ramping, go straight to unmuted
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
                d_state =
                    d_ramp ? ST_DECAY : ST_MUTED; // If not ramping, go straight to muted
                if (d_state == ST_MUTED)
                    add_item_tag(0, nitems_written(0) + j, d_eob_key, pmt::PMT_NIL);
            }
            break;

        case ST_ATTACK:
            d_envelope = 0.5 - std::cos(GR_M_PI * (++d_ramped) / d_ramp) /
                                   2.0; // FIXME: precalculate window for speed
            if (d_ramped >=
                d_ramp) { // use >= in case d_ramp is set to lower value elsewhere
                d_state = ST_UNMUTED;
                d_tag_next_unmuted = true;
                d_envelope = 1.0;
            }
            break;

        case ST_DECAY:
            d_envelope = 0.5 - std::cos(GR_M_PI * (--d_ramped) / d_ramp) /
                                   2.0; // FIXME: precalculate window for speed
            if (d_ramped == 0.0) {
                d_state = ST_MUTED;
                add_item_tag(0, nitems_written(0) + j, d_eob_key, pmt::PMT_NIL);
            }
            break;
        };

        // If unmuted, copy input times envelope to output
        // Otherwise, if not gating, copy zero to output
        if (d_state != ST_MUTED) {
            out[j++] = in[i] * gr_complex(d_envelope, 0.0);
        } else {
            if (!d_gate) {
                out[j++] = 0.0;
            }
        }
    }

    consume_each(noutput_items); // Use all the inputs
    return j;                    // But only report outputs copied
}

} /* namespace analog */
} /* namespace gr */

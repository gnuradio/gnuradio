/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "phase_shift_impl.h"
#include <gnuradio/io_signature.h>

#include <gnuradio/math.h>
#include <volk/volk.h>

namespace gr {
namespace blocks {

phase_shift::sptr phase_shift::make(float shift, bool is_radians)
{
    return gnuradio::make_block_sptr<phase_shift_impl>(shift, is_radians);
}

/*
 * The private constructor
 */
phase_shift_impl::phase_shift_impl(float shift, bool is_radians)
    : gr::sync_block("phase_shift",
                     gr::io_signature::make(1, 1, sizeof(gr_complex)),
                     gr::io_signature::make(1, 1, sizeof(gr_complex))),
      d_is_radians(is_radians)
{
    set_shift(shift);

    message_port_register_in(pmt::mp("shift"));
    set_msg_handler(pmt::mp("shift"),
                    [this](pmt::pmt_t msg) { this->handle_msg_in(msg); });
}

/*
 * Our virtual destructor.
 */
phase_shift_impl::~phase_shift_impl() {}

void phase_shift_impl::handle_msg_in(pmt::pmt_t msg)
{
    if (pmt::is_number(msg)) {
        set_shift(pmt::to_float(msg));
    } else {
        if (pmt::is_pair(msg)) {
            pmt::pmt_t data = pmt::cdr(msg);
            if (pmt::is_number(data)) {
                set_shift(pmt::to_float(data));
            } else
                d_logger->warn(
                    "Phase message must be a number or a number pair.  Ignoring value.");
        }
    }
}

void phase_shift_impl::set_shift(float new_value)
{
    gr::thread::scoped_lock guard(d_setlock);
    if (d_is_radians)
        d_shift = new_value;
    else
        d_shift = new_value * GR_M_PI / 180.0; // convert to radians

    d_shift_cc = gr_complex(cos(d_shift), sin(d_shift));
}

int phase_shift_impl::work(int noutput_items,
                           gr_vector_const_void_star& input_items,
                           gr_vector_void_star& output_items)
{
    const gr_complex* in = (const gr_complex*)input_items[0];
    gr_complex* out = (gr_complex*)output_items[0];

    gr::thread::scoped_lock guard(d_setlock);

    if (d_shift != 0.0f) {
#if VOLK_VERSION >= 030100
        volk_32fc_s32fc_multiply2_32fc(out, in, &d_shift_cc, noutput_items);
#else
        volk_32fc_s32fc_multiply_32fc(out, in, d_shift_cc, noutput_items);
#endif
    } else {
        memcpy(out, in, sizeof(gr_complex) * noutput_items);
    }

    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */

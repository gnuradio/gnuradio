/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "rotator_cc_impl.h"
#include <gnuradio/io_signature.h>

#include <cmath>

namespace gr {
namespace blocks {

rotator_cc::sptr rotator_cc::make(double phase_inc, bool tag_inc_updates)
{
    return gnuradio::make_block_sptr<rotator_cc_impl>(phase_inc, tag_inc_updates);
}

rotator_cc_impl::rotator_cc_impl(double phase_inc, bool tag_inc_updates)
    : sync_block("rotator_cc",
                 io_signature::make(1, 1, sizeof(gr_complex)),
                 io_signature::make(1, 1, sizeof(gr_complex))),
      d_tag_inc_updates(tag_inc_updates),
      d_inc_update_queue(cmp_phase_inc_update_offset)
{
    set_phase_inc(phase_inc);

    const pmt::pmt_t port_id = pmt::mp("cmd");
    message_port_register_in(port_id);
    set_msg_handler(port_id, [this](pmt::pmt_t msg) { this->handle_cmd_msg(msg); });
}

rotator_cc_impl::~rotator_cc_impl() {}

void rotator_cc_impl::set_phase_inc(double phase_inc)
{
    d_r.set_phase_incr(exp(gr_complex(0, phase_inc)));
}

void rotator_cc_impl::handle_cmd_msg(pmt::pmt_t msg)
{
    gr::thread::scoped_lock l(d_mutex);

    static const pmt::pmt_t inc_key = pmt::intern("inc");
    static const pmt::pmt_t offset_key = pmt::intern("offset");

    if (!pmt::is_dict(msg)) {
        throw std::runtime_error("rotator_cc: Rotator command message "
                                 "must be a PMT dictionary");
    }

    bool handled = false;

    if (pmt::dict_has_key(msg, inc_key)) {
        /* Prepare to update the phase increment on a specific absolute sample
         * offset. If the offset is not defined in the command message,
         * configure the phase increment update to occur immediately. */
        phase_inc_update_t update{};
        update.phase_inc = pmt::to_double(pmt::dict_ref(msg, inc_key, pmt::PMT_NIL));
        update.offset = pmt::dict_has_key(msg, offset_key)
                            ? pmt::to_uint64(pmt::dict_ref(msg, offset_key, pmt::PMT_NIL))
                            : nitems_written(0);
        d_inc_update_queue.push(update);
        handled = true;
    }

    if (!handled) {
        throw std::runtime_error("rotator_cc: Unsupported command message");
    }
}

int rotator_cc_impl::work(int noutput_items,
                          gr_vector_const_void_star& input_items,
                          gr_vector_void_star& output_items)
{
    gr::thread::scoped_lock l(d_mutex);
    const gr_complex* in = (const gr_complex*)input_items[0];
    gr_complex* out = (gr_complex*)output_items[0];
    const uint64_t n_written = nitems_written(0);
    static const pmt::pmt_t phase_inc_tag_key = pmt::intern("rot_phase_inc");

    if (d_inc_update_queue.empty()) {
        d_r.rotateN(out, in, noutput_items);
    } else {
        /* If there are phase increment updates scheduled for now, handle the
         * rotation in steps and update the phase increment in between. */
        int nprocessed_items = 0;
        while (!d_inc_update_queue.empty()) {
            auto next_update = d_inc_update_queue.top();

            if (next_update.offset < (n_written + nprocessed_items)) {
                d_inc_update_queue.pop();
                continue; // we didn't process this update on time - drop it
            }

            if (next_update.offset >= (n_written + noutput_items)) {
                break; // the update is for a future batch of samples
            }

            // The update is scheduled for this batch of samples. Apply it now.
            d_inc_update_queue.pop();

            // Process all samples until the scheduled phase increment update
            int items_before_update = next_update.offset - n_written - nprocessed_items;
            d_r.rotateN(
                out + nprocessed_items, in + nprocessed_items, items_before_update);
            nprocessed_items += items_before_update;

            set_phase_inc(next_update.phase_inc);

            if (d_tag_inc_updates) {
                add_item_tag(0,
                             next_update.offset,
                             phase_inc_tag_key,
                             pmt::from_float(next_update.phase_inc));
            }
        }

        // Rotate the remaining samples
        d_r.rotateN(out + nprocessed_items,
                    in + nprocessed_items,
                    (noutput_items - nprocessed_items));
    }

    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */

/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 * Copyright 2023 Marcus Müller
 * Copyright 2023 Daniel Estevez <daniel@destevez.net>
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "repeat_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/pmt_fmt.h>
#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <vector>

namespace gr {
namespace blocks {

repeat::sptr repeat::make(size_t itemsize, int interp)
{
    return gnuradio::make_block_sptr<repeat_impl>(itemsize, interp);
}

repeat_impl::repeat_impl(size_t itemsize, int interp)
    : block("repeat",
            io_signature::make(1, 1, itemsize),
            io_signature::make(1, 1, itemsize)),
      d_itemsize(itemsize),
      d_interp(interp),
      d_left_to_copy(0),
      c_msg_port(pmt::mp("interpolation"))
{
    if (interp < 1) {
        throw std::invalid_argument(fmt::format(
            FMT_STRING(
                "Trying to set interpolation to {}, but must be a positive integer."),
            interp));
    }
    // tags are propagated manually using the copy_tags() helper function
    set_tag_propagation_policy(TPP_DONT);
    message_port_register_in(c_msg_port);
    set_msg_handler(c_msg_port,
                    [this](const pmt::pmt_t& msg) { this->msg_set_interpolation(msg); });
}

void repeat_impl::msg_set_interpolation(const pmt::pmt_t& msg)
{
    if (!pmt::eq(c_msg_port, pmt::car(msg))) {
        d_logger->error("Message with unknown key {}", pmt::car(msg));
    }
    auto new_interp = pmt::to_long(pmt::cdr(msg));
    if (new_interp < 0) {
        d_logger->error(
            "Trying to set interpolation to {}, but must be a positive integer.",
            new_interp);
        return;
    }
    d_logger->trace("Setting new interpolation {}, replacing {}. Items left at old "
                    "interpolation: {}.",
                    new_interp,
                    d_interp,
                    d_left_to_copy);
    d_interp = new_interp;
}
void repeat_impl::set_interpolation(int interp)
{
    // This ensures that interpolation is only changed between calls to work
    // (and not in the middle of an ongoing work)
    if (interp < 1) {
        d_logger->error(
            "Trying to set interpolation to {}, but must be a positive integer.", interp);
        throw std::invalid_argument(fmt::format(
            FMT_STRING(
                "Trying to set interpolation to {}, but must be a positive integer."),
            interp));
    }

    _post(c_msg_port,                                   /* port */
          pmt::cons(c_msg_port, pmt::from_long(interp)) /* pair */
    );
}

void repeat_impl::forecast(int noutput_items, gr_vector_int& required)
{
    if (static_cast<size_t>(noutput_items) <= d_left_to_copy) {
        // can fill request from memory
        required[0] = 0;
        return;
    }


    // making sure we get enough input
    size_t requested_round_up = (static_cast<size_t>(noutput_items) -
                                 d_left_to_copy) // amount we haven't "prepared"
                                + d_interp - 1;  // round up to multiple of d_interp

    // only one possible input
    required[0] = requested_round_up / d_interp;
}

//! \brief helper function for the copying
inline void
copy_and_advance_output(char*& out, const char* in, size_t itemsize, size_t times)
{
    for (size_t out_counter = times; out_counter; --out_counter) {
        std::memcpy(out, in, itemsize);
        out += itemsize;
    }
}

int repeat_impl::general_work(int noutput_items,
                              gr_vector_int& ninput_items,
                              gr_vector_const_void_star& input_items,
                              gr_vector_void_star& output_items)
{
    /*
     * And this is how we do it:
     * 1. We output the reminder of the last iteration's input item's repetitions (if
     * any!)
     * 2. We output the repetitions of the items that fit a full d_interp times
     * 3. We fill the remainder of output_items with a partial set of repetitions, save
     * the number of remaining repetitions
     */
    const char* in = (const char*)input_items[0];
    char* out = (char*)output_items[0];

    auto nout = static_cast<size_t>(noutput_items);
    size_t consumed = 0;

    // copy what is left to copy; here we do not need to copy any tags, since
    // the tags were copied in a previous work() call
    if (d_left_to_copy) {
        auto copy_from_buffer = std::min(d_left_to_copy, nout);
        copy_and_advance_output(out, in, d_itemsize, copy_from_buffer);
        d_left_to_copy -= copy_from_buffer;
        nout -= copy_from_buffer;
        if (!d_left_to_copy) {
            in += d_itemsize;
            ++consumed; // we're now done with this item
        } else {
            // we haven't even copied one full set of repetitons
            return copy_from_buffer;
        }
    }

    // copy the full multiples of d_interp from in to out
    size_t full_items = std::min<size_t>(nout / d_interp, ninput_items[0] - consumed);

    // copy tags for these items: each tag is copied to the first item of the
    // d_interp repetitions
    copy_tags(consumed, noutput_items - nout, full_items);

    for (auto in_counter = full_items; in_counter; --in_counter) {
        copy_and_advance_output(out, in, d_itemsize, d_interp);
        in += d_itemsize;
    }
    consumed += full_items;
    nout -= full_items * d_interp;

    if (nout && static_cast<size_t>(ninput_items[0]) > consumed) {
        // nout now only contains at most d_interp - 1
        copy_tags(consumed, noutput_items - nout, 1); // copy tags for only one item
        copy_and_advance_output(out, in, d_itemsize, nout);
        d_left_to_copy = d_interp - nout;
        nout -= nout;
    }
    consume_each(consumed);
    return noutput_items - nout;
}

} /* namespace blocks */
} /* namespace gr */

/* -*- c++ -*- */
/*
 * Copyright 2007,2010,2012-2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "delay_impl.h"
#include <gnuradio/io_signature.h>
#include <cstring>

namespace gr {
namespace blocks {

delay::sptr delay::make(size_t itemsize, int delay)
{
    return gnuradio::make_block_sptr<delay_impl>(itemsize, delay);
}

delay_impl::delay_impl(size_t itemsize, int delay)
    : block("delay",
            io_signature::make(1, -1, itemsize),
            io_signature::make(1, -1, itemsize)),
      d_itemsize(itemsize)
{
    if (delay < 0) {
        throw std::runtime_error("delay: Cannot initialize block with a delay < 0.");
    }
    set_dly(delay);
    d_delta = 0;

    message_port_register_in(pmt::mp("dly"));
    set_msg_handler(pmt::mp("dly"), [this](pmt::pmt_t msg) { this->handle_msg(msg); });
}

delay_impl::~delay_impl() {}

void delay_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{
    // make sure all inputs have noutput_items available
    unsigned ninputs = ninput_items_required.size();
    for (unsigned i = 0; i < ninputs; i++)
        ninput_items_required[i] = noutput_items;
}

void delay_impl::set_dly(int d)
{
    // only set a new delta if there is a change in the delay; this
    // protects from quickly-repeated calls to this function that
    // would end with d_delta=0.
    if (d != dly()) {
        gr::thread::scoped_lock l(d_setlock);
        int old = dly();
        set_history(d + 1);
        declare_sample_delay(history() - 1);
        d_delta += dly() - old;
    }
}

void delay_impl::handle_msg(pmt::pmt_t msg)
{
    if (pmt::is_number(msg)) {
        int value = pmt::to_long(msg);
        set_dly(value);
    } else {
        if (pmt::is_pair(msg)) {
            pmt::pmt_t data = pmt::cdr(msg);
            if (pmt::is_number(data)) {
                int value = pmt::to_long(data);
                set_dly(value);
            } else
                GR_LOG_WARN(
                    d_logger,
                    "Delay message must be a number or a number pair.  Ignoring value.");
        }
    }
}

int delay_impl::general_work(int noutput_items,
                             gr_vector_int& ninput_items,
                             gr_vector_const_void_star& input_items,
                             gr_vector_void_star& output_items)
{
    gr::thread::scoped_lock l(d_setlock);
    assert(input_items.size() == output_items.size());

    const char* iptr;
    char* optr;
    int cons, ret;

    // No change in delay; just memcpy ins to outs
    if (d_delta == 0) {
        for (size_t i = 0; i < input_items.size(); i++) {
            iptr = (const char*)input_items[i];
            optr = (char*)output_items[i];
            std::memcpy(optr, iptr, noutput_items * d_itemsize);
        }
        cons = noutput_items;
        ret = noutput_items;
    }

    // Skip over d_delta items on the input
    else if (d_delta < 0) {
        int n_to_copy, n_adj;
        int delta = -d_delta;
        n_to_copy = std::max(0, noutput_items - delta);
        n_adj = std::min(delta, noutput_items);
        for (size_t i = 0; i < input_items.size(); i++) {
            iptr = (const char*)input_items[i];
            optr = (char*)output_items[i];
            std::memcpy(optr, iptr + delta * d_itemsize, n_to_copy * d_itemsize);
        }
        cons = noutput_items;
        ret = n_to_copy;
        delta -= n_adj;
        d_delta = -delta;
    }

    // produce but not consume (inserts zeros)
    else { // d_delta > 0
        int n_from_input, n_padding;
        n_from_input = std::max(0, noutput_items - d_delta);
        n_padding = std::min(d_delta, noutput_items);
        for (size_t i = 0; i < input_items.size(); i++) {
            iptr = (const char*)input_items[i];
            optr = (char*)output_items[i];
            std::memset(optr, 0, n_padding * d_itemsize);
            std::memcpy(optr, iptr, n_from_input * d_itemsize);
        }
        cons = n_from_input;
        ret = noutput_items;
        d_delta -= n_padding;
    }

    consume_each(cons);
    return ret;
}

} /* namespace blocks */
} /* namespace gr */

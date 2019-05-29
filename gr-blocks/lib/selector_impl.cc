/* -*- c++ -*- */
/*
 * Copyright 2019 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "selector_impl.h"
#include <gnuradio/io_signature.h>
#include <string.h>
#include <stdexcept>

namespace gr {
namespace blocks {

selector::sptr
selector::make(size_t itemsize, unsigned int input_index, unsigned int output_index)
{
    return gnuradio::get_initial_sptr(
        new selector_impl(itemsize, input_index, output_index));
}

selector_impl::selector_impl(size_t itemsize,
                             unsigned int input_index,
                             unsigned int output_index)
    : block("selector",
            io_signature::make(1, -1, itemsize),
            io_signature::make(1, -1, itemsize)),
      d_itemsize(itemsize),
      d_enabled(true),
      d_input_index(input_index),
      d_output_index(output_index),
      d_num_inputs(0),
      d_num_outputs(0)
{
    message_port_register_in(pmt::mp("en"));
    set_msg_handler(pmt::mp("en"), [this](pmt::pmt_t msg) { this->handle_enable(msg); });

    // TODO: add message ports for input_index and output_index
}

selector_impl::~selector_impl() {}

void selector_impl::set_input_index(unsigned int input_index)
{
    gr::thread::scoped_lock l(d_mutex);
    if (input_index < d_num_inputs)
        d_input_index = input_index;
    else
        throw std::out_of_range("input_index must be < ninputs");
}

void selector_impl::set_output_index(unsigned int output_index)
{
    gr::thread::scoped_lock l(d_mutex);
    if (output_index < d_num_outputs)
        d_output_index = output_index;
    else
        throw std::out_of_range("output_index must be < noutputs");
}

void selector_impl::handle_enable(pmt::pmt_t msg)
{
    if (pmt::is_bool(msg)) {
        bool en = pmt::to_bool(msg);
        gr::thread::scoped_lock l(d_mutex);
        d_enabled = en;
    } else {
        GR_LOG_WARN(d_logger,
                    "handle_enable: Non-PMT type received, expecting Boolean PMT");
    }
}

void selector_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{
    unsigned ninputs = ninput_items_required.size();
    for (unsigned i = 0; i < ninputs; i++) {
        ninput_items_required[i] = noutput_items;
    }
}

bool selector_impl::check_topology(int ninputs, int noutputs)
{
    if ((int)d_input_index < ninputs && (int)d_output_index < noutputs) {
        d_num_inputs = (unsigned int)ninputs;
        d_num_outputs = (unsigned int)noutputs;
        return true;
    } else {
        GR_LOG_WARN(d_logger,
                    "check_topology: Input or Output index greater than number of ports");
        return false;
    }
}

int selector_impl::general_work(int noutput_items,
                                gr_vector_int& ninput_items,
                                gr_vector_const_void_star& input_items,
                                gr_vector_void_star& output_items)
{
    const uint8_t** in = (const uint8_t**)&input_items[0];
    uint8_t** out = (uint8_t**)&output_items[0];

    gr::thread::scoped_lock l(d_mutex);
    if (d_enabled) {
        std::copy(in[d_input_index],
                  in[d_input_index] + noutput_items * d_itemsize,
                  out[d_output_index]);
        produce(d_output_index, noutput_items);
    }

    consume_each(noutput_items);
    return WORK_CALLED_PRODUCE;
}

void selector_impl::setup_rpc()
{
#ifdef GR_CTRLPORT
    add_rpc_variable(rpcbasic_sptr(new rpcbasic_register_handler<selector>(
        alias(), "en", "", "Enable", RPC_PRIVLVL_MIN, DISPNULL)));
#endif /* GR_CTRLPORT */
}

} /* namespace blocks */
} /* namespace gr */

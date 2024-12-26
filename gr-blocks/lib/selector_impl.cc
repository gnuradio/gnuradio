/* -*- c++ -*- */
/*
 * Copyright 2019 Free Software Foundation, Inc.
 * Copyright 2024 Skandalis Georgios
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "pmt/pmt.h"
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "selector_impl.h"
#include <gnuradio/io_signature.h>
#include <cstring>
#include <stdexcept>

namespace gr {
namespace blocks {

selector::sptr
selector::make(size_t itemsize, unsigned int input_index, unsigned int output_index)
{
    return gnuradio::make_block_sptr<selector_impl>(itemsize, input_index, output_index);
}

selector_impl::selector_impl(size_t itemsize,
                             unsigned int input_index,
                             unsigned int output_index)
    : block("selector",
            io_signature::make(1, -1, itemsize),
            io_signature::make(1, -1, itemsize)),
      d_itemsize(itemsize),
      d_input_index(input_index),
      d_output_index(output_index),
      d_num_inputs(0),
      d_num_outputs(0),
      d_enabled(true)
{
    message_port_register_in(pmt::mp("en"));
    set_msg_handler(pmt::mp("en"),
                    [this](const pmt::pmt_t& msg) { this->handle_enable(msg); });

    message_port_register_in(pmt::mp("iindex"));
    set_msg_handler(pmt::mp("iindex"),
                    [this](const pmt::pmt_t& msg) { this->handle_msg_input_index(msg); });

    message_port_register_in(pmt::mp("oindex"));
    set_msg_handler(pmt::mp("oindex"), [this](const pmt::pmt_t& msg) {
        this->handle_msg_output_index(msg);
    });

    set_tag_propagation_policy(TPP_CUSTOM);
}

selector_impl::~selector_impl() {}

void selector_impl::set_input_index(unsigned int input_index)
{
    gr::thread::scoped_lock l(d_mutex);
    if (input_index < d_num_inputs) {
        d_input_index = input_index;
    } else {
        throw std::out_of_range("input_index must be < ninputs");
    }
}

void selector_impl::set_output_index(unsigned int output_index)
{
    gr::thread::scoped_lock l(d_mutex);
    if (output_index < d_num_outputs) {
        d_output_index = output_index;
    } else {
        throw std::out_of_range("output_index must be < noutputs");
    }
}

void selector_impl::handle_msg_input_index(const pmt::pmt_t& msg)
{
    pmt::pmt_t data = pmt::cdr(msg);

    if (pmt::is_integer(data)) {
        const unsigned int new_port = pmt::to_long(data);

        if (new_port < d_num_inputs) {
            set_input_index(new_port);
        } else {
            d_logger->warn("handle_msg_input_index: port index {:d} greater than "
                           "available ports {:d}.",
                           new_port,
                           d_num_inputs);
        }
    } else {
        d_logger->warn(
            "handle_msg_input_index: Non-PMT type received, expecting integer PMT");
    }
}

void selector_impl::handle_msg_output_index(const pmt::pmt_t& msg)
{
    pmt::pmt_t data = pmt::cdr(msg);

    if (pmt::is_integer(data)) {
        const unsigned int new_port = pmt::to_long(data);
        if (new_port < d_num_outputs) {
            set_output_index(new_port);
        } else {
            d_logger->warn("handle_msg_input_index: port index {:d} greater than "
                           "available ports {:d}.",
                           new_port,
                           d_num_inputs);
        }
    } else {
        d_logger->warn(
            "handle_msg_output_index: Non-PMT type received, expecting integer PMT");
    }
}


void selector_impl::handle_enable(const pmt::pmt_t& msg)
{
    if (pmt::is_bool(msg)) {
        const bool en = pmt::to_bool(msg);
        gr::thread::scoped_lock l(d_mutex);
        d_enabled = en;
    } else if (pmt::is_pair(msg)) {
        const bool en = pmt::to_bool(pmt::cdr(msg));
        gr::thread::scoped_lock l(d_mutex);
        d_enabled = en;
    } else {
        d_logger->warn(
            "handle_enable: Invalid message type received, expected Boolean PMT or PDU");
    }
}

void selector_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{
    unsigned ninputs = ninput_items_required.size();
    for (unsigned i = 0; i < ninputs; i++) {
        ninput_items_required[i] = 0;
    }
    ninput_items_required[d_input_index] = noutput_items;
}

bool selector_impl::check_topology(int ninputs, int noutputs)
{
    if ((int)d_input_index < ninputs && (int)d_output_index < noutputs) {
        d_num_inputs = (unsigned int)ninputs;
        d_num_outputs = (unsigned int)noutputs;
        return true;
    }
    d_logger->warn("check_topology: Input or Output index greater than number of ports");
    return false;
}

int selector_impl::general_work(int noutput_items,
                                gr_vector_int& ninput_items,
                                gr_vector_const_void_star& input_items,
                                gr_vector_void_star& output_items)
{
    const auto in = reinterpret_cast<const uint8_t**>(input_items.data());
    auto out = reinterpret_cast<uint8_t**>(output_items.data());

    unsigned int to_copy = std::min(ninput_items[d_input_index], noutput_items);

    gr::thread::scoped_lock l(d_mutex);
    if (d_enabled) {
        auto nread = nitems_read(d_input_index);
        auto nwritten = nitems_written(d_output_index);

        std::vector<tag_t> tags;
        get_tags_in_window(tags, d_input_index, 0, to_copy);

        for (auto tag : tags) {
            tag.offset -= (nread - nwritten);
            add_item_tag(d_output_index, tag);
        }

        std::copy(in[d_input_index],
                  in[d_input_index] + to_copy * d_itemsize,
                  out[d_output_index]);
        produce(d_output_index, to_copy);
    }

    for (unsigned int in_index = 0; in_index < ninput_items.size(); ++in_index) {
        consume(in_index,
                std::min(static_cast<unsigned int>(ninput_items[in_index]), to_copy));
    }
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

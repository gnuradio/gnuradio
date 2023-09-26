/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 * Copyright 2020 Ettus Research, A National Instruments Brand.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/pmt_fmt.h>
#include <gnuradio/uhd/cmd_keys.h>
#include <gnuradio/uhd/rfnoc_block.h>

namespace gr {
namespace uhd {


/******************************************************************************
 * Factory and Structors
 *****************************************************************************/
::uhd::rfnoc::noc_block_base::sptr
rfnoc_block::make_block_ref(rfnoc_graph::sptr graph,
                            const ::uhd::device_addr_t& block_args,
                            const std::string& block_name,
                            const int device_select,
                            const int block_select,
                            const size_t max_ref_count)
{
    const std::string block_id =
        graph->get_block_id(block_name, device_select, block_select);
    if (block_id.empty()) {
        throw std::runtime_error(
            fmt::format("Cannot find block with ID: {:s} Device/Instance: {:d}/{:d}",
                        block_name,
                        device_select,
                        block_select));
    }

    auto block = graph->get_block_ref(block_id, max_ref_count);
    if (block) {
        block->set_properties(block_args, 0);
    }

    return block;
}

rfnoc_block::rfnoc_block(::uhd::rfnoc::noc_block_base::sptr block_ref)
    : gr::block(
          std::string("RFNoC::") + block_ref->get_unique_id(),
          gr::io_signature::make(0, 0, 0), // All RFNoC blocks don't stream into GNU Radio
          gr::io_signature::make(0, 0, 0)),
      d_block_ref(block_ref)
{
    // Set up message ports:
    message_port_register_in(pmt::mp("command"));
    set_msg_handler(pmt::mp("command"),
                    [this](pmt::pmt_t msg) { this->_msg_handler_command(msg); });
}


/******************************************************************************
 * Command Message Interface
 *****************************************************************************/
void rfnoc_block::_msg_handler_command(pmt::pmt_t msg)
{
    // pmt_dict is a subclass of pmt_pair. Make sure we use pmt_pair!
    if (!pmt::is_dict(msg) && pmt::is_pair(msg)) {
        d_logger->debug(
            "Command message is pair, converting to dict: '{}': car({}), cdr({})",
            msg,
            pmt::car(msg),
            pmt::cdr(msg));
        msg = pmt::dict_add(pmt::make_dict(), pmt::car(msg), pmt::cdr(msg));
    }

    // Make sure, we use dicts!
    if (!pmt::is_dict(msg)) {
        d_logger->error("Command message is neither dict nor pair: {}",
                        pmt::write_string(msg));
        return;
    }

    /*** Start the actual message processing *************************/
    /// 1) Read chan value
    int chan = int(pmt::to_long(pmt::dict_ref(msg,
                                              cmd_chan_key(),
                                              pmt::from_long(-1) // Default to all chans
                                              )));

    /// 2) Loop through all the values
    d_debug_logger->debug("Processing command message {}", pmt::write_string(msg));
    pmt::pmt_t msg_items = pmt::dict_items(msg);
    for (size_t i = 0; i < pmt::length(msg_items); i++) {
        try {
            _dispatch_msg_cmd_handler(pmt::write_string(pmt::car(pmt::nth(i, msg_items))),
                                      pmt::cdr(pmt::nth(i, msg_items)),
                                      chan,
                                      msg);
        } catch (const pmt::wrong_type& e) {
            d_logger->alert("Invalid command value for key {}: {}",
                            pmt::car(pmt::nth(i, msg_items)),
                            pmt::cdr(pmt::nth(i, msg_items)));
            break;
        }
    }
}

void rfnoc_block::_dispatch_msg_cmd_handler(const std::string& cmd,
                                            const pmt::pmt_t& val,
                                            int chan,
                                            pmt::pmt_t& msg)
{
    if (_msg_cmd_handlers.count(cmd)) {
        _msg_cmd_handlers[cmd](val, chan, msg);
    } else if (cmd != "chan" || cmd != "time") {
        // We don't warn for keys that are usually additional arguments
        d_logger->warn("No command handler registered for command: `{}'", cmd);
    }
}

void rfnoc_block::register_msg_cmd_handler(const std::string& cmd, cmd_handler_t handler)
{
    _msg_cmd_handlers[cmd] = handler;
}

/******************************************************************************
 * GNU Radio API
 *****************************************************************************/
std::string rfnoc_block::get_unique_id() const { return d_block_ref->get_unique_id(); }

int rfnoc_block::general_work(int /*noutput_items*/,
                              gr_vector_int& /*ninput_items*/,
                              gr_vector_const_void_star& /*input_items*/,
                              gr_vector_void_star& /*output_items*/)
{
    // We should never land here
    throw std::runtime_error("Unexpected call to general_work() in an RFNoC block!");
    return 0;
}

std::vector<std::string> rfnoc_block::get_property_ids()
{
    return d_block_ref->get_property_ids();
}

} /* namespace uhd */
} /* namespace gr */

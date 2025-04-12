/* -*- c++ -*- */
/*
 * Copyright 2023 Ettus Research, a National Instruments Brand.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "rfnoc_replay_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace uhd {

const pmt::pmt_t replay_cmd_key()
{
    static const pmt::pmt_t val = pmt::mp("command");
    return val;
}
const pmt::pmt_t replay_cmd_port_key()
{
    static const pmt::pmt_t val = pmt::mp("port");
    return val;
}
const pmt::pmt_t replay_cmd_offset_key()
{
    static const pmt::pmt_t val = pmt::mp("offset");
    return val;
}
const pmt::pmt_t replay_cmd_size_key()
{
    static const pmt::pmt_t val = pmt::mp("size");
    return val;
}
const pmt::pmt_t replay_cmd_time_key()
{
    static const pmt::pmt_t val = pmt::mp("time");
    return val;
}
const pmt::pmt_t replay_cmd_repeat_key()
{
    static const pmt::pmt_t val = pmt::mp("repeat");
    return val;
}
const pmt::pmt_t replay_debug_port_key()
{
    static const pmt::pmt_t val = pmt::mp("debug_msgs");
    return val;
}
const pmt::pmt_t replay_mem_fullness_key()
{
    static const pmt::pmt_t val = pmt::mp("record_fullness");
    return val;
}
const pmt::pmt_t replay_mem_size_key()
{
    static const pmt::pmt_t val = pmt::mp("memory_size");
    return val;
}
const pmt::pmt_t replay_word_size_key()
{
    static const pmt::pmt_t val = pmt::mp("word_size");
    return val;
}

rfnoc_replay::sptr rfnoc_replay::make(rfnoc_graph::sptr graph,
                                      const ::uhd::device_addr_t& block_args,
                                      const int device_select,
                                      const int instance)
{
    return gnuradio::make_block_sptr<rfnoc_replay_impl>(rfnoc_block::make_block_ref(
        graph, block_args, "Replay", device_select, instance));
}

rfnoc_replay_impl::rfnoc_replay_impl(::uhd::rfnoc::noc_block_base::sptr block_ref)
    : rfnoc_block(block_ref),
      d_wrapped_ref(
          std::dynamic_pointer_cast<::uhd::rfnoc::replay_block_control>(block_ref))
{
    message_port_register_out(replay_debug_port_key());
    message_port_register_in(replay_cmd_key());
    set_msg_handler(replay_cmd_key(),
                    [this](const pmt::pmt_t& msg) { this->_command_handler(msg); });
}

rfnoc_replay_impl::~rfnoc_replay_impl() {}

/******************************************************************************
 * rfnoc_replay API
 *****************************************************************************/
void rfnoc_replay_impl::record(const uint64_t offset,
                               const uint64_t size,
                               const size_t port)
{
    d_wrapped_ref->record(offset, size, port);
}

void rfnoc_replay_impl::record_restart(const size_t port)
{
    d_wrapped_ref->record_restart(port);
}

void rfnoc_replay_impl::play(const uint64_t offset,
                             const uint64_t size,
                             const size_t port,
                             const ::uhd::time_spec_t time_spec,
                             const bool repeat)
{
    d_wrapped_ref->play(offset, size, port, time_spec, repeat);
}

void rfnoc_replay_impl::stop_playback(const size_t port) { d_wrapped_ref->stop(port); }

void rfnoc_replay_impl::set_record_type(const std::string type, const size_t port)
{
    d_wrapped_ref->set_record_type(type, port);
}

void rfnoc_replay_impl::set_play_type(const std::string type, const size_t port)
{
    d_wrapped_ref->set_play_type(type, port);
}

void rfnoc_replay_impl::issue_stream_cmd(const ::uhd::stream_cmd_t& cmd,
                                         const size_t port)
{
    d_wrapped_ref->issue_stream_cmd(cmd, port);
}

void rfnoc_replay_impl::_command_handler(pmt::pmt_t msg)
{
    if (!(pmt::is_dict(msg)) && pmt::is_pair(msg)) {
        d_logger->debug(
            "Command message is pair, converting to dict: '{}': car({}), cdr({})",
            pmt::write_string(msg),
            pmt::write_string(pmt::car(msg)),
            pmt::write_string(pmt::cdr(msg)));
        msg = pmt::dict_add(pmt::make_dict(), pmt::car(msg), pmt::cdr(msg));
    }

    // Make sure, we use dicts!
    if (!pmt::is_dict(msg)) {
        d_logger->error("Command message is neither dict nor pair: {}",
                        pmt::write_string(msg));
        return;
    }

    if (!pmt::dict_has_key(msg, replay_cmd_key())) {
        d_logger->error("Command message does not have a command: {}",
                        pmt::write_string(msg));
        return;
    }

    bool has_cmd_offset = false;
    bool has_cmd_size = false;
    uint64_t cmd_offset, cmd_size;

    // Check to see if the command sent specifies a size and/or offset for the command
    // If it does not that is fine, we will use the record/play size/offset properties.
    if (pmt::dict_has_key(msg, replay_cmd_offset_key())) {
        cmd_offset = int(
            pmt::to_long(pmt::dict_ref(msg, replay_cmd_offset_key(), pmt::from_long(0))));
        has_cmd_offset = true;
    }
    if (pmt::dict_has_key(msg, replay_cmd_size_key())) {
        cmd_size = int(
            pmt::to_long(pmt::dict_ref(msg, replay_cmd_size_key(), pmt::from_long(0))));
        has_cmd_size = true;
    }

    int cmd_port = int(pmt::to_long(pmt::dict_ref(msg,
                                                  replay_cmd_port_key(),
                                                  pmt::from_long(0) // Default to Port 0
                                                  )));

    double cmd_time =
        int(pmt::to_float(pmt::dict_ref(msg,
                                        replay_cmd_time_key(),
                                        pmt::from_float(0.0) // Default to 0.0
                                        )));

    std::string command =
        pmt::symbol_to_string(pmt::dict_ref(msg, replay_cmd_key(), pmt::intern("")));

    if (command == "record") {
        // If command didn't specify size/offset get from the record properties
        if (!has_cmd_offset)
            cmd_offset = d_wrapped_ref->get_record_offset(cmd_port);
        if (!has_cmd_size)
            cmd_size = d_wrapped_ref->get_record_size(cmd_port);
        record(cmd_offset, cmd_size, cmd_port);
        return;
    }
    if (command == "play") {
        // If command didn't specify size/offset get from the play properties
        if (!has_cmd_offset)
            cmd_offset = d_wrapped_ref->get_play_offset(cmd_port);
        if (!has_cmd_size)
            cmd_size = d_wrapped_ref->get_play_size(cmd_port);
        bool repeat = pmt::to_bool(
            pmt::dict_ref(msg, replay_cmd_repeat_key(), pmt::from_bool(false)));
        play(cmd_offset, cmd_size, cmd_port, cmd_time, repeat);
        return;
    }
    if (command == "stop") {
        stop_playback(cmd_port);
        return;
    }
    if (command == "get_record_fullness") {
        pmt::pmt_t out_dict =
            pmt::dict_add(pmt::make_dict(),
                          replay_mem_fullness_key(),
                          pmt::from_uint64(d_wrapped_ref->get_record_fullness(cmd_port)));
        message_port_pub(replay_debug_port_key(), out_dict);
        return;
    }
    if (command == "get_block_size") {
        pmt::pmt_t out_dict =
            pmt::dict_add(pmt::make_dict(),
                          replay_mem_size_key(),
                          pmt::from_uint64(d_wrapped_ref->get_mem_size()));
        out_dict = pmt::dict_add(out_dict,
                                 replay_word_size_key(),
                                 pmt::from_uint64(d_wrapped_ref->get_word_size()));
        message_port_pub(replay_debug_port_key(), out_dict);
        return;
    }

    d_logger->error("Invalid Command for RFNoC Replay Block: {}", pmt::write_string(msg));
}

} /* namespace uhd */
} /* namespace gr */
/* -*- c++ -*- */
/*
 * Copyright 2025 Ettus Research, a National Instruments Brand
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gr_uhd_common.h"
#include "rfnoc_mb_controller_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/logger.h>
#include <gnuradio/pmt_fmt.h>
#include <pmt/pmt.h>
#include <uhd/rfnoc/mb_controller.hpp>
#include <stdexcept>

namespace gr {
namespace uhd {

rfnoc_mb_controller::sptr rfnoc_mb_controller::make(rfnoc_graph::sptr graph,
                                                    const size_t mb_index)
{
    return std::make_shared<rfnoc_mb_controller_impl>(graph->get_rfnoc_graph(), mb_index);
}

rfnoc_mb_controller_impl::rfnoc_mb_controller_impl(::uhd::rfnoc::rfnoc_graph::sptr graph,
                                                   const size_t mb_index)
    : gr::block("rfnoc_mb_controller",
                gr::io_signature::make(0, 0, 0),
                gr::io_signature::make(0, 0, 0))
{
    gr::configure_default_loggers(d_logger, d_debug_logger, "rfnoc_mb_controller");
    _update_graph_and_mb_idx(graph, mb_index);

    // Set up message port
    message_port_register_in(pmt::mp("command"));
    set_msg_handler(pmt::mp("command"),
                    [this](pmt::pmt_t msg) { this->_msg_handler_command(msg); });
}

rfnoc_mb_controller_impl::~rfnoc_mb_controller_impl() {}

void rfnoc_mb_controller_impl::_update_graph_and_mb_idx(
    ::uhd::rfnoc::rfnoc_graph::sptr graph, const size_t mb_index)
{
    d_graph = graph;
    d_mb_index = mb_index;
    if (d_graph && d_mb_index != ALL_MBOARDS) {
        if (d_mb_index >= d_graph->get_num_mboards()) {
            throw std::runtime_error("Invalid motherboard index");
        }
    }
}

void rfnoc_mb_controller_impl::set_rfnoc_graph(rfnoc_graph::sptr graph)
{
    _update_graph_and_mb_idx(graph->get_rfnoc_graph(), d_mb_index);
}

void rfnoc_mb_controller_impl::set_mb_index(const size_t mb_index)
{
    _update_graph_and_mb_idx(d_graph, mb_index);
}

size_t rfnoc_mb_controller_impl::get_mb_index() const { return d_mb_index; }

void rfnoc_mb_controller_impl::set_time_source(const std::string& source)
{
    if (!d_graph) {
        throw std::runtime_error("RFNoC Graph not initialized");
    }
    if (d_mb_index == ALL_MBOARDS) {
        for (size_t i = 0; i < d_graph->get_num_mboards(); ++i) {
            d_graph->get_mb_controller(i)->set_time_source(source);
        }
        return;
    }
    d_graph->get_mb_controller(d_mb_index)->set_time_source(source);
}

std::string rfnoc_mb_controller_impl::get_time_source() const
{
    if (!d_graph) {
        throw std::runtime_error("RFNoC Graph not initialized");
    }
    if (d_mb_index == ALL_MBOARDS) {
        d_logger->warn("get_time_source() called with ALL_MBOARDS index; returning time "
                       "source on first motherboard");
        return d_graph->get_mb_controller(0)->get_time_source();
    }
    return d_graph->get_mb_controller(d_mb_index)->get_time_source();
}

std::vector<std::string> rfnoc_mb_controller_impl::get_time_sources() const
{
    if (!d_graph) {
        throw std::runtime_error("RFNoC Graph not initialized");
    }
    if (d_mb_index == ALL_MBOARDS) {
        d_logger->warn("get_time_sources() called with ALL_MBOARDS index; returning time "
                       "sources on first motherboard");
        return d_graph->get_mb_controller(0)->get_time_sources();
    }
    return d_graph->get_mb_controller(d_mb_index)->get_time_sources();
}

void rfnoc_mb_controller_impl::set_clock_source(const std::string& source)
{
    if (!d_graph) {
        throw std::runtime_error("RFNoC Graph not initialized");
    }
    if (d_mb_index == ALL_MBOARDS) {
        for (size_t i = 0; i < d_graph->get_num_mboards(); ++i) {
            d_graph->get_mb_controller(i)->set_clock_source(source);
        }
        return;
    }
    d_graph->get_mb_controller(d_mb_index)->set_clock_source(source);
}

std::string rfnoc_mb_controller_impl::get_clock_source() const
{
    if (!d_graph) {
        throw std::runtime_error("RFNoC Graph not initialized");
    }
    if (d_mb_index == ALL_MBOARDS) {
        d_logger->warn("get_clock_source() called with ALL_MBOARDS index; returning "
                       "clock source on first motherboard");
        return d_graph->get_mb_controller(0)->get_clock_source();
    }
    return d_graph->get_mb_controller(d_mb_index)->get_clock_source();
}

std::vector<std::string> rfnoc_mb_controller_impl::get_clock_sources() const
{
    if (!d_graph) {
        throw std::runtime_error("RFNoC Graph not initialized");
    }
    if (d_mb_index == ALL_MBOARDS) {
        d_logger->warn("get_clock_sources() called with ALL_MBOARDS index; returning "
                       "clock sources on first motherboard");
        return d_graph->get_mb_controller(0)->get_clock_sources();
    }
    return d_graph->get_mb_controller(d_mb_index)->get_clock_sources();
}

void rfnoc_mb_controller_impl::set_time_now(const ::uhd::time_spec_t& time,
                                            const size_t tk_idx)
{
    if (!d_graph) {
        throw std::runtime_error("RFNoC Graph not initialized");
    }
    if (d_mb_index == ALL_MBOARDS) {
        for (size_t i = 0; i < d_graph->get_num_mboards(); ++i) {
            d_graph->get_mb_controller(i)->get_timekeeper(tk_idx)->set_time_now(time);
        }
        return;
    }
    d_graph->get_mb_controller(d_mb_index)->get_timekeeper(tk_idx)->set_time_now(time);
}

void rfnoc_mb_controller_impl::set_time_next_pps(const ::uhd::time_spec_t& time,
                                                 const size_t tk_idx)
{
    if (!d_graph) {
        throw std::runtime_error("RFNoC Graph not initialized");
    }
    if (d_mb_index == ALL_MBOARDS) {
        for (size_t i = 0; i < d_graph->get_num_mboards(); ++i) {
            d_graph->get_mb_controller(i)->get_timekeeper(tk_idx)->set_time_next_pps(
                time);
        }
        return;
    }
    d_graph->get_mb_controller(d_mb_index)
        ->get_timekeeper(tk_idx)
        ->set_time_next_pps(time);
}

::uhd::time_spec_t rfnoc_mb_controller_impl::get_time_now(const size_t tk_idx) const
{
    if (!d_graph) {
        throw std::runtime_error("RFNoC Graph not initialized");
    }
    if (d_mb_index == ALL_MBOARDS) {
        d_logger->warn("get_time_now() called with ALL_MBOARDS index; returning time on "
                       "first motherboard");
        return d_graph->get_mb_controller(0)->get_timekeeper(tk_idx)->get_time_now();
    }
    return d_graph->get_mb_controller(d_mb_index)->get_timekeeper(tk_idx)->get_time_now();
}

::uhd::time_spec_t rfnoc_mb_controller_impl::get_time_last_pps(const size_t tk_idx) const
{
    if (!d_graph) {
        throw std::runtime_error("RFNoC Graph not initialized");
    }
    if (d_mb_index == ALL_MBOARDS) {
        d_logger->warn("get_time_last_pps() called with ALL_MBOARDS index; returning "
                       "time on first motherboard");
        return d_graph->get_mb_controller(0)->get_timekeeper(tk_idx)->get_time_last_pps();
    }
    return d_graph->get_mb_controller(d_mb_index)
        ->get_timekeeper(tk_idx)
        ->get_time_last_pps();
}

void rfnoc_mb_controller_impl::synchronize(::uhd::time_spec_t& time)
{
    if (!d_graph) {
        throw std::runtime_error("RFNoC Graph not initialized");
    }
    d_graph->synchronize_devices(time, false);
}

::uhd::time_spec_t rfnoc_mb_controller_impl::_pmt_to_time_spec(pmt::pmt_t pmt_time) const
{
    if (pmt::is_pair(pmt_time)) {
        // Pair format: (int_seconds . frac_seconds)
        return ::uhd::time_spec_t(pmt::to_long(pmt::car(pmt_time)),
                                  pmt::to_double(pmt::cdr(pmt_time)));
    } else if (pmt::is_number(pmt_time)) {
        // Number format: double seconds
        return ::uhd::time_spec_t(pmt::to_double(pmt_time));
    } else {
        throw std::runtime_error("Invalid PMT type for time specification");
    }
}

void rfnoc_mb_controller_impl::_msg_handler_command(pmt::pmt_t msg)
{
    // Convert pair to dict if necessary
    if (!pmt::is_dict(msg) && pmt::is_pair(msg)) {
        d_logger->debug(
            "Command message is pair, converting to dict: '{}': car({}), cdr({})",
            msg,
            pmt::car(msg),
            pmt::cdr(msg));
        msg = pmt::dict_add(pmt::make_dict(), pmt::car(msg), pmt::cdr(msg));
    }

    // Make sure we use dicts!
    if (!pmt::is_dict(msg)) {
        d_logger->error("Command message is neither dict nor pair: {}",
                        pmt::write_string(msg));
        return;
    }

    d_debug_logger->debug("Processing MB controller command message {}",
                          pmt::write_string(msg));

    // Process each key-value pair in the message
    pmt::pmt_t msg_items = pmt::dict_items(msg);
    for (size_t i = 0; i < pmt::length(msg_items); i++) {
        try {
            pmt::pmt_t key = pmt::car(pmt::nth(i, msg_items));
            pmt::pmt_t val = pmt::cdr(pmt::nth(i, msg_items));
            std::string cmd = pmt::symbol_to_string(key);

            // Dispatch commands
            if (cmd == "time_source") {
                set_time_source(pmt::symbol_to_string(val));
            } else if (cmd == "clock_source") {
                set_clock_source(pmt::symbol_to_string(val));
            } else if (cmd == "time_now") {
                if (pmt::is_dict(val)) {
                    // Dict format: {"time": timespec, "tk_idx": index}
                    pmt::pmt_t time_pmt =
                        pmt::dict_ref(val, pmt::mp("time"), pmt::from_double(0.0));
                    ::uhd::time_spec_t time_spec = _pmt_to_time_spec(time_pmt);
                    size_t tk_idx = pmt::to_long(
                        pmt::dict_ref(val, pmt::mp("tk_idx"), pmt::from_long(0)));
                    set_time_now(time_spec, tk_idx);
                } else {
                    // Direct time format (pair or number)
                    ::uhd::time_spec_t time_spec = _pmt_to_time_spec(val);
                    set_time_now(time_spec, 0);
                }
            } else if (cmd == "time_next_pps") {
                if (pmt::is_dict(val)) {
                    // Dict format: {"time": timespec, "tk_idx": index}
                    pmt::pmt_t time_pmt =
                        pmt::dict_ref(val, pmt::mp("time"), pmt::from_double(0.0));
                    ::uhd::time_spec_t time_spec = _pmt_to_time_spec(time_pmt);
                    size_t tk_idx = pmt::to_long(
                        pmt::dict_ref(val, pmt::mp("tk_idx"), pmt::from_long(0)));
                    set_time_next_pps(time_spec, tk_idx);
                } else {
                    // Direct time format (pair or number)
                    ::uhd::time_spec_t time_spec = _pmt_to_time_spec(val);
                    set_time_next_pps(time_spec, 0);
                }
            } else if (cmd == "synchronize") {
                ::uhd::time_spec_t sync_time = _pmt_to_time_spec(val);
                synchronize(sync_time);
            } else if (cmd == "mb_index") {
                set_mb_index(pmt::to_long(val));
            } else {
                d_logger->warn("Unknown command: {}", cmd);
            }
        } catch (const std::exception& e) {
            d_logger->error("Error processing command: {}", e.what());
        }
    }
}

} // namespace uhd
} // namespace gr

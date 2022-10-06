/* -*- c++ -*- */
/*
 * Copyright 2019 Ettus Research, a National Instruments Brand.
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gr_uhd_common.h"
#include <gnuradio/logger.h>
#include <gnuradio/uhd/rfnoc_block.h>
#include <gnuradio/uhd/rfnoc_graph.h>
#include <uhd/rfnoc/mb_controller.hpp>
#include <uhd/rfnoc_graph.hpp>
#include <unordered_map>
#include <atomic>
#include <stdexcept>

using ::uhd::rfnoc::block_id_t;
using namespace gr::uhd;
using namespace ::uhd;


class rfnoc_graph_impl : public rfnoc_graph
{
public:
    rfnoc_graph_impl(const device_addr_t& dev_addr)
        : _graph(::uhd::rfnoc::rfnoc_graph::make(dev_addr))
    {
        gr::configure_default_loggers(d_logger, d_debug_logger, "rfnoc_graph");
    }

    void connect(const std::string& src_block_id,
                 const size_t src_block_port,
                 const std::string& dst_block_id,
                 const size_t dst_block_port,
                 const bool skip_property_propagation) override
    {
        d_logger->debug("Connecting {:s}:{:d} -> {:s}:{:d}",
                        src_block_id,
                        src_block_port,
                        dst_block_id,
                        dst_block_port);
        if (_tx_streamers.count(src_block_id)) {
            if (_rx_streamers.count(dst_block_id)) {
                throw std::runtime_error("Cannot connect RFNoC streamers directly!");
            }
            _graph->connect(_tx_streamers.at(src_block_id),
                            src_block_port,
                            block_id_t(dst_block_id),
                            dst_block_port);
            return;
        }
        if (_rx_streamers.count(dst_block_id)) {
            _graph->connect(src_block_id,
                            src_block_port,
                            _rx_streamers.at(dst_block_id),
                            dst_block_port);
            return;
        }

        _graph->connect(block_id_t(src_block_id),
                        src_block_port,
                        block_id_t(dst_block_id),
                        dst_block_port);
    }

    void connect(const std::string& block1,
                 const std::string& block2,
                 bool skip_property_propagation) override
    {
        connect(block1, 0, block2, 0, skip_property_propagation);
    }

    void connect(rfnoc_block::sptr src_block,
                 const size_t src_block_port,
                 rfnoc_block::sptr dst_block,
                 const size_t dst_block_port,
                 const bool skip_property_propagation)
    {
        connect(src_block->get_unique_id(),
                src_block_port,
                dst_block->get_unique_id(),
                dst_block_port,
                skip_property_propagation);
    }


    ::uhd::rx_streamer::sptr create_rx_streamer(const size_t num_ports,
                                                const stream_args_t& args) override
    {
        auto streamer = _graph->create_rx_streamer(num_ports, args);
        const std::string streamer_id =
            std::dynamic_pointer_cast<::uhd::rfnoc::node_t>(streamer)->get_unique_id();
        _rx_streamers.insert({ streamer_id, streamer });
        return streamer;
    }

    ::uhd::tx_streamer::sptr create_tx_streamer(const size_t num_ports,
                                                const stream_args_t& args) override
    {
        auto streamer = _graph->create_tx_streamer(num_ports, args);
        const std::string streamer_id =
            std::dynamic_pointer_cast<::uhd::rfnoc::node_t>(streamer)->get_unique_id();
        _tx_streamers.insert({ streamer_id, streamer });
        return streamer;
    }

    void commit() override
    {
        if (!_commit_called.exchange(true)) {
            _graph->commit();
        }
    }

    std::string get_block_id(const std::string& block_name,
                             const int device_select,
                             const int block_select) override
    {
        std::string block_hint = block_name;
        if (device_select >= 0) {
            block_hint = std::to_string(device_select) + "/" + block_hint;
        }
        if (block_select >= 0) {
            block_hint = block_hint + "#" + std::to_string(block_select);
        }

        auto block_ids = _graph->find_blocks(block_hint);
        if (block_ids.empty()) {
            return "";
        }

        // If the hint produced a unique hit, then we return that
        if (block_ids.size() == 1) {
            return block_ids.front().to_string();
        }

        // If the hint produced multiple hits, then we default to one that was
        // not yet acquired. If that fails, return any one.
        std::lock_guard<std::mutex> l(_block_ref_mutex);
        for (const auto& block_id : block_ids) {
            const std::string block_id_str = block_id.to_string();
            if (!_acqd_block_refs.count(block_id_str)) {
                return block_id_str;
            }
        }
        return block_ids.front().to_string();
    }

    void set_time_source(const std::string& source, const size_t mb_index) override
    {
        _graph->get_mb_controller(mb_index)->set_time_source(source);
    }

    void set_clock_source(const std::string& source, const size_t mb_index) override
    {
        _graph->get_mb_controller(mb_index)->set_clock_source(source);
    }

    ::uhd::rfnoc::noc_block_base::sptr get_block_ref(const std::string& block_id,
                                                     const size_t max_ref_count) override
    {
        std::lock_guard<std::mutex> l(_block_ref_mutex);
        if (max_ref_count == 0) {
            throw std::runtime_error("Invalid max_ref_count 0!");
        }

        auto block_ref = _graph->get_block(block_id_t(block_id));
        const std::string real_block_id = block_ref->get_unique_id();

        // If this is the first time we encounter this block ID, create a ref
        // counter for it
        if (!_acqd_block_refs.count(real_block_id)) {
            _acqd_block_refs.insert({ real_block_id, 0 });
        }
        // Make sure the max ref count limit is set. If it already exists,
        // decrease it if max_ref_count is smaller than what we already have.
        if (!_max_ref_count.count(real_block_id)) {
            _max_ref_count.insert({ real_block_id, max_ref_count });
        } else {
            _max_ref_count[real_block_id] =
                std::min(max_ref_count, _max_ref_count[real_block_id]);
        }
        // Now check we haven't exceeded the ref count limit
        if (_acqd_block_refs.at(real_block_id) >= _max_ref_count.at(real_block_id)) {
            throw std::runtime_error(
                std::string("Attempting to get another reference to block ") +
                real_block_id);
        }
        // Increase ref counter for this block ID
        _acqd_block_refs[real_block_id]++;

        // And Bob's your uncle
        return block_ref;
    }


private:
    std::atomic<bool> _commit_called{ false };
    ::uhd::rfnoc::rfnoc_graph::sptr _graph;

    std::unordered_map<std::string, ::uhd::rx_streamer::sptr> _rx_streamers;
    std::unordered_map<std::string, ::uhd::tx_streamer::sptr> _tx_streamers;

    std::mutex _block_ref_mutex;
    std::unordered_map<std::string, size_t> _acqd_block_refs;
    std::unordered_map<std::string, size_t> _max_ref_count;

    gr::logger_ptr d_logger, d_debug_logger;
};


rfnoc_graph::sptr rfnoc_graph::make(const device_addr_t& dev_addr)
{
    check_abi();
    return std::make_shared<rfnoc_graph_impl>(dev_addr);
}

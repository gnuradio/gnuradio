/* -*- c++ -*- */
/*
 * Copyright 2025 Ettus Research, a National Instruments Brand
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_GR_UHD_RFNOC_MB_CONTROLLER_IMPL_H
#define INCLUDED_GR_UHD_RFNOC_MB_CONTROLLER_IMPL_H

#include <gnuradio/uhd/rfnoc_mb_controller.h>
#include <pmt/pmt.h>
#include <uhd/rfnoc_graph.hpp>
#include <string>
#include <vector>

namespace gr {
namespace uhd {

class rfnoc_mb_controller_impl : public rfnoc_mb_controller
{
public:
    rfnoc_mb_controller_impl(::uhd::rfnoc::rfnoc_graph::sptr graph,
                             const size_t mb_index);
    ~rfnoc_mb_controller_impl() override;

    /*** API *****************************************************************/
    void set_rfnoc_graph(rfnoc_graph::sptr graph) override;
    void set_mb_index(const size_t mb_index) override;
    size_t get_mb_index() const override;

    void set_time_source(const std::string& source) override;
    std::string get_time_source() const override;
    std::vector<std::string> get_time_sources() const override;

    void set_clock_source(const std::string& source) override;
    std::string get_clock_source() const override;
    std::vector<std::string> get_clock_sources() const override;

    void set_time_now(const ::uhd::time_spec_t& time, const size_t tk_idx = 0) override;
    void set_time_next_pps(const ::uhd::time_spec_t& time,
                           const size_t tk_idx = 0) override;
    ::uhd::time_spec_t get_time_now(const size_t tk_idx = 0) const override;
    ::uhd::time_spec_t get_time_last_pps(const size_t tk_idx = 1) const override;

    void synchronize(::uhd::time_spec_t& time) override;

private:
    void _update_graph_and_mb_idx(::uhd::rfnoc::rfnoc_graph::sptr graph,
                                  const size_t mb_index);
    void _msg_handler_command(pmt::pmt_t msg);
    ::uhd::time_spec_t _pmt_to_time_spec(pmt::pmt_t pmt_time) const;

    ::uhd::rfnoc::rfnoc_graph::sptr d_graph;
    size_t d_mb_index = ALL_MBOARDS;
};

} // namespace uhd
} // namespace gr

#endif /* INCLUDED_GR_UHD_RFNOC_MB_CONTROLLER_IMPL_H */
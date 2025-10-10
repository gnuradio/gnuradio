/* -*- c++ -*- */
/*
 * Copyright 2025 Ettus Research, a National Instruments Brand
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_GR_UHD_RFNOC_MB_CONTROLLER_H
#define INCLUDED_GR_UHD_RFNOC_MB_CONTROLLER_H

#include <gnuradio/block.h>
#include <gnuradio/uhd/api.h>
#include <gnuradio/uhd/rfnoc_graph.h>
#include <uhd/types/time_spec.hpp>
#include <memory>

namespace gr {
namespace uhd {

/*! GNU Radio block for RFNoC motherboard controller
 *
 * This block provides GNU Radio access to RFNoC motherboard controller
 * functionality, allowing control of motherboard-level features such as
 * time synchronization, reference clocks, and other system-level settings.
 */
class GR_UHD_API rfnoc_mb_controller : virtual public gr::block
{
public:
    static constexpr size_t DEFAULT_MB_INDEX = 0;
    static constexpr size_t ALL_MBOARDS = static_cast<size_t>(-1);

    using sptr = std::shared_ptr<rfnoc_mb_controller>;

    //! Factory function to create an RFNoC motherboard controller block
    //
    // \param graph Reference to the flowgraph's RFNoC graph.
    // \param mb_index Motherboard index to control
    static sptr make(rfnoc_graph::sptr graph = nullptr,
                     const size_t mb_index = ALL_MBOARDS);


    //! Set the RFNoC graph this controller is associated with
    //
    // If the graph is changed after construction, the internal
    // motherboard controller will be updated to reflect the new graph.
    // If no graph was provided at construction time, this must be called
    // before any other method is used.
    virtual void set_rfnoc_graph(rfnoc_graph::sptr graph) = 0;

    //! Set the motherboard index this controller manages
    virtual void set_mb_index(const size_t mb_index) = 0;

    //! Get the motherboard index this controller manages
    virtual size_t get_mb_index() const = 0;

    //! Set the time source for the motherboard
    //
    // \param source The time source (e.g., "internal", "external", "gpsdo")
    virtual void set_time_source(const std::string& source) = 0;

    //! Get the current time source
    virtual std::string get_time_source() const = 0;

    //! Get available time sources
    virtual std::vector<std::string> get_time_sources() const = 0;

    //! Set the clock source for the motherboard
    //
    // \param source The clock source (e.g., "internal", "external", "gpsdo")
    virtual void set_clock_source(const std::string& source) = 0;

    //! Get the current clock source
    virtual std::string get_clock_source() const = 0;

    //! Get available clock sources
    virtual std::vector<std::string> get_clock_sources() const = 0;

    //! Set the motherboard time
    //
    // \param time The time to set
    // \param tk_idx The timekeeper index
    virtual void set_time_now(const ::uhd::time_spec_t& time,
                              const size_t tk_idx = 0) = 0;

    //! Set the motherboard time at the next PPS edge
    //
    // \param time The time to set at next PPS
    // \param tk_idx The timekeeper index
    virtual void set_time_next_pps(const ::uhd::time_spec_t& time,
                                   const size_t tk_idx = 0) = 0;

    //! Get the current motherboard time
    // \param tk_idx The timekeeper index
    virtual ::uhd::time_spec_t get_time_now(const size_t tk_idx = 0) const = 0;

    //! Get the time at the last PPS edge
    // \param tk_idx The timekeeper index
    virtual ::uhd::time_spec_t get_time_last_pps(const size_t tk_idx = 0) const = 0;

    //! Synchronize time across multiple motherboards
    //
    // \param mb_controllers Vector of motherboard controllers to synchronize
    virtual void synchronize(::uhd::time_spec_t& time) = 0;
};

} // namespace uhd
} // namespace gr

#endif /* INCLUDED_GR_UHD_RFNOC_MB_CONTROLLER_H */
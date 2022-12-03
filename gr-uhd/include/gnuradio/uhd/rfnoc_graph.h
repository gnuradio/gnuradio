/* -*- c++ -*- */
/*
 * Copyright 2019 Ettus Research, a National Instruments Brand.
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_GR_UHD_RFNOC_GRAPH_H
#define INCLUDED_GR_UHD_RFNOC_GRAPH_H

#include <gnuradio/uhd/api.h>
#include <uhd/rfnoc/noc_block_base.hpp>
#include <uhd/rfnoc_graph.hpp>
#include <uhd/stream.hpp>
#include <uhd/types/device_addr.hpp>
#include <uhd/types/sensors.hpp>
#include <memory>

namespace gr {
namespace uhd {

class rfnoc_block;

/*! GNU Radio-specific wrapper for uhd::rfnoc::rfnoc_graph
 *
 * This wraps uhd::rfnoc::rfnoc_graph for simpler insertion into GNU Radio flow
 * graphs. All API calls match those on said class.
 */
class GR_UHD_API rfnoc_graph
{
public:
    using sptr = std::shared_ptr<rfnoc_graph>;

    static const size_t NULL_ADAPTER_ID =
        static_cast<size_t>(::uhd::transport::NULL_ADAPTER_ID);

    static sptr make(const ::uhd::device_addr_t& dev_addr);

    virtual ~rfnoc_graph() {}

    //! Connect two blocks, or a block to a streamer, or a streamer to a block
    //
    // \param src_block The block ID of the source block (e.g., "0/Radio#0)
    // \param src_block_port The port on the source block to connect to
    // \param dst_block The block ID of the destination block (e.g., "0/DDC#0)
    // \param dst_block_port The port on the destination block to connect to
    // \param skip_property_propagation Disable property propagation on this
    //                                  connection (see the UHD documentation)
    virtual void connect(const std::string& src_block,
                         const size_t src_block_port,
                         const std::string& dst_block,
                         const size_t dst_block_port,
                         const bool skip_property_propagation = false) = 0;

    //! Convenience overload: Defaults to port 0 on both blocks
    //
    // \param src_block The block ID of the source block (e.g., "0/Radio#0)
    // \param dst_block The block ID of the destination block (e.g., "0/DDC#0)
    // \param skip_property_propagation Disable property propagation on this
    //                                  connection (see the UHD documentation)
    virtual void connect(const std::string& src_block,
                         const std::string& dst_block,
                         const bool skip_property_propagation = false) = 0;

    //! Create an RX streamer
    //
    // Note: This streamer is not connected to anything after creation.
    //
    // See also the UHD documentation for uhd::rfnoc::rfnoc_graph::create_rx_streamer().
    //
    // \param num_ports Number of streaming ports
    // \param args Stream args.
    virtual ::uhd::rx_streamer::sptr
    create_rx_streamer(const size_t num_ports, const ::uhd::stream_args_t& args) = 0;

    //! Create a TX streamer
    //
    // Note: This streamer is not connected to anything after creation.
    //
    // See also the UHD documentation for uhd::rfnoc::rfnoc_graph::create_tx_streamer().
    //
    // \param num_ports Number of streaming ports
    // \param args Stream args.
    virtual ::uhd::tx_streamer::sptr
    create_tx_streamer(const size_t num_ports, const ::uhd::stream_args_t& args) = 0;

    //! Set the desired adapter ID for a streamer connection
    //
    // If it is desired to connect a streamer to the device using a specific
    // adapter ID, this method needs to be called before calling connect().
    //
    // For more detail on adapter IDs, see the UHD documentation (e.g.,
    // ::uhd::rfnoc::rfnoc_graph::connect()). Note that this does not have a
    // corresponding API call in UHD: There, the connect() call is atomic and
    // takes the adapter ID as an argument. The reason this is different in the
    // GNU Radio implementation is that this makes it much easier to generate
    // code from GRC for C++ and Python. It reduces the number of connect calls
    // down to 1 (from 3 in UHD) and allows the streamer blocks to set this
    // property on the streamer-edges before connect calls are made later on.
    virtual void set_streamer_adapter_id(const std::string& stream_block_id,
                                         const size_t port,
                                         const size_t adapter_id) = 0;

    //! Commit the graph and run initial checks
    //
    // See ::uhd::rfnoc::rfnoc_graph::commit() for more documentation.
    virtual void commit() = 0;

    /*! Return a valid block ID string, if it exists, or an empty string, if not
     *
     * This will check the available blocks on the connected devices and see if
     * they match the block name, device ID, and block number.
     */
    virtual std::string get_block_id(const std::string& block_name,
                                     const int device_select,
                                     const int block_select) = 0;

    //! Set time source on the specified motherboard
    //
    // Note: This is a convenience call, it directly dereferences the underlying
    // motherboard controller.
    //
    // \param source Time source (e.g., "internal")
    // \param mb_index Motherboard index, starting at 0
    virtual void set_time_source(const std::string& source, const size_t mb_index) = 0;

    //! Set clock source on the specified motherboard
    //
    // Note: This is a convenience call, it directly dereferences the underlying
    // motherboard controller.
    //
    // \param source Clock source (e.g., "internal")
    // \param mb_index Motherboard index, starting at 0
    virtual void set_clock_source(const std::string& source, const size_t mb_index) = 0;

    //! Return a reference to the block with a given block ID
    //
    // This allows to retrieve a shared pointer to a block controller with a
    // limit. It keeps track of the references given out, and will throw an
    // exception if called more than max_ref_count times. This is to help ensure
    // that GNU Radio blocks don't share block controllers unintentionally.
    //
    // \param block_id A valid block ID. Use get_block_id() to make sure it is
    //                 valid.
    // \param max_ref_count The maximum number of references that are doled out.
    //
    // \throws std::runtime_error if more than \p max_ref_count references are
    //                            given out
    virtual ::uhd::rfnoc::noc_block_base::sptr
    get_block_ref(const std::string& block_id, const size_t max_ref_count) = 0;

    /*!
     * Get the current time registers.
     *
     * \param mboard the motherboard index 0 to M-1
     * \param timekeeper the timekeeper index within the motherboard
     * \return the current usrp time
     */
    virtual ::uhd::time_spec_t get_time_now(size_t mboard = 0, size_t timekeeper = 0) = 0;

    /*!
     * Get the time when the last pps pulse occurred.
     * \param mboard the motherboard index 0 to M-1
     * \param timekeeper the timekeeper index within the motherboard
     * \return the current usrp time
     */
    virtual ::uhd::time_spec_t get_time_last_pps(size_t mboard = 0,
                                                 size_t timekeeper = 0) = 0;

    /*!
     * Sets the time registers immediately.
     * \param time_spec the new time
     * \param mboard the motherboard index 0 to M-1
     * \param timekeeper the timekeeper index within the motherboard
     */
    virtual void set_time_now(const ::uhd::time_spec_t& time_spec,
                              size_t mboard = 0,
                              size_t timekeeper = 0) = 0;

    /*!
     * Set the time registers at the next pps.
     * \param time_spec the new time
     * \param mboard the motherboard index 0 to M-1
     * \param timekeeper the timekeeper index within the motherboard
     */
    virtual void set_time_next_pps(const ::uhd::time_spec_t& time_spec,
                                   size_t mboard = 0,
                                   size_t timekeeper = 0) = 0;

    /*!
     * Get a list of possible motherboard sensor names.
     *
     * \param mboard the motherboard index 0 to M-1
     * \return a vector of sensor names
     */
    virtual std::vector<std::string> get_mboard_sensor_names(size_t mboard = 0) = 0;

    /*!
     * Get a motherboard sensor value.
     *
     * \param name the name of the sensor
     * \param mboard the motherboard index 0 to M-1
     * \return a sensor value object
     */
    virtual ::uhd::sensor_value_t get_mboard_sensor(const std::string& name,
                                                    size_t mboard = 0) = 0;
};

} // namespace uhd
} // namespace gr

#endif /* INCLUDED_GR_UHD_RFNOC_GRAPH_H */

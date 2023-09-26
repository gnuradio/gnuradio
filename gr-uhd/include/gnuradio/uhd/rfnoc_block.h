/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 * Copyright 2020 Ettus Research, A National Instruments Brand.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_UHD_RFNOC_BLOCK_H
#define INCLUDED_UHD_RFNOC_BLOCK_H

#include <gnuradio/block.h>
#include <gnuradio/uhd/rfnoc_graph.h>
#include <pmt/pmt.h>
#include <uhd/rfnoc/noc_block_base.hpp>
#include <unordered_map>
#include <functional>
#include <string>

namespace gr {
namespace uhd {

/*! Base class for RFNoC blocks controlled by GNU Radio
 *
 * Any GNU Radio block that is meant to control an RFNoC block
 * should be derived from this class.
 */
class GR_UHD_API rfnoc_block : public gr::block
{
protected:
    // \param block_ref A reference to the underlying block controller
    rfnoc_block(::uhd::rfnoc::noc_block_base::sptr block_ref);

    rfnoc_block() {} // For virtual subclassing

public:
    using sptr = std::shared_ptr<rfnoc_block>;

    //! Factory function to create a UHD block controller reference
    //
    // \param graph Reference to the flowgraph's RFNoC graph
    // \param block_args Block args
    // \param block_name Block name (e.g. "DDC")
    // \param device_select Device index (motherboard index)
    // \param block_select Block index
    // \param max_ref_count Maximum number of references this block can have in
    //                      the GNU Radio flow graph
    static ::uhd::rfnoc::noc_block_base::sptr
    make_block_ref(rfnoc_graph::sptr graph,
                   const ::uhd::device_addr_t& block_args,
                   const std::string& block_name,
                   const int device_select = -1,
                   const int block_select = -1,
                   const size_t max_ref_count = 1);

    //! Return a type-cast block reference, or throw if the cast failed.
    //
    // \throws std::runtime_error if there is no valid block reference
    template <typename block_type>
    std::shared_ptr<block_type> get_block_ref()
    {
        auto cast_block_ref = std::dynamic_pointer_cast<block_type>(d_block_ref);
        if (!cast_block_ref) {
            throw std::runtime_error(
                std::string(
                    "Unable to cast the following block into its desired type: ") +
                d_block_ref->get_unique_id());
        }
        return cast_block_ref;
    }

    /*! Return the unique ID of the underlying block
     */
    std::string get_unique_id() const;

    // GNU Radio-specific overrides

    //! This method should never be called by RFNoC blocks, they do the work
    // in the FPGA.
    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) final;

    /*! Set multiple properties coming from a dictionary
     *
     * See the [UHD
     * manual](https://uhd.readthedocs.io/en/latest/classuhd_1_1rfnoc_1_1node__t.html#abf34d4be8fe7a602b27927194195f1f6)
     * for details.
     *
     * This function allows the client to override the \p instance parameter
     * for each property key/value pair passed in via the \p props parameter.
     * If the key consists of the property name, followed by a colon (':') and
     * then a number, the number following the colon is used to determine
     * which instance of the property this set pertains to, and the \p
     * instance parameter is ignored for that property. (Note that if the key
     * does not have the colon and instance number override syntax, then
     * \p instance is still used to determine which instance of the property
     * to set. For example, in the following call:
     *
     *     node->set_properties("dog=10,cat:2=5,bird:0=0.5", 1)
     *
     * instance 1 of node's 'dog' property is set to 10, the 1 coming from the
     * instance parameter, instance 2 of the node's 'cat' property is set to
     * 5 due to the override syntax provided in the string, and instance 0 of
     * the node's 'bird' property is set to 0.5 due to its override.
     *
     * If the instance override is malformed, that is, there is no
     * number following the colon, or the number cannot be parsed as an
     * integer, a value_error is thrown.
     *
     * If a key in \p props is not a valid property of this block, a warning is
     * logged, but no error is raised.
     */
    void set_properties(const ::uhd::device_addr_t& props, const size_t instance = 0)
    {
        d_block_ref->set_properties(props, instance);
    }

    /*! Set a specific user property that belongs to this block.
     *
     * Setting a user property will trigger a property resolution. This means
     * that changing this block can have effects on other RFNoC blocks or nodes
     * (like streamers).
     *
     * \param name The name of the property.
     * \param value The new value of the property.
     * \param port The port of the property.
     */
    template <typename T>
    void set_property(const std::string& name, const T& value, const size_t port = 0)
    {
        d_block_ref->set_property<T>(name, value, port);
    }

    /*! Get the value of a specific block argument. \p The type of an argument
     *  must be known at compile time.
     *
     * Note: Despite this being a "getter", this function is not declared const.
     * This is because internally, it can resolve properties, which may cause
     * changes within the object.
     *
     * \param name The name of the property.
     * \param port The port of the property.
     * \return The value of the property.
     */
    template <typename T>
    const T get_property(const std::string& name, const size_t port = 0)
    {
        return d_block_ref->get_property<T>(name, port);
    }

    std::vector<std::string> get_property_ids();


protected:
    using cmd_handler_t = std::function<void(const pmt::pmt_t&, int, const pmt::pmt_t&)>;

    //! Register a new handler for command key \p cmd
    void register_msg_cmd_handler(const std::string& cmd, cmd_handler_t handler);

private:
    /**********************************************************************
     * Command Interface
     **********************************************************************/
    //! Receives commands and handles them
    void _msg_handler_command(pmt::pmt_t msg);

    //! For a given argument, call the associated handler, or if none exists,
    // show a warning through the logging interface.
    void _dispatch_msg_cmd_handler(const std::string& cmd,
                                   const pmt::pmt_t& val,
                                   int chan,
                                   pmt::pmt_t& msg);

    //! Stores the individual command handlers
    std::unordered_map<std::string, cmd_handler_t> _msg_cmd_handlers;

    //! Reference to the underlying RFNoC block
    ::uhd::rfnoc::noc_block_base::sptr d_block_ref;
};

} // namespace uhd
} // namespace gr

#endif /* INCLUDED_UHD_RFNOC_BLOCK_H */

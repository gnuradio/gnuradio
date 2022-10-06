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
#include <uhd/rfnoc/noc_block_base.hpp>
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
                     gr_vector_void_star& output_items);

    template <typename T>
    void set_property(const std::string& name, const T& value, const size_t port = 0)
    {
        d_block_ref->set_property<T>(name, value, port);
    }

    template <typename T>
    const T get_property(const std::string& name, const size_t port = 0)
    {
        return d_block_ref->get_property<T>(name, port);
    }

    std::vector<std::string> get_property_ids();

private:
    //! Reference to the underlying RFNoC block
    ::uhd::rfnoc::noc_block_base::sptr d_block_ref;
};

} // namespace uhd
} // namespace gr

#endif /* INCLUDED_UHD_RFNOC_BLOCK_H */

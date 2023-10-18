/* -*- c++ -*- */
/*
 * Copyright 2019 Ettus Research, a National Instruments Brand.
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_GR_UHD_RFNOC_RX_STREAMER_H
#define INCLUDED_GR_UHD_RFNOC_RX_STREAMER_H

#include <gnuradio/sync_block.h>
#include <gnuradio/uhd/api.h>
#include <gnuradio/uhd/rfnoc_graph.h>
#include <uhd/stream.hpp>

namespace gr {
namespace uhd {

/*! RFNoC Rx Streamer: Block to handle data flow from an RFNoC flow graph into
 * a GNU Radio flow graph.
 *
 * Use this block for ingress into a GNU Radio flow graph. "Rx" is from the
 * viewpoint of the GNU Radio flow graph. For example, if the GNU Radio flow
 * graph is receiving samples from a radio, use this block to transport the
 * samples into GNU Radio.
 *
 * Note: The input ports of this block can only connect to other RFNoC blocks.
 *
 * \ingroup uhd_blk
 */
class GR_UHD_API rfnoc_rx_streamer : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<rfnoc_rx_streamer> sptr;

    /*!
     * \param graph Reference to the graph this block is connected to
     * \param num_chans Number of input- and output ports
     * \param stream_args These will be passed on to
     *                    rfnoc_graph::create_rx_streamer, see that for details.
     *                    The cpu_format and otw_format parts of these args will
     *                    be used to determine the in- and output signatures of
     *                    this block.
     * \param vlen Vector length
     * \param issue_stream_cmd_on_start If true, the streamer sends a stream
     *                                  command upstream.
     * \param start_time_set If true, set start time spec to the stream command
     * \param start_time The time spec for the stream command if start_time_set is true
     */
    static sptr make(rfnoc_graph::sptr graph,
                     const size_t num_chans,
                     const ::uhd::stream_args_t& stream_args,
                     const size_t vlen = 1,
                     const bool issue_stream_cmd_on_start = true,
                     const bool start_time_set = false,
                     const ::uhd::time_spec_t& start_time = ::uhd::time_spec_t(0.0));

    //! Return the unique ID associated with the underlying RFNoC streamer
    virtual std::string get_unique_id() const = 0;
};

} // namespace uhd
} // namespace gr

#endif /* INCLUDED_GR_UHD_RFNOC_RX_STREAMER_H */

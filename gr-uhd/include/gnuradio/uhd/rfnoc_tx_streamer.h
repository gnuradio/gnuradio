/* -*- c++ -*- */
/*
 * Copyright 2019 Ettus Research, a National Instruments Brand.
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_GR_UHD_RFNOC_TX_STREAMER_H
#define INCLUDED_GR_UHD_RFNOC_TX_STREAMER_H

#include <gnuradio/sync_block.h>
#include <gnuradio/uhd/api.h>
#include <gnuradio/uhd/rfnoc_graph.h>
#include <uhd/stream.hpp>

namespace gr {
namespace uhd {

/*! RFNoC Tx Streamer: Block to handle data flow from a GNU Radio flow graph
 * into an RFNoC flow graph.
 *
 * Use this block for egress from a GNU Radio flow graph. "Tx" is from the
 * viewpoint of the GNU Radio flow graph. For example, if the GNU Radio flow
 * graph is creating samples to be transmitted to a radio, use this block to
 * transport the samples out of GNU Radio.
 *
 * Note: The output ports of this block can only connect to other RFNoC blocks.
 *
 * \ingroup ettus
 */
class GR_UHD_API rfnoc_tx_streamer : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<rfnoc_tx_streamer> sptr;

    /*!
     * \param graph Reference to the graph this block is connected to
     * \param num_chans Number of input- and output ports
     * \param stream_args These will be passed on to
     *                    rfnoc_graph::create_tx_streamer, see that for details.
     *                    The cpu_format and otw_format parts of these args will
     *                    be used to determine the in- and output signatures of
     *                    this block.
     * \param vlen Vector length
     */
    static sptr make(rfnoc_graph::sptr graph,
                     const size_t num_chans,
                     const ::uhd::stream_args_t& stream_args,
                     const size_t vlen = 1);

    //! Return the unique ID associated with the underlying RFNoC streamer
    virtual std::string get_unique_id() const = 0;
};

} // namespace uhd
} // namespace gr

#endif /* INCLUDED_GR_UHD_RFNOC_TX_STREAMER_H */

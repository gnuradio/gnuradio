/* -*- c++ -*- */
/*
 * Copyright 2019 Ettus Research, a National Instruments Brand.
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_GR_UHD_RFNOC_TX_STREAMER_IMPL_H
#define INCLUDED_GR_UHD_RFNOC_TX_STREAMER_IMPL_H

#include <gnuradio/uhd/rfnoc_tx_streamer.h>

namespace gr {
namespace uhd {

class rfnoc_tx_streamer_impl : public rfnoc_tx_streamer
{
public:
    rfnoc_tx_streamer_impl(rfnoc_graph::sptr graph,
                           const size_t num_chans,
                           const ::uhd::stream_args_t& stream_args,
                           const size_t vlen);
    ~rfnoc_tx_streamer_impl() override;

    /***** API ***************************************************************/
    std::string get_unique_id() const override { return d_unique_id; }

    /***** GNU Radio API *****************************************************/
    bool check_topology(int, int) override;
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;

private:
    //! Bytes per item (e.g. 4 for sc16)
    const size_t d_itemsize;
    //! Input vector length
    const size_t d_vlen;
    //! Reference to the underlying graph
    rfnoc_graph::sptr d_graph;
    //! Stream args
    ::uhd::stream_args_t d_stream_args;
    //! Reference to the actual streamer
    ::uhd::tx_streamer::sptr d_streamer;
    //! Copy of the streamer's block ID
    const std::string d_unique_id;
    //! Stash for the TX metadata
    ::uhd::tx_metadata_t d_metadata;
    //! TX timeout value
    double d_timeout = 1.0;
};

} // namespace uhd
} // namespace gr

#endif /* INCLUDED_GR_UHD_RFNOC_TX_STREAMER_IMPL_H */

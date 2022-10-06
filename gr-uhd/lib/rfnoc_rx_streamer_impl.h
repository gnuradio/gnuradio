/* -*- c++ -*- */
/*
 * Copyright 2019 Ettus Research, a National Instruments Brand.
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_GR_UHD_RFNOC_RX_STREAMER_IMPL_H
#define INCLUDED_GR_UHD_RFNOC_RX_STREAMER_IMPL_H

#include <gnuradio/uhd/rfnoc_rx_streamer.h>

namespace gr {
namespace uhd {

class rfnoc_rx_streamer_impl : public rfnoc_rx_streamer
{
public:
    rfnoc_rx_streamer_impl(rfnoc_graph::sptr graph,
                           const size_t num_chans,
                           const ::uhd::stream_args_t& stream_args,
                           const size_t vlen,
                           const bool issue_stream_cmd_on_start);
    ~rfnoc_rx_streamer_impl() override;

    std::string get_unique_id() const override { return d_unique_id; }

    /***** API ***************************************************************/
    void set_start_time(const ::uhd::time_spec_t& time);

    /***** GNU Radio API *****************************************************/
    bool check_topology(int, int) override;
    bool start() override;
    bool stop() override;
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;

private:
    void flush();

    //! Bytes per item (e.g. 4 for sc16)
    const size_t d_itemsize;
    //! Input vector length
    const size_t d_vlen;
    //! Reference to the underlying graph
    rfnoc_graph::sptr d_graph;
    //! Stream args
    ::uhd::stream_args_t d_stream_args;
    //! Reference to the actual streamer
    ::uhd::rx_streamer::sptr d_streamer;
    //! Copy of the streamer's block ID
    const std::string d_unique_id;
    //! Stash for the TX metadata
    ::uhd::rx_metadata_t d_metadata;
    //! RX timeout value
    double d_timeout = 1.0;
    //! True if the stream should immediately start with the flow graph without
    // external prompting
    const bool d_issue_stream_cmd_on_start;
    //! True if d_start_time holds a value we need to process
    bool d_start_time_set = false;
    //! A start time for the next stream command
    ::uhd::time_spec_t d_start_time;
};

} // namespace uhd
} // namespace gr

#endif /* INCLUDED_GR_UHD_RFNOC_RX_STREAMER_IMPL_H */

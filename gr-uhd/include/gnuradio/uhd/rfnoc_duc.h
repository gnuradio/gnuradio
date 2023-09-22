/* -*- c++ -*- */
/*
 * Copyright 2019 Ettus Research, a National Instruments Brand.
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_GR_UHD_RFNOC_DUC_H
#define INCLUDED_GR_UHD_RFNOC_DUC_H

#include <gnuradio/uhd/api.h>
#include <gnuradio/uhd/rfnoc_block.h>
#include <gnuradio/uhd/rfnoc_graph.h>

namespace gr {
namespace uhd {

/*! RFNoC Digital Upconverter Block
 *
 * \ingroup uhd_blk
 */
class GR_UHD_API rfnoc_duc : virtual public rfnoc_block
{
public:
    typedef std::shared_ptr<rfnoc_duc> sptr;

    /*!
     * \param graph Reference to the rfnoc_graph object this block is attached to
     * \param block_args Additional block arguments
     * \param device_select Device Selection
     * \param instance Instance Selection
     */
    static sptr make(rfnoc_graph::sptr graph,
                     const ::uhd::device_addr_t& block_args,
                     const int device_select,
                     const int instance);

    /*! Set DDS frequency
     */
    virtual double set_freq(const double freq,
                            const size_t chan,
                            const ::uhd::time_spec_t time = ::uhd::time_spec_t::ASAP) = 0;

    /*! Set the sample rate at the input
     *
     * When the DUC block is connected to a TX streamer block, then this will
     * help with the property propagation.
     *
     * \param rate Input Sampling Rate (Hz)
     * \param chan Channel Index
     */
    virtual double set_input_rate(const double rate, const size_t chan) = 0;
};

} // namespace uhd
} // namespace gr

#endif /* INCLUDED_GR_UHD_RFNOC_DUC_H */

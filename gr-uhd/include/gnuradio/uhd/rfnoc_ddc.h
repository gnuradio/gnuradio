/* -*- c++ -*- */
/*
 * Copyright 2019 Ettus Research, a National Instruments Brand.
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_GR_UHD_RFNOC_DDC_H
#define INCLUDED_GR_UHD_RFNOC_DDC_H

#include <gnuradio/uhd/api.h>
#include <gnuradio/uhd/rfnoc_block.h>

namespace gr {
namespace uhd {

/*! RFNoC Digital Downconverter Block
 *
 * \ingroup uhd_blk
 */
class GR_UHD_API rfnoc_ddc : virtual public rfnoc_block
{
public:
    typedef std::shared_ptr<rfnoc_ddc> sptr;

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

    /*! Set the DDS frequency of the DDC
     *
     * \param freq Frequency Shift (Hz)
     * \param chan Channel Index
     * \param time Command Time for the frequency shift
     */
    virtual double set_freq(const double freq,
                            const size_t chan,
                            const ::uhd::time_spec_t time = ::uhd::time_spec_t::ASAP) = 0;

    /*! Manually configure output rate
     *
     * When the DDC block is connected to an RX streamer block, then this will
     * help with the property propagation.
     *
     * \param rate Sampling Rate (Hz)
     * \param chan Channel Index
     */
    virtual double set_output_rate(const double rate, const size_t chan) = 0;
};

} // namespace uhd
} // namespace gr

#endif /* INCLUDED_GR_UHD_RFNOC_DDC_H */

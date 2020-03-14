/* -*- c++ -*- */
/*
 * Copyright 2019 Ettus Research, a National Instruments Brand.
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_GR_UHD_RFNOC_BLOCK_GENERIC_H
#define INCLUDED_GR_UHD_RFNOC_BLOCK_GENERIC_H

#include <gnuradio/block.h>
#include <gnuradio/uhd/api.h>
#include <gnuradio/uhd/rfnoc_block.h>
#include <gnuradio/uhd/rfnoc_graph.h>

namespace gr {
namespace uhd {

/*! Generic RFNoC block holder
 *
 * This block can represent any RFNoC block
 *
 * \ingroup uhd_blk
 */
class GR_UHD_API rfnoc_block_generic : virtual public rfnoc_block
{
public:
    typedef std::shared_ptr<rfnoc_block_generic> sptr;

    /*!
     * \param graph Reference to the underlying rfnoc_graph object
     * \param block_args Arguments that get passed into the block
     * \param block_name Block name. This argument, along with \p device_select
     *                   and \p block_select, are used to identify which block
     *                   is instantiated.
     * \param device_select Optional: Device count.
     * \param block_select Optional: Block select.
     */
    static sptr make(rfnoc_graph::sptr graph,
                     const ::uhd::device_addr_t& block_args,
                     const std::string& block_name,
                     const int device_select = -1,
                     const int block_select = -1);
};

} // namespace uhd
} // namespace gr

#endif /* INCLUDED_GR_UHD_RFNOC_BLOCK_GENERIC_H */

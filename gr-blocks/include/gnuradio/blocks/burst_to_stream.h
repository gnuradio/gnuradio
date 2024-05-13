/* -*- c++ -*- */
/*
 * Copyright (C) 2023-2024 Daniel Estevez <daniel@destevez.net>
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: MIT
 *
 */


#ifndef INCLUDED_BLOCKS_BURST_TO_STREAM_H
#define INCLUDED_BLOCKS_BURST_TO_STREAM_H

#include <gnuradio/block.h>
#include <gnuradio/blocks/api.h>

namespace gr {
namespace blocks {

/*!
 * \brief Burst to Stream
 * \ingroup blocks
 *
 * Transforms a bursty tagged stream into a continuous stream by inserting zeros
 * in the output between input packets whenever no packets are available at the
 * input.
 */
class BLOCKS_API burst_to_stream : virtual public gr::block
{
public:
    typedef std::shared_ptr<burst_to_stream> sptr;

    /*!
     * \brief Creates a Burst to Stream block
     *
     * \param itemsize Size of the items in bytes.
     * \param len_tag_key Packet length key of the tagged stream.
     * \param propagate_tags Controls propagation of tags from input to output.
     */
    static sptr make(size_t itemsize,
                     const std::string& len_tag_key = "packet_len",
                     bool propagate_tags = false);
};

} // namespace blocks
} // namespace gr

#endif /* INCLUDED_BLOCKS_BURST_TO_STREAM_H */

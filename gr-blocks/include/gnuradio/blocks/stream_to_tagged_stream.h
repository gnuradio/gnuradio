/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_STREAM_TO_TAGGED_STREAM_H
#define INCLUDED_BLOCKS_STREAM_TO_TAGGED_STREAM_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief Converts a regular stream into a tagged stream.
 * \ingroup blocks
 *
 * All this block does is add length tags in regular intervals.
 * It can be used to connect a regular stream to a gr::tagged_stream_block.
 *
 * \b Careful: This block is meant to be connected directly to a tagged
 * stream block. If there are blocks between this block and a tagged
 * stream block, make sure they either don't change the rate, or modify
 * the tag value to make sure the length tags actually represent the
 * packet length.
 */
class BLOCKS_API stream_to_tagged_stream : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<stream_to_tagged_stream> sptr;

    /*!
     * \param itemsize Item size
     * \param vlen Vector length of the input items. Note that one vector is one item.
     * \param packet_len Number of items per tagged stream packet. One tag is written
     * every \p packet_len items. \param len_tag_key Key of the length tag.
     */
    static sptr make(size_t itemsize,
                     unsigned int vlen,
                     unsigned packet_len,
                     const std::string& len_tag_key);
    virtual void set_packet_len(unsigned packet_len) = 0;
    virtual void set_packet_len_pmt(unsigned packet_len) = 0;
};

} // namespace blocks
} // namespace gr

#endif /* INCLUDED_BLOCKS_STREAM_TO_TAGGED_STREAM_H */

/* -*- c++ -*- */
/*
 * Copyright 2010,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_ANNOTATOR_1TO1_H
#define INCLUDED_GR_ANNOTATOR_1TO1_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief 1-to-1 stream annotator testing block. FOR TESTING PURPOSES ONLY.
 * \ingroup debug_tools_blk
 *
 * \details
 * This block creates tags to be sent downstream every 10,000
 * items it sees. The tags contain the name and ID of the
 * instantiated block, use "seq" as a key, and have a counter that
 * increments by 1 for every tag produced that is used as the
 * tag's value. The tags are propagated using the 1-to-1 policy.
 *
 * It also stores a copy of all tags it sees flow past it. These
 * tags can be recalled externally with the data() member.
 *
 * Warning: This block is only meant for testing and showing how to use the
 * tags.
 */
class BLOCKS_API annotator_1to1 : virtual public sync_block
{
public:
    // gr::blocks::annotator_1to1::sptr
    typedef std::shared_ptr<annotator_1to1> sptr;

    static sptr make(int when, size_t sizeof_stream_item);

    virtual std::vector<tag_t> data() const = 0;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_ANNOTATOR_1TO1_H */

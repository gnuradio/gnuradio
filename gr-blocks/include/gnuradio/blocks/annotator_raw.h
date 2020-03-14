/* -*- c++ -*- */
/*
 * Copyright 2010,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_ANNOTATOR_RAW_H
#define INCLUDED_GR_ANNOTATOR_RAW_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>
#include <gnuradio/tags.h>

namespace gr {
namespace blocks {

/*!
 * \brief raw stream annotator testing block.
 * \ingroup debug_tools_blk
 *
 * \details
 * This block creates arbitrary tags to be sent downstream gnuradio/blocks
 * to be sent are set manually via accessor methods and are sent
 * only once.
 *
 * This block is intended for testing of tag related gnuradio/blocks
 */
class BLOCKS_API annotator_raw : virtual public sync_block
{
public:
    // gr::blocks::annotator_raw::sptr
    typedef std::shared_ptr<annotator_raw> sptr;

    static sptr make(size_t sizeof_stream_item);

    // insert a tag to be added
    virtual void add_tag(uint64_t offset, pmt::pmt_t key, pmt::pmt_t val) = 0;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_ANNOTATOR_RAW_H */

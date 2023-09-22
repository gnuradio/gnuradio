/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_NULL_SINK_H
#define INCLUDED_GR_NULL_SINK_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>
#include <cstddef> // size_t

namespace gr {
namespace blocks {

/*!
 * \brief Bit bucket. Use as a termination point when a sink is
 * required and we don't want to do anything real.
 * \ingroup misc_blk
 */
class BLOCKS_API null_sink : virtual public sync_block
{
public:
    // gr::blocks::null_sink::sptr
    typedef std::shared_ptr<null_sink> sptr;

    /*!
     * Build a null sink block.
     *
     * \param sizeof_stream_item size of the stream items in bytes.
     */
    static sptr make(size_t sizeof_stream_item);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_NULL_SINK_H */

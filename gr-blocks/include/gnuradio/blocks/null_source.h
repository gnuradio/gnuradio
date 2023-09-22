/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_NULL_SOURCE_H
#define INCLUDED_GR_NULL_SOURCE_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief A source of zeros used mainly for testing.
 * \ingroup misc_blk
 */
class BLOCKS_API null_source : virtual public sync_block
{
public:
    // gr::blocks::null_source::sptr
    typedef std::shared_ptr<null_source> sptr;

    /*!
     * Build a null source block.
     *
     * \param sizeof_stream_item size of the stream items in bytes.
     */
    static sptr make(size_t sizeof_stream_item);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_NULL_SOURCE_H */

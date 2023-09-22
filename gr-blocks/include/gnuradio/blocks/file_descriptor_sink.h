/* -*- c++ -*- */
/*
 * Copyright 2004,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_FILE_DESCRIPTOR_SINK_H
#define INCLUDED_GR_FILE_DESCRIPTOR_SINK_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief Write stream to file descriptor.
 * \ingroup file_operators_blk
 */
class BLOCKS_API file_descriptor_sink : virtual public sync_block
{
public:
    // gr::blocks::file_descriptor_sink::sptr
    typedef std::shared_ptr<file_descriptor_sink> sptr;

    /*!
     * Build a file descriptor sink block. The provided file descriptor will
     * be closed when the sink is destroyed.
     *
     * \param itemsize item size of the incoming data stream.
     * \param fd file descriptor (as an integer).
     */
    static sptr make(size_t itemsize, int fd);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_FILE_DESCRIPTOR_SINK_H */

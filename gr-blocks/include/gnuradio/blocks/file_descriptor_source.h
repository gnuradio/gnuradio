/* -*- c++ -*- */
/*
 * Copyright 2004,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_FILE_DESCRIPTOR_SOURCE_H
#define INCLUDED_GR_FILE_DESCRIPTOR_SOURCE_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>


namespace gr {
namespace blocks {

/*!
 * \brief Read stream from file descriptor.
 * \ingroup file_operators_blk
 */
class BLOCKS_API file_descriptor_source : virtual public sync_block
{
protected:
    virtual int read_items(char* buf, int nitems) = 0;
    virtual int handle_residue(char* buf, int nbytes_read) = 0;
    virtual void flush_residue() = 0;

public:
    // gr::blocks::file_descriptor_source::sptr
    typedef std::shared_ptr<file_descriptor_source> sptr;

    /*!
     * Build a file descriptor source block. The provided file descriptor will
     * be closed when the sink is destroyed.
     *
     * \param itemsize item size of the incoming data stream.
     * \param fd file descriptor (as an integer).
     * \param repeat repeat the data stream continuously.
     */
    static sptr make(size_t itemsize, int fd, bool repeat = false);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_FILE_DESCRIPTOR_SOURCE_H */

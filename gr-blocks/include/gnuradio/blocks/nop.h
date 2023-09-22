/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_NOP_H
#define INCLUDED_GR_NOP_H

#include <gnuradio/block.h>
#include <gnuradio/blocks/api.h>
#include <cstddef> // size_t

namespace gr {
namespace blocks {

/*!
 * \brief Does nothing. Used for testing only.
 * \ingroup misc_blk
 */
class BLOCKS_API nop : virtual public block
{
public:
    // gr::blocks::nop::sptr
    typedef std::shared_ptr<nop> sptr;

    /*!
     * Build a nop block.
     *
     * \param sizeof_stream_item size of the stream items in bytes.
     */
    static sptr make(size_t sizeof_stream_item);

    virtual int nmsgs_received() const = 0;

    virtual int ctrlport_test() const = 0;
    virtual void set_ctrlport_test(int x) = 0;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_NOP_H */

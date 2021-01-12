/* -*- c++ -*- */
/*
 * Copyright 2006,2009,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_COPY_H
#define INCLUDED_GR_COPY_H

#include <gnuradio/block.h>
#include <gnuradio/blocks/api.h>

namespace gr {
namespace blocks {

/*!
 * \brief output[i] = input[i]
 * \ingroup misc_blk
 *
 * \details
 * When enabled (default), this block copies its input to its
 * output. When disabled, this block drops its input on the floor.
 *
 * Message Ports:
 *
 * - en (input):
 *      Receives a PMT bool message to either enable to disable
 *      copy.
 */
class BLOCKS_API copy : virtual public block
{
public:
    // gr::blocks::copy::sptr
    typedef std::shared_ptr<copy> sptr;

    static sptr make(size_t itemsize);

    virtual void set_enabled(bool enable) = 0;
    virtual bool enabled() const = 0;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_COPY_H */

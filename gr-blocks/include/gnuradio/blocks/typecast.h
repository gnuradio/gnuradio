/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2013 Free Software Foundation, Inc.
 * Copyright 2025 Sam Lane
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_TYPECAST_H
#define INCLUDED_GR_TYPECAST_H

#include <gnuradio/block.h>
#include <gnuradio/blocks/api.h>
#include <cstddef> // size_t

namespace gr {
namespace blocks {
/*!
 * \brief Casts between types directly, c-style
 * \ingroup misc_blk
 */
class BLOCKS_API typecast : virtual public block
{
public:
    // gr::blocks::typecast::sptr
    typedef std::shared_ptr<typecast> sptr;

    /*!
     * Build a typecast block.
     *
     * \param sizeof_stream_in_item size of the input stream items in bytes.
     * \param sizeof_stream_out_item size of the output stream items in bytes.
     */
    static sptr make(size_t sizeof_in_stream_item, size_t sizeof_out_stream_item);
};
} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_TYPECAST_H */

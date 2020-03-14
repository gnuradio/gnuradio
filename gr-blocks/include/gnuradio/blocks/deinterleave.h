/* -*- c++ -*- */
/*
 * Copyright 2012,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_DEINTERLEAVE_H
#define INCLUDED_BLOCKS_DEINTERLEAVE_H

#include <gnuradio/block.h>
#include <gnuradio/blocks/api.h>

namespace gr {
namespace blocks {

/*!
 * \brief deinterleave an input block of samples into N outputs.
 * \ingroup stream_operators_blk
 *
 * \details
 * This block deinterleaves blocks of samples. For each output
 * connection, the input stream will be deinterleaved successively
 * to the output connections. By default, the block deinterleaves
 * a single input to each output unless blocksize is given in the
 * constructor.
 *
 * \code
 *   blocksize = 1
 *   connections = 2
 *   input = [a, b, c, d, e, f, g, h]
 *   output[0] = [a, c, e, g]
 *   output[1] = [b, d, f, h]
 * \endcode
 *
 * \code
 *   blocksize = 2
 *   connections = 2
 *   input = [a, b, c, d, e, f, g, h]
 *   output[0] = [a, b, e, f]
 *   output[1] = [c, d, g, h]
 * \endcode
 */
class BLOCKS_API deinterleave : virtual public block
{
public:
    // gr::blocks::deinterleave::sptr
    typedef std::shared_ptr<deinterleave> sptr;

    /*!
     * Make a deinterleave block.
     *
     * \param itemsize stream itemsize
     * \param blocksize size of block to deinterleave
     */
    static sptr make(size_t itemsize, unsigned int blocksize = 1);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_DEINTERLEAVE_H */

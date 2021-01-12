/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_MULTIPLY_CONJUGATE_CC_H
#define INCLUDED_GR_MULTIPLY_CONJUGATE_CC_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief Multiplies stream 0 by the complex conjugate of stream 1.
 * \ingroup math_operators_blk
 */
class BLOCKS_API multiply_conjugate_cc : virtual public sync_block
{
public:
    // gr::blocks::multiply_conjugate_cc::sptr
    typedef std::shared_ptr<multiply_conjugate_cc> sptr;

    /*!
     * \brief Multiplies a streams by the conjugate of a second stream
     * \param vlen Vector length
     * \ingroup math_blk
     */
    static sptr make(size_t vlen = 1);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_MULTIPLY_CONJUGATE_CC_H */

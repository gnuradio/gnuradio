/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_CONJUGATE_CC_H
#define INCLUDED_BLOCKS_CONJUGATE_CC_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief output = complex conjugate of input
 * \ingroup math_operators_blk
 */
class BLOCKS_API conjugate_cc : virtual public sync_block
{
public:
    // gr::blocks::conjugate_cc_ff::sptr
    typedef std::shared_ptr<conjugate_cc> sptr;

    static sptr make();
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_CONJUGATE_CC_H */

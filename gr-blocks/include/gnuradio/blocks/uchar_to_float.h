/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_UCHAR_TO_FLOAT_H
#define INCLUDED_BLOCKS_UCHAR_TO_FLOAT_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief Convert stream of unsigned chars to a stream of floats
 * \ingroup type_converters_blk
 */
class BLOCKS_API uchar_to_float : virtual public sync_block
{
public:
    // gr::blocks::uchar_to_float_ff::sptr
    typedef std::shared_ptr<uchar_to_float> sptr;

    /*!
     * Build a uchar to float block.
     */
    static sptr make();
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_UCHAR_TO_FLOAT_H */

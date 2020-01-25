/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_FLOAT_TO_UCHAR_H
#define INCLUDED_BLOCKS_FLOAT_TO_UCHAR_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief Convert stream of floats to a stream of unsigned chars
 * \ingroup type_converters_blk
 */
class BLOCKS_API float_to_uchar : virtual public sync_block
{
public:
    // gr::blocks::float_to_uchar_ff::sptr
    typedef boost::shared_ptr<float_to_uchar> sptr;

    /*!
     * Build a float to uchar block.
     */
    static sptr make();
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_FLOAT_TO_UCHAR_H */

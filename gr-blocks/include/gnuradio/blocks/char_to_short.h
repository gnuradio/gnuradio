/* -*- c++ -*- */
/*
 * Copyright 2005,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_CHAR_TO_SHORT_H
#define INCLUDED_BLOCKS_CHAR_TO_SHORT_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief Convert stream of chars to a stream of shorts.
 * \ingroup type_converters_blk
 *
 * \details
 * Converts \p vlen length vectors of input char samples to shorts,
 * multiplying each element by 256:
 *
 * \li output[0][m:m+vlen] = 256 * static_cast<short>(input[0][m:m+vlen])
 */
class BLOCKS_API char_to_short : virtual public sync_block
{
public:
    // gr::blocks::char_to_short_ff::sptr
    typedef std::shared_ptr<char_to_short> sptr;

    static sptr make(size_t vlen = 1);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_CHAR_TO_SHORT_H */

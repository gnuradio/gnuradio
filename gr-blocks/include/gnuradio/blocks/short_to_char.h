/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_SHORT_TO_CHAR_H
#define INCLUDED_BLOCKS_SHORT_TO_CHAR_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief Convert stream of shorts to a stream of chars.
 *
 * This block strips off the least significant byte from the
 * short value.
 *
 * [0x00ff, 0x0ff0, 0xff00] => [0x00, 0x0f, 0xff]
 *
 * \ingroup type_converters_blk
 *
 * \details
 * Converts \p vlen length vectors of input short samples to chars,
 * dividing each element by 256.
 */
class BLOCKS_API short_to_char : virtual public sync_block
{
public:
    // gr::blocks::short_to_char_ff::sptr
    typedef std::shared_ptr<short_to_char> sptr;

    /*!
     * Build a short to char block.
     *
     * \param vlen vector length of data streams.
     */
    static sptr make(size_t vlen = 1);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_SHORT_TO_CHAR_H */

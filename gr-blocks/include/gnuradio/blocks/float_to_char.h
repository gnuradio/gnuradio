/* -*- c++ -*- */
/*
 * Copyright 2005,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_FLOAT_TO_CHAR_H
#define INCLUDED_BLOCKS_FLOAT_TO_CHAR_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief Convert stream of floats to a stream of char
 * \ingroup type_converters_blk
 */
class BLOCKS_API float_to_char : virtual public sync_block
{
public:
    // gr::blocks::float_to_char_ff::sptr
    typedef std::shared_ptr<float_to_char> sptr;

    /*!
     * Build a float to char block.
     *
     * \param vlen vector length of data streams.
     * \param scale a scalar multiplier to change the output signal scale.
     */
    static sptr make(size_t vlen = 1, float scale = 1.0);

    /*!
     * Get the scalar multiplier value.
     */
    virtual float scale() const = 0;

    /*!
     * Set the scalar multiplier value.
     */
    virtual void set_scale(float scale) = 0;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_FLOAT_TO_CHAR_H */

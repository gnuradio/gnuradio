/* -*- c++ -*- */
/*
 * Copyright 2005,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_CHAR_TO_FLOAT_H
#define INCLUDED_BLOCKS_CHAR_TO_FLOAT_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief Convert stream of chars to a stream of float.
 * \ingroup type_converters_blk
 *
 * \details
 * Converts \p vlen length vectors of input char samples to floats
 * and applies a scaling factor of \p scale:
 *
 * \li output[0][m:m+vlen] = static_cast<float>(input[0][m:m+vlen]) / scale
 */
class BLOCKS_API char_to_float : virtual public sync_block
{
public:
    // gr::blocks::char_to_float_ff::sptr
    typedef std::shared_ptr<char_to_float> sptr;

    /*!
     * Build a chars to float stream converter block.
     *
     * \param vlen vector length of data streams.
     * \param scale a scalar divider to change the output signal scale.
     */
    static sptr make(size_t vlen = 1, float scale = 1.0);

    /*!
     * Get the scalar divider value.
     */
    virtual float scale() const = 0;

    /*!
     * Set the scalar divider value.
     */
    virtual void set_scale(float scale) = 0;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_CHAR_TO_FLOAT_H */

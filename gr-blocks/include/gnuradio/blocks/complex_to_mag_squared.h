/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_COMPLEX_TO_MAG_SQUARED_H
#define INCLUDED_BLOCKS_COMPLEX_TO_MAG_SQUARED_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief complex in, magnitude squared out (float)
 * \ingroup type_converters_blk
 *
 * \details
 * Calculates the magnitude squared of the complex samples:
 *
 * \li output[0][m] = |input[0][m]|^2
 *
 * Or:
 * \li output[0][m] = Re{input[0][m]}^2 + Im{input[0][m]}^2
 *
 * The input stream can be a vector of length \p vlen, and for
 * each vector, each item is converted using the above
 * function. So above, m is from 0 to noutput_items*vlen for each
 * call to work.
 */
class BLOCKS_API complex_to_mag_squared : virtual public sync_block
{
public:
    // gr::blocks::complex_to_mag_squared_ff::sptr
    typedef std::shared_ptr<complex_to_mag_squared> sptr;

    /*!
     * Build a complex to magnitude squared block.
     *
     * \param vlen vector len (default 1)
     */
    static sptr make(size_t vlen = 1);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_COMPLEX_TO_MAG_SQUARED_H */

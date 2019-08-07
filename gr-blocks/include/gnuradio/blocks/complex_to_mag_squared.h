/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
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
    typedef boost::shared_ptr<complex_to_mag_squared> sptr;

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

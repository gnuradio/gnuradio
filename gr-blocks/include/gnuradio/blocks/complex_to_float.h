/* -*- c++ -*- */
/*
 * Copyright 2005,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_COMPLEX_TO_FLOAT_H
#define INCLUDED_BLOCKS_COMPLEX_TO_FLOAT_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief Convert a stream of gr_complex to 1 or 2 streams of float.
 * \ingroup type_converters_blk
 *
 * \details
 * If a single output stream is attached, this will output the
 * real part of the input complex samples. If a second output
 * stream is connected, output[0] is the real part and output[1]
 * is the imaginary part.
 */
class BLOCKS_API complex_to_float : virtual public sync_block
{
public:
    // gr::blocks::complex_to_float_ff::sptr
    typedef std::shared_ptr<complex_to_float> sptr;

    /*!
     * Build a complex to float block.
     *
     * \param vlen vector len (default 1)
     */
    static sptr make(size_t vlen = 1);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_COMPLEX_TO_FLOAT_H */

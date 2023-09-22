/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_COMPLEX_TO_IMAG_H
#define INCLUDED_BLOCKS_COMPLEX_TO_IMAG_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief Produces the imaginary part (as a float) of a complex stream.
 * \ingroup type_converters_blk
 */
class BLOCKS_API complex_to_imag : virtual public sync_block
{
public:
    // gr::blocks::complex_to_imag_ff::sptr
    typedef std::shared_ptr<complex_to_imag> sptr;

    /*!
     * Build a complex to imaginary part block.
     *
     * \param vlen vector len (default 1)
     */
    static sptr make(size_t vlen = 1);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_COMPLEX_TO_IMAG_H */

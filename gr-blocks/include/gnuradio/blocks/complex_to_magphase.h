/* -*- c++ -*- */
/*
 * Copyright 2005,2012,2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_COMPLEX_TO_MAGPHASE_H
#define INCLUDED_BLOCKS_COMPLEX_TO_MAGPHASE_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief Convert a stream of gr_complex to magnitude and phase (both floats)
 * \ingroup type_converters_blk
 *
 * \details
 * output[0] is the magnitude
 * output[1] is the phase (in radians)
 */
class BLOCKS_API complex_to_magphase : virtual public sync_block
{
public:
    // gr::blocks::complex_to_magphase_ff::sptr
    typedef std::shared_ptr<complex_to_magphase> sptr;

    /*!
     * Build a complex to magnitude and phase block.
     *
     * \param vlen vector len (default 1)
     */
    static sptr make(size_t vlen = 1);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_COMPLEX_TO_MAGPHASE_H */

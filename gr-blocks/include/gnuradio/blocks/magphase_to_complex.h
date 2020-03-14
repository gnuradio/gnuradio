/* -*- c++ -*- */
/*
 * Copyright 2012,2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_MAGPHASE_TO_COMPLEX_H
#define INCLUDED_BLOCKS_MAGPHASE_TO_COMPLEX_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief two floats in (mag and phase), complex out
 * \ingroup type_converters_blk
 */
class BLOCKS_API magphase_to_complex : virtual public sync_block
{
public:
    // gr::blocks::magphase_to_complex_ff::sptr
    typedef std::shared_ptr<magphase_to_complex> sptr;

    /*!
     * Build a mag and phase to complex block.
     *
     * \param vlen vector len (default 1)
     */
    static sptr make(size_t vlen = 1);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_MAGPHASE_TO_COMPLEX_H */

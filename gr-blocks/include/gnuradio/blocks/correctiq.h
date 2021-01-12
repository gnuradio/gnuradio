/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_CORRECTIQ_CORRECTIQ_H
#define INCLUDED_CORRECTIQ_CORRECTIQ_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief This block to removes that center frequency IQ DC spike with an IIR filter.
 * \ingroup iq_correction
 *
 */
class BLOCKS_API correctiq : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<correctiq> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of correctiq::correctiq.
     *
     * To avoid accidental use of raw pointers, correctiq::correctiq's
     * constructor is in a private implementation
     * class. correctiq::correctiq::make is the public interface for
     * creating new instances.
     */
    static sptr make();
};

} // namespace blocks
} // namespace gr

#endif /* INCLUDED_CORRECTIQ_CORRECTIQ_H */

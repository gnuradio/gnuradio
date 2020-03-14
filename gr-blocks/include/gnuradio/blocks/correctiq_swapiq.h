/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_CORRECTIQ_SWAPIQ_H
#define INCLUDED_CORRECTIQ_SWAPIQ_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief This block will transpose the I and Q channels (Swap IQ) to
 * \p correct for spectrally inverted inputs.
 * \ingroup iq_correction
 *
 */
class BLOCKS_API swap_iq : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<swap_iq> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of correctiq::swap_iq.
     *
     * To avoid accidental use of raw pointers, correctiq::swap_iq's
     * constructor is in a private implementation
     * class. correctiq::swap_iq::make is the public interface for
     * creating new instances.
     */
    static sptr make(int datatype, int datasize);
};

} // namespace blocks
} // namespace gr

#endif /* INCLUDED_CORRECTIQ_SWAPIQ_H */

/* -*- c++ -*- */
/*
 * Copyright 2007,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_REGENERATE_BB_H
#define INCLUDED_GR_REGENERATE_BB_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief Detect the peak of a signal and repeat every period samples
 * \ingroup stream_operators_blk
 *
 * \details
 * If a peak is detected, this block outputs a 1 repeated every
 * period samples until reset by detection of another 1 on the
 * input or stopped after max_regen regenerations have occurred.
 *
 * Note that if max_regen=(-1)/ULONG_MAX then the regeneration
 * will run forever.
 */
class BLOCKS_API regenerate_bb : virtual public sync_block
{
public:
    // gr::blocks::regenerate_bb::sptr
    typedef std::shared_ptr<regenerate_bb> sptr;

    /*!
     * \brief Make a regenerate block
     * \param period The number of samples between regenerations
     * \param max_regen The maximum number of regenerations to
     * perform; if set to ULONG_MAX, it will regenerate
     * continuously.
     */
    static sptr make(int period, unsigned int max_regen = 500);

    /*! \brief Reset the maximum regeneration count; this will reset
        the current regen.
     */
    virtual void set_max_regen(unsigned int regen) = 0;

    /*! \brief Reset the period of regenerations; this will reset
        the current regen.
    */
    virtual void set_period(int period) = 0;

    /*! \brief return the maximum regeneration count.
     */
    virtual unsigned int max_regen() const = 0;

    /*! \brief return the regeneration period.
     */
    virtual int period() const = 0;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_REGENERATE_BB_H */

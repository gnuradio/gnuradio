/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_TEST_TAG_VARIABLE_RATE_FF_H
#define INCLUDED_GR_TEST_TAG_VARIABLE_RATE_FF_H

#include <gnuradio/block.h>
#include <gnuradio/blocks/api.h>

namespace gr {
namespace blocks {

/*!
 * \brief Used for testing tag propagation.
 * \ingroup measurement_tools_blk
 * \ingroup stream_tag_tools_blk
 * \ingroup debug_tools_blk
 *
 * This block resamples the stream by a factor that starts at 0.5
 * but varies around by some random walk. The relative rate of the
 * block is initialized to 0.5 and the random number generator is
 * seeded based on the time. For each input, if the random number
 * is >= 0.5, the relative rate is increased by \p update_step;
 * otherwise, it is decreased by \p update_step.
 *
 * The rate of the change of the resampling factor is set by the
 * flag \p update_once. If this is set to true, then the rate is
 * only updated once per work function. If it is set to false
 * (default), then the rate is changed with every input sample.
 *
 * The block's ctor sets the propagation policy to TPP_DONT to
 * stop tags from automatically propagating. Instead, we handle
 * the tag propagation ourselves from within the work
 * function. Because the relative_rate changes so fast, the tag
 * placement cannot be based on a single factor after the call to
 * work and must be handled when the samples are and based on the
 * current resampling rate.
 */
class BLOCKS_API test_tag_variable_rate_ff : virtual public block
{
public:
    // gr::blocks::test_tag_variable_rate_ff::sptr
    typedef std::shared_ptr<test_tag_variable_rate_ff> sptr;

    /*!
     * Build a test_tag_variable_rate_ff block.
     *
     * \param update_once Update the resampling rate once per call
     *        to work (default=false).
     * \param update_step How much to adjust the resampling rate by
     *        when the update occurs (default=0.001).
     */
    static sptr make(bool update_once = false, double update_step = 0.001);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_TEST_TAG_VARIABLE_RATE_FF_H */

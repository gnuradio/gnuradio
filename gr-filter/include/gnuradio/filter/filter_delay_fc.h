/* -*- c++ -*- */
/*
 * Copyright 2004,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FILTER_FILTER_DELAY_FC_H
#define INCLUDED_FILTER_FILTER_DELAY_FC_H

#include <gnuradio/filter/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace filter {

/*!
 * \brief Filter-Delay Combination Block.
 * \ingroup filter_blk
 *
 * \details
 * The block takes one or two float stream and outputs a complex
 * stream.
 *
 * If only one float stream is input, the real output is a delayed
 * version of this input and the imaginary output is the filtered
 * output.
 *
 * If two floats are connected to the input, then the real output
 * is the delayed version of the first input, and the imaginary
 * output is the filtered output.
 *
 * The delay in the real path accounts for the group delay
 * introduced by the filter in the imaginary path. The filter taps
 * needs to be calculated before initializing this block.
 */
class FILTER_API filter_delay_fc : virtual public sync_block
{
public:
    // gr::filter::filter_delay_fc::sptr
    typedef boost::shared_ptr<filter_delay_fc> sptr;

    /*!
     * Build a filter with delay block.
     */
    static sptr make(const std::vector<float>& taps);
};

} /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_FILTER_FILTER_DELAY_FC_H */

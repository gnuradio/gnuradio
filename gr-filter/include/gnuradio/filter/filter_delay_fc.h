/* -*- c++ -*- */
/*
 * Copyright 2004,2012 Free Software Foundation, Inc.
 * Copyright 2021 Marcus Müller
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
 * \brief Filter-Delay Combination Block
 * \ingroup filter_blk
 *
 * \details
 * The purpose of this block is to compensate the delay that a linear-phase filter
 * introduces.
 *
 * It does that by introducing a delay of half the length of the FIR tap vector on
 * passed-through samples.
 *
 * The block takes one or two float stream and outputs a stream composed of pairs of
 * floats. (In GNU Radio, pairs of floats are identical to complex numbers, where the real
 * and imaginary parts are the first and second element, respectively.)
 *
 * If only one float stream is input, the first element of each output item is a delayed
 * version of this input and the second element is the filtered output.
 *
 * If two floats are connected to the input, then the first element of each output item is
 * the delayed version of the first input, and the second element is the filtered second
 * input.
 *
 * The delay in the first elements of the output accounts for the group delay introduced
 * by the filter in the second elements path under the assumption of linear-phase
 * filtering. The filter taps need to be calculated before initializing this block.
 */
class FILTER_API filter_delay_fc : virtual public sync_block
{
public:
    typedef std::shared_ptr<filter_delay_fc> sptr;

    /*!
     * Build a filter with delay block.
     *
     * \param taps the vector of real-valued tags. Half of the length of this vector is
     * the delay introduced by this block.
     */
    static sptr make(const std::vector<float>& taps);
};

} /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_FILTER_FILTER_DELAY_FC_H */

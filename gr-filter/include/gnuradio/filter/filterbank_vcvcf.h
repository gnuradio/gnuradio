/* -*- c++ -*- */
/*
 * Copyright 2009,2010,2012,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef INCLUDED_FILTER_FILTERBANK_CHANNELIZER_VCVCF_H
#define INCLUDED_FILTER_FILTERBANK_CHANNELIZER_VCVCF_H

#include <gnuradio/block.h>
#include <gnuradio/filter/api.h>

namespace gr {
namespace filter {

/*!
 * \brief Filterbank with vector of gr_complex input,
 * vector of gr_complex output and float taps
 * \ingroup filter_blk
 *
 * \details
 * This block takes in complex vectors and outputs complex vectors of the same
 * size.  Vectors of length N, rather than N normal streams are used to reduce
 * overhead.
 */
class FILTER_API filterbank_vcvcf : virtual public block
{
public:
    typedef std::shared_ptr<filterbank_vcvcf> sptr;

    /*!
     * Build the filterbank.
     * \param taps (vector of vector of floats/list of list of floats)
     *             Used to populate the filters.
     */
    static sptr make(const std::vector<std::vector<float>>& taps);

    /*!
     * Resets the filterbank's filter taps with the new prototype filter
     * \param taps (vector/list of floats) The prototype filter to populate the
     * filterbank.
     */
    virtual void set_taps(const std::vector<std::vector<float>>& taps) = 0;

    /*!
     * Print all of the filterbank taps to screen.
     */
    virtual void print_taps() = 0;

    /*!
     * Return a vector<vector<>> of the filterbank taps
     */
    virtual std::vector<std::vector<float>> taps() const = 0;
};

} /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_FILTER_PFB_CHANNELIZER_VCVCF_H */

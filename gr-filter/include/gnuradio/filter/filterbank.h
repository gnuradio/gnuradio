/* -*- c++ -*- */
/*
 * Copyright 2012, 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef INCLUDED_FILTER_FILTERBANK_H
#define INCLUDED_FILTER_FILTERBANK_H

#include <gnuradio/filter/api.h>
#include <gnuradio/filter/fir_filter.h>

namespace gr {
namespace filter {
namespace kernel {

/*!
 * \brief A filter bank with generic taps.
 *
 * This block takes in a vector of N complex inputs, passes
 * them through N FIR filters, and outputs a vector of N complex
 * outputs.
 *
 * The only advantage of using this block over N individual
 * FIR filter blocks is that it places less of a load on the
 * scheduler.
 *
 * The number of filters cannot be changed dynamically, however
 * filters can be deactivated (i.e. no processing is done for
 * them) by passing a vector of filter taps containing all zeros
 * to them.  In this case their entry in the output vector is a
 * zero.
 *
 */

class FILTER_API filterbank
{
protected:
    std::vector<std::vector<float>> d_taps;
    unsigned int d_nfilts;
    unsigned int d_ntaps;
    std::vector<kernel::fir_filter_ccf> d_fir_filters;
    std::vector<bool> d_active;
    unsigned int d_taps_per_filter;

public:
    /*!
     * Build the filterbank.
     *
     * \param taps (vector of vector of floats / list of list of floats)
     *             Populates the filters.
     */
    filterbank(const std::vector<std::vector<float>>& taps);
    filterbank(filterbank&&) = default;
    virtual ~filterbank() = default;

    /*!
     * Update the filterbank's filter taps.
     *
     * \param taps (vector of vector of floats / list of list of floats)
     *             The prototype filter to populate the filterbank.
     */
    virtual void set_taps(const std::vector<std::vector<float>>& taps);

    /*!
     * Print all of the filterbank taps to screen.
     */
    void print_taps();

    /*!
     * Return a vector<vector<>> of the filterbank taps
     */
    std::vector<std::vector<float>> taps() const;
};

} /* namespace kernel */
} /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_FILTER_FILTERBANK_H */

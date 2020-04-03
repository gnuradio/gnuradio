/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef INCLUDED_PFB_ARB_RESAMPLER_CCC_H
#define INCLUDED_PFB_ARB_RESAMPLER_CCC_H

#include <gnuradio/block.h>
#include <gnuradio/filter/api.h>

namespace gr {
namespace filter {

/*!
 * \brief Polyphase filterbank arbitrary resampler with
 *        gr_complex input, gr_complex output and gr_complex taps
 * \ingroup resamplers_blk
 *
 * \details

 * This block takes in a signal stream and calls
 * gr::filter::kernel::pfb_arb_resampler_ccc to perform
 * arbitrary resampling on the stream.
 *
 * Output sampling rate is \p rate * input rate.
 */
class FILTER_API pfb_arb_resampler_ccc : virtual public block
{
public:
    // gr::filter::pfb_arb_resampler_ccc::sptr
    typedef std::shared_ptr<pfb_arb_resampler_ccc> sptr;

    /*!
     * Build the polyphase filterbank arbitrary resampler.
     * \param rate  (float) Specifies the resampling rate to use
     * \param taps  (vector/list of complex) The prototype filter to populate the
     * filterbank. The taps should be generated at the filter_size sampling rate. \param
     * filter_size (unsigned int) The number of filters in the filter bank. This is
     * directly related to quantization noise introduced during the resampling. Defaults
     * to 32 filters.
     */
    static sptr
    make(float rate, const std::vector<gr_complex>& taps, unsigned int filter_size = 32);

    /*!
     * Resets the filterbank's filter taps with the new prototype filter
     * \param taps    (vector/list of complex) The prototype filter to populate the
     * filterbank.
     */
    virtual void set_taps(const std::vector<gr_complex>& taps) = 0;

    /*!
     * Return a vector<vector<>> of the filterbank taps
     */
    virtual std::vector<std::vector<gr_complex>> taps() const = 0;

    /*!
     * Print all of the filterbank taps to screen.
     */
    virtual void print_taps() = 0;

    /*!
     * Sets the resampling rate of the block.
     */
    virtual void set_rate(float rate) = 0;

    /*!
     * Sets the current phase offset in radians (0 to 2pi).
     */
    virtual void set_phase(float ph) = 0;

    /*!
     * Gets the current phase of the resampler in radians (2 to 2pi).
     */
    virtual float phase() const = 0;

    /*!
     * Gets the number of taps per filter.
     */
    virtual unsigned int taps_per_filter() const = 0;

    /*!
     * Gets the interpolation rate of the filter.
     */
    virtual unsigned int interpolation_rate() const = 0;

    /*!
     * Gets the decimation rate of the filter.
     */
    virtual unsigned int decimation_rate() const = 0;

    /*!
     * Gets the fractional rate of the filter.
     */
    virtual float fractional_rate() const = 0;

    /*!
     * Get the group delay of the filter.
     */
    virtual int group_delay() const = 0;

    /*!
     * Calculates the phase offset expected by a sine wave of
     * frequency \p freq and sampling rate \p fs (assuming input
     * sine wave has 0 degree phase).
     */
    virtual float phase_offset(float freq, float fs) = 0;
};

} /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_PFB_ARB_RESAMPLER_CCC_H */

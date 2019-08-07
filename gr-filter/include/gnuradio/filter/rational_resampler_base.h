/* -*- c++ -*- */
/*
 * Copyright 2005,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef RATIONAL_RESAMPLER_BASE_H
#define RATIONAL_RESAMPLER_BASE_H

#include <gnuradio/block.h>
#include <gnuradio/filter/api.h>


namespace gr {
namespace filter {

/*!
 * \brief Rational Resampling Polyphase FIR filter with IN_T
 * input, OUT_T output and TAP_T taps.
 * \ingroup resamplers_blk
 *
 * Make a rational resampling FIR filter. If the input signal is
 * at rate fs, then the output signal will be at a rate of \p
 * interpolation * fs / \p decimation.
 *
 * The interpolation and decimation rates should be kept as
 * small as possible, and generally should be relatively prime
 * to help reduce complexity in memory and computation.
 *
 * The set of \p taps supplied to this filterbank should be
 * designed around the resampling amount and must avoid aliasing
 * (when interpolation/decimation < 1) and images (when
 * interpolation/decimation > 1).
 *
 * As with any filter, the behavior of the filter taps (or
 * coefficients) is determined by the highest sampling rate that
 * the filter will ever see. In the case of a resampler that
 * increases the sampling rate, the highest sampling rate observed
 * is \p interpolation since in the filterbank, the number of
 * filter arms is equal to \p interpolation. When the resampler
 * decreases the sampling rate (decimation > interpolation), then
 * the highest rate is the input sample rate of the original
 * signal. We must create a filter based around this value to
 * reduce any aliasing that may occur from out-of-band signals.
 *
 * Another way to think about how to create the filter taps is
 * that the filter is effectively applied after interpolation and
 * before decimation. And yet another way to think of it is that
 * the taps should be a LPF that is at least as narrow as the
 * narrower of the required anti-image postfilter or anti-alias
 * prefilter.
 */
template <class IN_T, class OUT_T, class TAP_T>
class FILTER_API rational_resampler_base : virtual public block
{
public:
    typedef boost::shared_ptr<rational_resampler_base<IN_T, OUT_T, TAP_T>> sptr;

    /*!
     * Make a rational resampling FIR filter.
     *
     * \param interpolation The integer interpolation rate of the filter
     * \param decimation The integer decimation rate of the filter
     * \param taps The filter taps to control images and aliases
     */
    static sptr
    make(unsigned interpolation, unsigned decimation, const std::vector<TAP_T>& taps);

    virtual unsigned interpolation() const = 0;
    virtual unsigned decimation() const = 0;

    virtual void set_taps(const std::vector<TAP_T>& taps) = 0;
    virtual std::vector<TAP_T> taps() const = 0;
};
typedef rational_resampler_base<gr_complex, gr_complex, gr_complex>
    rational_resampler_base_ccc;
typedef rational_resampler_base<gr_complex, gr_complex, float>
    rational_resampler_base_ccf;
typedef rational_resampler_base<float, gr_complex, gr_complex>
    rational_resampler_base_fcc;
typedef rational_resampler_base<float, float, float> rational_resampler_base_fff;
typedef rational_resampler_base<float, std::int16_t, float> rational_resampler_base_fsf;
typedef rational_resampler_base<std::int16_t, gr_complex, gr_complex>
    rational_resampler_base_scc;

} /* namespace filter */
} /* namespace gr */

#endif /* RATIONAL_RESAMPLER_BASE_H */

/* -*- c++ -*- */
/*
 * Copyright 2005,2012 Free Software Foundation, Inc.
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

/* @WARNING@ */

#ifndef @GUARD_NAME@
#define	@GUARD_NAME@

#include <gnuradio/filter/api.h>
#include <gnuradio/block.h>


namespace gr {
  namespace filter {

    /*!
     * \brief Rational Resampling Polyphase FIR filter with @I_TYPE@
     * input, @O_TYPE@ output and @TAP_TYPE@ taps.
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
    class FILTER_API @NAME@ : virtual public block
    {
    public:
      // gr::filter::@BASE_NAME@::sptr
      typedef boost::shared_ptr<@BASE_NAME@> sptr;

      /*!
       * Make a rational resampling FIR filter.
       *
       * \param interpolation The integer interpolation rate of the filter
       * \param decimation The integer decimation rate of the filter
       * \param taps The filter taps to control images and aliases
       */
      static sptr make(unsigned interpolation,
                       unsigned decimation,
                       const std::vector<@TAP_TYPE@> &taps);

      virtual unsigned interpolation() const = 0;
      virtual unsigned decimation() const = 0;

      virtual void set_taps(const std::vector<@TAP_TYPE@> &taps) = 0;
      virtual std::vector<@TAP_TYPE@> taps() const = 0;
    };

  } /* namespace filter */
} /* namespace gr */

#endif /* @GUARD_NAME@ */

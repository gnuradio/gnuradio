/* -*- c++ -*- */
/*
 * Copyright 2009-2012 Free Software Foundation, Inc.
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


#ifndef INCLUDED_PFB_ARB_RESAMPLER_FFF_H
#define	INCLUDED_PFB_ARB_RESAMPLER_FFF_H

#include <gnuradio/filter/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace filter {

    /*!
     * \brief Polyphase filterbank arbitrary resampler with
     *        float input, float output and float taps
     * \ingroup resamplers_blk
     *
     * \details
     * This block takes in a signal stream and performs arbitrary
     * resampling. The resampling rate can be any real number
     * <EM>r</EM>. The resampling is done by constructing <EM>N</EM>
     * filters where <EM>N</EM> is the interpolation rate.  We then
     * calculate <EM>D</EM> where <EM>D = floor(N/r)</EM>.
     *
     * Using <EM>N</EM> and <EM>D</EM>, we can perform rational
     * resampling where <EM>N/D</EM> is a rational number close to the
     * input rate <EM>r</EM> where we have <EM>N</EM> filters and we
     * cycle through them as a polyphase filterbank with a stride of
     * <EM>D</EM> so that <EM>i+1 = (i + D) % N</EM>.
     *
     * To get the arbitrary rate, we want to interpolate between two
     * points. For each value out, we take an output from the current
     * filter, <EM>i</EM>, and the next filter <EM>i+1</EM> and then
     * linearly interpolate between the two based on the real
     * resampling rate we want.
     *
     * The linear interpolation only provides us with an approximation
     * to the real sampling rate specified. The error is a
     * quantization error between the two filters we used as our
     * interpolation points.  To this end, the number of filters,
     * <EM>N</EM>, used determines the quantization error; the larger
     * <EM>N</EM>, the smaller the noise. You can design for a
     * specified noise floor by setting the filter size (parameters
     * <EM>filter_size</EM>). The size defaults to 32 filters, which
     * is about as good as most implementations need.
     *
     * The trick with designing this filter is in how to specify the
     * taps of the prototype filter. Like the PFB interpolator, the
     * taps are specified using the interpolated filter rate. In this
     * case, that rate is the input sample rate multiplied by the
     * number of filters in the filterbank, which is also the
     * interpolation rate. All other values should be relative to this
     * rate.
     *
     * For example, for a 32-filter arbitrary resampler and using the
     * GNU Radio's firdes utility to build the filter, we build a
     * low-pass filter with a sampling rate of <EM>fs</EM>, a 3-dB
     * bandwidth of <EM>BW</EM> and a transition bandwidth of
     * <EM>TB</EM>. We can also specify the out-of-band attenuation to
     * use, <EM>ATT</EM>, and the filter window function (a
     * Blackman-harris window in this case). The first input is the
     * gain of the filter, which we specify here as the interpolation
     * rate (<EM>32</EM>).
     *
     *   <B><EM>self._taps = filter.firdes.low_pass_2(32, 32*fs, BW, TB,
     *      attenuation_dB=ATT, window=filter.firdes.WIN_BLACKMAN_hARRIS)</EM></B>
     *
     * The theory behind this block can be found in Chapter 7.5 of the
     * following book.
     *
     *   <B><EM>f. harris, "Multirate Signal Processing for Communication
     *      Systems", Upper Saddle River, NJ: Prentice Hall, Inc. 2004.</EM></B>
     */

    class FILTER_API pfb_arb_resampler_fff : virtual public block
    {
    public:
      // gr::filter::pfb_arb_resampler_fff::sptr
      typedef boost::shared_ptr<pfb_arb_resampler_fff> sptr;

      /*!
       * Build the polyphase filterbank arbitray resampler.
       * \param rate  (float) Specifies the resampling rate to use
       * \param taps  (vector/list of floats) The prototype filter to populate the filterbank. The taps
       *                                      should be generated at the filter_size sampling rate.
       * \param filter_size (unsigned int) The number of filters in the filter bank. This is directly
       *                                   related to quantization noise introduced during the resampling.
       *			           Defaults to 32 filters.
       */
      static sptr make(float rate,
                       const std::vector<float> &taps,
                       unsigned int filter_size=32);

      /*!
       * Resets the filterbank's filter taps with the new prototype filter
       * \param taps    (vector/list of floats) The prototype filter to populate the filterbank.
       */
      virtual void set_taps(const std::vector<float> &taps) = 0;

      /*!
       * Return a vector<vector<>> of the filterbank taps
       */
      virtual std::vector<std::vector<float> > taps() const = 0;

      /*!
       * Print all of the filterbank taps to screen.
       */
      virtual void print_taps() = 0;

      /*!
       * Sets the resampling rate of the block.
       */
      virtual void set_rate (float rate) = 0;

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
      virtual unsigned int decimation_rate() const =0;
      
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

#endif /* INCLUDED_PFB_ARB_RESAMPLER_FFF_H */

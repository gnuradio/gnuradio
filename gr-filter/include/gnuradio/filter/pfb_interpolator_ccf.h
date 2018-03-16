/* -*- c++ -*- */
/*
 * Copyright 2009,2012 Free Software Foundation, Inc.
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


#ifndef INCLUDED_PFB_INTERPOLATOR_CCF_H
#define	INCLUDED_PFB_INTERPOLATOR_CCF_H

#include <gnuradio/filter/api.h>
#include <gnuradio/sync_interpolator.h>

namespace gr {
  namespace filter {

    /*!
     * \brief Polyphase filterbank interpolator with gr_complex input,
     * gr_complex output and float taps
     * \ingroup channelizers_blk
     *
     * \details
     * This block takes in a signal stream and performs integer up-
     * sampling (interpolation) with a polyphase filterbank. The first
     * input is the integer specifying how much to interpolate by. The
     * second input is a vector (Python list) of floating-point taps
     * of the prototype filter.
     *
     * The filter's taps should be based on the interpolation rate
     * specified. That is, the bandwidth specified is relative to the
     * bandwidth after interpolation.
     *
     * For example, using the GNU Radio's firdes utility to building
     * filters, we build a low-pass filter with a sampling rate of
     * <EM>fs</EM>, a 3-dB bandwidth of <EM>BW</EM> and a transition
     * bandwidth of <EM>TB</EM>. We can also specify the out-of-band
     * attenuation to use, ATT, and the filter window function (a
     * Blackman-harris window in this case). The first input is the
     * gain, which is also specified as the interpolation rate so that
     * the output levels are the same as the input (this creates an
     * overall increase in power).
     *
     *   <B><EM>self._taps = filter.firdes.low_pass_2(interp, interp*fs, BW, TB,
     *      attenuation_dB=ATT, window=filter.firdes.WIN_BLACKMAN_hARRIS)</EM></B>
     *
     * The PFB interpolator code takes the taps generated above and
     * builds a set of filters. The set contains <EM>interp</EM>
     * filters and each filter contains
     * ceil(taps.size()/interp) taps. Each tap from the
     * filter prototype is sequentially inserted into the next
     * filter. When all of the input taps are used, the remaining
     * filters in the filterbank are filled out with 0's to make sure
     * each filter has the same number of taps.
     *
     * The theory behind this block can be found in Chapter 7.1 of the
     * following book:
     *
     *    <B><EM>f. harris, "Multirate Signal Processing for Communication
     *       Systems</EM>," Upper Saddle River, NJ: Prentice Hall,
     *       Inc. 2004.</EM></B>
     */

    class FILTER_API pfb_interpolator_ccf : virtual public sync_interpolator
    {
    public:
      // gr::filter::pfb_interpolator_ccf::sptr
      typedef boost::shared_ptr<pfb_interpolator_ccf> sptr;

      /*!
       * Build the polyphase filterbank interpolator.
       * \param interp  (unsigned integer) Specifies the interpolation rate to use
       * \param taps    (vector/list of floats) The prototype filter to populate the filterbank. The taps
       *                                        should be generated at the interpolated sampling rate.
       */
      static sptr make(unsigned int interp,
				  const std::vector<float> &taps);

      /*!
       * Resets the filterbank's filter taps with the new prototype filter
       * \param taps    (vector/list of floats) The prototype filter to populate the filterbank.
       *                The taps should be generated at the interpolated sampling rate.
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
    };

  } /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_FILTER_PFB_INTERPOLATOR_CCF_H */

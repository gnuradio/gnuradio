/* -*- c++ -*- */
/*
 * Copyright 2011,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_FILTER_ADAPTIVE_FIR_CCF_H
#define	INCLUDED_FILTER_ADAPTIVE_FIR_CCF_H

#include <gnuradio/filter/api.h>
#include <gnuradio/sync_decimator.h>

namespace gr {
  namespace filter {

    /*!
     * \brief Adaptive FIR filter with gr_complex input, gr_complex output and float taps
     * \ingroup filter_blk
     *
     * \details
     * This is a base class to implement an adaptive FIR
     * filter. Generally, another block will inherit from this one to
     * build a new type of adaptive filter such as an equalizer.
     *
     * This class implements two functions that are designed to be
     * overloaded by the child class: error(gr_complex out) and
     * update_tap(float tap, gr_complex in).
     *
     * The error() function calculates the error value that will be
     * used to adjust the taps. The update_tap function then uses the
     * error and the input signal value to update a particular
     * tap. Typically, the error is calculated for a given output and
     * then this is used in a loop to update all of the filter taps in
     * a loop:
     *
     * \code
     *     d_error = error(sum);
     *     for(k = 0; k < l; k++) {
     *       update_tap(d_taps[ntaps-k-1], in[i+k]);
     *     }
     * \endcode
     */
    class FILTER_API adaptive_fir_ccf : virtual public sync_decimator
    {
    public:
      // gr::filter::adaptive_fir_ccf::sptr
      typedef boost::shared_ptr<adaptive_fir_ccf> sptr;

      /*!
       * \brief Adaptive FIR filter with gr_complex input, gr_complex output and float taps
       *
       * \param name Provides a name to identify this type of algorithm
       * \param decimation (interger) decimation rate of the filter
       * \param taps (real) filter taps
       */
      static sptr make(const char *name, int decimation,
				  const std::vector<float> &taps);

      virtual void set_taps(const std::vector<float> &taps) = 0;
      virtual std::vector<float> taps() = 0;
    };

  } /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_FILTER_ADAPTIVE_FIR_CCF_H */

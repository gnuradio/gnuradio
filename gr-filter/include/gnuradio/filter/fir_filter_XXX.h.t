/* -*- c++ -*- */
/*
 * Copyright 2004,2012 Free Software Foundation, Inc.
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
#include <gnuradio/sync_decimator.h>

namespace gr {
  namespace filter {

    /*!
     * \brief FIR filter with @I_TYPE@ input, @O_TYPE@ output, and @TAP_TYPE@ taps
     * \ingroup filter_blk
     *
     * \details
     * The fir_filter_XXX blocks create finite impulse response
     * (FIR) filters that perform the convolution in the time
     * domain:
     *
     * \code
     *   out = 0
     *   for i in ntaps:
     *      out += input[n-i] * taps[i]
     * \endcode
     *
     * The taps are a C++ vector (or Python list) of values of the
     * type specified by the third letter in the block's suffix. For
     * this block, the value is of type @TAP_TYPE@. Taps can be
     * created using the firdes or optfir tools.
     *
     * These versions of the filter can also act as down-samplers
     * (or decimators) by specifying an integer value for \p
     * decimation.
     */
    class FILTER_API @BASE_NAME@ : virtual public sync_decimator
    {
    public:

      // gr::filter::@BASE_NAME@::sptr
      typedef boost::shared_ptr<@BASE_NAME@> sptr;

      /*!
       * \brief FIR filter with @I_TYPE@ input, @O_TYPE@ output, and @TAP_TYPE@ taps
       *
       * \param decimation set the integer decimation rate
       * \param taps a vector/list of taps of type @TAP_TYPE@
       */
      static sptr make(int decimation,
                       const std::vector<@TAP_TYPE@> &taps);

      virtual void set_taps(const std::vector<@TAP_TYPE@> &taps) = 0;
      virtual std::vector<@TAP_TYPE@> taps() const = 0;
    };

  } /* namespace filter */
} /* namespace gr */

#endif /* @GUARD_NAME@ */

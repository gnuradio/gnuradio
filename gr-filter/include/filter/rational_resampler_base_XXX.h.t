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

#include <filter/api.h>
#include <gr_block.h>


namespace gr {
  namespace filter {

    /*!
     * \brief Rational Resampling Polyphase FIR filter with @I_TYPE@
     * input, @O_TYPE@ output and @TAP_TYPE@ taps.
     *
     *\ingroup filter_blk
     */
    class FILTER_API @NAME@ : virtual public gr_block
    {
    public:
      // gr::filter::@BASE_NAME@::sptr
      typedef boost::shared_ptr<@BASE_NAME@> sptr;

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

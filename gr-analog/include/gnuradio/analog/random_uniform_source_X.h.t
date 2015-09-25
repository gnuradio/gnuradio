/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
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
#define @GUARD_NAME@

#include <gnuradio/analog/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace analog {

    /*!
     * \brief Uniform Random Number Generator with @TYPE@ output.
     * \ingroup waveform_generators_blk
     */
    class ANALOG_API @BASE_NAME@ : virtual public sync_block
    {
    public:
      // gr::analog::@BASE_NAME@::sptr
      typedef boost::shared_ptr<@BASE_NAME@> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of analog::random_uniform_source_X.
       *
       * To avoid accidental use of raw pointers, analog::random_uniform_source_b's
       * constructor is in a private implementation
       * class. analog::random_uniform_source_b::make is the public interface for
       * creating new instances.
       * \param minimum defines minimal integer value output.
       * \param maximum output values are below this value
       * \param seed for Pseudo Random Number Generator. Defaults to 0. In this case current time is used.
       */
      static sptr make(int minimum, int maximum, int seed);
    };

  } /* namespace analog */
} /* namespace gr */

#endif /* @GUARD_NAME@ */

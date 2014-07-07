/* -*- c++ -*- */
/*
 * Copyright 2008,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_ANALOG_FMDET_CF_H
#define INCLUDED_ANALOG_FMDET_CF_H

#include <gnuradio/analog/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace analog {

    /*!
     * \brief Implements an IQ slope detector
     * \ingroup modulators_blk
     *
     * \details
     * input: stream of complex; output: stream of floats
     *
     * This implements a limiting slope detector. The limiter is in
     * the normalization by the magnitude of the sample
     */
    class ANALOG_API fmdet_cf : virtual public sync_block
    {
    public:
      // gr::analog::fmdet_cf::sptr
      typedef boost::shared_ptr<fmdet_cf> sptr;

      /*!
       * \brief Make FM detector block.
       *
       * \param samplerate sample rate of signal (is not used; to be removed)
       * \param freq_low lowest frequency of signal (Hz)
       * \param freq_high highest frequency of signal (Hz)
       * \param scl scale factor
       */
      static sptr make(float samplerate, float freq_low,
		       float freq_high, float scl);

      virtual void set_scale(float scl) = 0;
      virtual void set_freq_range(float freq_low, float freq_high) = 0;

      virtual float freq() const = 0;
      virtual float freq_high() const = 0;
      virtual float freq_low() const = 0;
      virtual float scale() const = 0;
      virtual float bias() const = 0;
    };

  } /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_FMDET_CF_H */

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

#ifndef INCLUDED_DIGITAL_CMA_EQUALIZER_CC_H
#define	INCLUDED_DIGITAL_CMA_EQUALIZER_CC_H

#include <gnuradio/digital/api.h>
#include <gnuradio/sync_decimator.h>

namespace gr {
  namespace digital {

    /*!
     * \brief Implements constant modulus adaptive filter on complex stream.
     * \ingroup equalizers_blk
     *
     * \details
     * The error value and tap update equations (for p=2) can be found in:
     *
     * D. Godard, "Self-Recovering Equalization and Carrier Tracking
     * in Two-Dimensional Data Communication Systems," IEEE
     * Transactions on Communications, Vol. 28, No. 11, pp. 1867 -
     * 1875, 1980.
     */
    class DIGITAL_API cma_equalizer_cc: virtual public sync_decimator
    {
    protected:
      virtual gr_complex error(const gr_complex &out) = 0;
      virtual void update_tap(gr_complex &tap, const gr_complex &in) = 0;
  
    public:
      // gr::digital::cma_equalizer_cc::sptr
      typedef boost::shared_ptr<cma_equalizer_cc> sptr;

      /*!
       * Make a CMA Equalizer block
       *
       * \param num_taps Numer of taps in the equalizer (channel size)
       * \param modulus Modulus of the modulated signals
       * \param mu Gain of the update loop
       * \param sps Number of samples per symbol of the input signal
       */
      static sptr make(int num_taps, float modulus, float mu, int sps);

      virtual void set_taps(const std::vector<gr_complex> &taps) = 0;
      virtual std::vector<gr_complex> taps() const = 0;
      virtual float gain() const = 0;
      virtual void set_gain(float mu) = 0;
      virtual float modulus() const = 0;
      virtual void set_modulus(float mod) = 0;
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_CMA_EQUALIZER_CC_H */

/* -*- c++ -*- */
/*
 * Copyright 2004,2011,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_ANALOG_PLL_REFOUT_CC_H
#define INCLUDED_ANALOG_PLL_REFOUT_CC_H

#include <gnuradio/analog/api.h>
#include <gnuradio/blocks/control_loop.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace analog {

    /*!
     * \brief Implements a PLL which locks to the input frequency and outputs a carrier
     * \ingroup synchronizers_blk
     *
     * \details
     * Input stream 0: complex
     * Output stream 0: complex
     *
     * This PLL locks onto a [possibly noisy] reference carrier on the
     * input and outputs a clean version which is phase and frequency
     * aligned to it.
     *
     * All settings max_freq and min_freq are in terms of radians per
     * sample, NOT HERTZ. The loop bandwidth determines the lock range
     * and should be set around pi/200 -- 2pi/100.  \sa
     * pll_freqdet_cf, pll_carriertracking_cc
     */
    class ANALOG_API pll_refout_cc
      : virtual public sync_block,
        virtual public blocks::control_loop
    {
    public:
      // gr::analog::pll_refout_cc::sptr
      typedef boost::shared_ptr<pll_refout_cc> sptr;

      /* \brief Make PLL block that outputs the tracked carrier signal.
       *
       * \param loop_bw: control loop's bandwidth parameter.
       * \param max_freq: maximum (normalized) frequency PLL will lock to.
       * \param min_freq: minimum (normalized) frequency PLL will lock to.
       */
      static sptr make(float loop_bw, float max_freq, float min_freq);
    };

  } /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_PLL_REFOUT_CC_H */

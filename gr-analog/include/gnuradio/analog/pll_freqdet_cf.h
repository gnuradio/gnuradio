/* -*- c++ -*- */
/*
 * Copyright 2004,2011 Free Software Foundation, Inc.
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

#ifndef INCLUDED_ANALOG_PLL_FREQDET_CF_H
#define INCLUDED_ANALOG_PLL_FREQDET_CF_H

#include <gnuradio/analog/api.h>
#include <gnuradio/blocks/control_loop.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace analog {

    /*!
     * \brief Implements a PLL which locks to the input frequency and outputs
     * an estimate of that frequency.  Useful for FM Demod.
     * \ingroup synchronizers_blk
     *
     * \details
     * Input stream 0: complex
     * Output stream 0: float
     *
     * This PLL locks onto a [possibly noisy] reference carrier on
     * the input and outputs an estimate of that frequency in radians per sample.
     * All settings max_freq and min_freq are in terms of radians per sample,
     * NOT HERTZ.  The loop bandwidth determines the lock range and should be set
     * around pi/200 -- 2pi/100.
     * \sa pll_refout_cc, pll_carriertracking_cc
     */
    class ANALOG_API pll_freqdet_cf
      : virtual public sync_block,
        virtual public blocks::control_loop
    {
    public:
      // gr::analog::pll_freqdet_cf::sptr
      typedef boost::shared_ptr<pll_freqdet_cf> sptr;

      /* \brief Make PLL block that outputs the tracked signal's frequency.
       *
       * \param loop_bw: control loop's bandwidth parameter.
       * \param max_freq: maximum (normalized) frequency PLL will lock to.
       * \param min_freq: minimum (normalized) frequency PLL will lock to.
       */
      static sptr make(float loop_bw, float max_freq, float min_freq);

      virtual void set_loop_bandwidth(float bw) = 0;
      virtual void set_damping_factor(float df) = 0;
      virtual void set_alpha(float alpha) = 0;
      virtual void set_beta(float beta) = 0;
      virtual void set_frequency(float freq) = 0;
      virtual void set_phase(float phase) = 0;
      virtual void set_min_freq(float freq) = 0;
      virtual void set_max_freq(float freq) = 0;

      virtual float get_loop_bandwidth() const = 0;
      virtual float get_damping_factor() const = 0;
      virtual float get_alpha() const = 0;
      virtual float get_beta() const = 0;
      virtual float get_frequency() const = 0;
      virtual float get_phase() const = 0;
      virtual float get_min_freq() const = 0;
      virtual float get_max_freq() const = 0;
    };

  } /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_PLL_FREQDET_CF_H */

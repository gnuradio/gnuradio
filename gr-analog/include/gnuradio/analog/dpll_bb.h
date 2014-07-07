/* -*- c++ -*- */
/*
 * Copyright 2007,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_ANALOG_DPLL_BB_H
#define INCLUDED_ANALOG_DPLL_BB_H

#include <gnuradio/analog/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace analog {

    /*!
     * \brief Detect the peak of a signal
     * \ingroup peak_detectors_blk
     *
     * \details
     * If a peak is detected, this block outputs a 1,
     * or it outputs 0's.
     */
    class ANALOG_API dpll_bb : virtual public sync_block
    {
    public:
      // gr::analog::dpll_bb::sptr
      typedef boost::shared_ptr<dpll_bb> sptr;

      static sptr make(float period, float gain);

      virtual void set_gain(float gain) = 0;
      virtual void set_decision_threshold(float thresh) = 0;

      virtual float gain() const = 0;
      virtual float freq() const = 0;
      virtual float phase() const = 0;
      virtual float decision_threshold() const = 0;
    };

  } /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_DPLL_BB_H */

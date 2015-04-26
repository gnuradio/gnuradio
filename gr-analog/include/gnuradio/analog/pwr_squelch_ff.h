/* -*- c++ -*- */
/*
 * Copyright 2006,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_ANALOG_PWR_SQUELCH_FF_H
#define INCLUDED_ANALOG_PWR_SQUELCH_FF_H

#include <gnuradio/analog/api.h>
#include <gnuradio/analog/squelch_base_ff.h>
#include <cmath>

namespace gr {
  namespace analog {

    /*!
     * \brief gate or zero output when input power below threshold
     * \ingroup level_controllers_blk
     */
    class ANALOG_API pwr_squelch_ff :
      public squelch_base_ff, virtual public block
    {
    protected:
      virtual void update_state(const float &in) = 0;
      virtual bool mute() const = 0;

    public:
      // gr::analog::pwr_squelch_ff::sptr
      typedef boost::shared_ptr<pwr_squelch_ff> sptr;

      /*!
       * \brief Make power-based squelch block.
       *
       * \param db threshold (in dB) for power squelch
       * \param alpha Gain of averaging filter. Defaults to 0.0001.
       * \param ramp sets response characteristic. Defaults to 0.
       * \param gate if true, no output if no squelch tone.
       *             if false, output 0's if no squelch tone (default).
       *
       * The block will emit a tag with the key pmt::intern("squelch_sob")
       * with the value of pmt::PMT_NIL on the first item it passes, and with
       * the key pmt::intern("squelch:eob") on the last item it passes.
       */
      static sptr make(double db, double alpha=0.0001,
		       int ramp=0, bool gate=false);

      virtual std::vector<float> squelch_range() const = 0;

      virtual double threshold() const = 0;
      virtual void set_threshold(double db) = 0;
      virtual void set_alpha(double alpha) = 0;

      virtual int ramp() const = 0;
      virtual void set_ramp(int ramp) = 0;
      virtual bool gate() const = 0;
      virtual void set_gate(bool gate) = 0;
      virtual bool unmuted() const = 0;
    };

  } /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_PWR_SQUELCH_FF_H */

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

#ifndef INCLUDED_ANALOG_RAIL_FF_H
#define INCLUDED_ANALOG_RAIL_FF_H

#include <gnuradio/analog/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace analog {

    /*!
     * \brief clips input values to min, max
     * \ingroup level_controllers_blk
     */
    class ANALOG_API rail_ff : virtual public sync_block
    {
    public:
      // gr::analog::rail_ff::sptr
      typedef boost::shared_ptr<rail_ff> sptr;

      /*!
       * Build a rail block.
       *
       * \param lo the low value to clip to.
       * \param hi the high value to clip to.
       */
      static sptr make(float lo, float hi);

      virtual float lo() const = 0;
      virtual float hi() const = 0;

      virtual void set_lo(float lo) = 0;
      virtual void set_hi(float hi) = 0;
    };

  } /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_RAIL_FF_H */

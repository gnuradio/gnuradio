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

#ifndef INCLUDED_ANALOG_SQUELCH_BASE_FF_H
#define INCLUDED_ANALOG_SQUELCH_BASE_FF_H

#include <gnuradio/analog/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace analog {

    /*!
     * \brief basic squelch block; to be subclassed for other squelches.
     * \ingroup level_blk
     */
    class ANALOG_API squelch_base_ff : virtual public block
    {
    protected:
      virtual void update_state(const float &sample) = 0;
      virtual bool mute() const = 0;

    public:
      squelch_base_ff() {};
      virtual int ramp() const = 0;
      virtual void set_ramp(int ramp) = 0;
      virtual bool gate() const = 0;
      virtual void set_gate(bool gate) = 0;
      virtual bool unmuted() const = 0;

      virtual std::vector<float> squelch_range() const = 0;
    };

  } /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_SQUELCH_BASE_FF_H */

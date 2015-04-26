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

#ifndef INCLUDED_GR_SQUELCH_BASE_FF_IMPL_H
#define INCLUDED_GR_SQUELCH_BASE_FF_IMPL_H

#include <gnuradio/analog/squelch_base_ff.h>
#include <pmt/pmt.h>

namespace gr {
  namespace analog {

    class squelch_base_ff_impl : public squelch_base_ff
    {
    private:
      int    d_ramp;
      int    d_ramped;
      bool   d_gate;
      double d_envelope;
      enum { ST_MUTED, ST_ATTACK, ST_UNMUTED, ST_DECAY } d_state;
      const pmt::pmt_t d_sob_key, d_eob_key;
      bool d_tag_next_unmuted;

    protected:
      virtual void update_state(const float &sample) {};
      virtual bool mute() const { return false; };

    public:
      squelch_base_ff_impl(const char *name, int ramp, bool gate);
      ~squelch_base_ff_impl();

      int ramp() const;
      void set_ramp(int ramp);
      bool gate() const;
      void set_gate(bool gate);
      bool unmuted() const;

      virtual std::vector<float> squelch_range() const = 0;

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_SQUELCH_BASE_IMPL_FF_H */

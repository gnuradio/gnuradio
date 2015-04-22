/* -*- c++ -*- */
/*
 * Copyright 2004,2006,2012 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "squelch_base_ff_impl.h"
#include <gnuradio/io_signature.h>
#include <pmt/pmt.h>

namespace gr {
  namespace analog {

    squelch_base_ff_impl::squelch_base_ff_impl(const char *name, int ramp, bool gate)
      : block(name,
                 io_signature::make(1, 1, sizeof(float)),
                 io_signature::make(1, 1, sizeof(float))),
        d_sob_key(pmt::intern("squelch_sob")),
        d_eob_key(pmt::intern("squelch_eob")),
        d_tag_next_unmuted(true)
    {
      set_ramp(ramp);
      set_gate(gate);
      d_state = ST_MUTED;
      d_envelope = d_ramp ? 0.0 : 1.0;
      d_ramped = 0;
    }

    squelch_base_ff_impl::~squelch_base_ff_impl()
    {
    }

    int
    squelch_base_ff_impl::ramp() const
    {
      return d_ramp;
    }

    void
    squelch_base_ff_impl::set_ramp(int ramp)
    {
      d_ramp = ramp;
    }

    bool
    squelch_base_ff_impl::gate() const
    {
      return d_gate;
    }

    void
    squelch_base_ff_impl::set_gate(bool gate)
    {
      d_gate = gate;
    }

    bool
    squelch_base_ff_impl::unmuted() const
    {
      return (d_state == ST_UNMUTED || d_state == ST_ATTACK);
    }

    int
    squelch_base_ff_impl::general_work(int noutput_items,
				       gr_vector_int &ninput_items,
				       gr_vector_const_void_star &input_items,
				       gr_vector_void_star &output_items)
    {
      const float *in = (const float *) input_items[0];
      float *out = (float *) output_items[0];

      int j = 0;

      for(int i = 0; i < noutput_items; i++) {
        update_state(in[i]);

        // Adjust envelope based on current state
        switch(d_state) {
          case ST_MUTED:
            if(!mute()) {
              // If not ramping, go straight to unmuted
              d_state = d_ramp ? ST_ATTACK : ST_UNMUTED;
              if(d_state == ST_UNMUTED)
                d_tag_next_unmuted = true;
            }
            break;

          case ST_UNMUTED:
            if(d_tag_next_unmuted) {
              d_tag_next_unmuted = false;
              add_item_tag(0, nitems_written(0) + j, d_sob_key, pmt::PMT_NIL);
            }
            if(mute()) {
              // If not ramping, go straight to muted
              d_state = d_ramp ? ST_DECAY : ST_MUTED;
              if(d_state == ST_MUTED)
                add_item_tag(0, nitems_written(0) + j, d_eob_key, pmt::PMT_NIL);
            }
            break;

          case ST_ATTACK:
            // FIXME: precalculate window for speed
            d_envelope = 0.5-std::cos(M_PI*(++d_ramped)/d_ramp)/2.0;

            // use >= in case d_ramp is set to lower value elsewhere
            if(d_ramped >= d_ramp) {
              d_state = ST_UNMUTED;
              d_tag_next_unmuted = true;
              d_envelope = 1.0;
            }
            break;

          case ST_DECAY:
            // FIXME: precalculate window for speed
            d_envelope = 0.5-std::cos(M_PI*(--d_ramped)/d_ramp)/2.0;
            if(d_ramped == 0.0) {
              d_state = ST_MUTED;
              add_item_tag(0, nitems_written(0) + j, d_eob_key, pmt::PMT_NIL);
            }
            break;
        };

        // If unmuted, copy input times envelope to output
        // Otherwise, if not gating, copy zero to output
        if(d_state != ST_MUTED)
          out[j++] = in[i]*d_envelope;
        else
          if(!d_gate)
            out[j++] = 0.0;
      }

      consume_each(noutput_items);  // Use all the inputs
      return j;		        // But only report outputs copied
    }

  } /* namespace analog */
} /* namespace gr */

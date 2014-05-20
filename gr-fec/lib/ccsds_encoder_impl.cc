/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
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

#include "ccsds_encoder_impl.h"
#include <gnuradio/fec/generic_encoder.h>
#include <cstdio>

extern "C" {
#include <gnuradio/fec/viterbi.h>
}

namespace gr {
  namespace fec {
    namespace code {

      generic_encoder::sptr
      ccsds_encoder::make(int frame_size, int start_state,
                          cc_mode_t mode)
      {
        return generic_encoder::sptr
          (new ccsds_encoder_impl(frame_size, start_state, mode));
      }

      ccsds_encoder_impl::ccsds_encoder_impl(int frame_size, int start_state,
                                             cc_mode_t mode)
        : generic_encoder("ccsds_encoder"),
          d_start_state(static_cast<unsigned char>(start_state)),
          d_mode(mode)
      {
        d_max_frame_size = frame_size;
        set_frame_size(frame_size);
      }

      ccsds_encoder_impl::~ccsds_encoder_impl()
      {
      }

      int
      ccsds_encoder_impl::get_output_size()
      {
        return d_output_size;
      }

      int
      ccsds_encoder_impl::get_input_size()
      {
        return d_frame_size/8; // packed byte input
      }

      const char*
      ccsds_encoder_impl::get_input_conversion()
      {
        // Expects packed data input; tell wrapper to pack it.
        return "pack";
      }

      bool
      ccsds_encoder_impl::set_frame_size(unsigned int frame_size)
      {
        bool ret = true;
        if(frame_size > d_max_frame_size) {
          GR_LOG_INFO(d_logger, boost::format("tried to set frame to %1%; max possible is %2%") \
                      % frame_size % d_max_frame_size);
          frame_size = d_max_frame_size;
          ret = false;
        }

        d_frame_size = frame_size;

        if(d_mode == CC_TERMINATED) {
          d_output_size = 2*(d_frame_size + 7 - 1);
        }
        else {
          d_output_size = 2*d_frame_size;
        }

        return ret;
      }

      double
      ccsds_encoder_impl::rate()
      {
        return 0.5;
      }

      void
      ccsds_encoder_impl::generic_work(void *in_buffer, void *out_buffer)
      {
        unsigned char *in = (unsigned char*) in_buffer;
        unsigned char *out = (unsigned char*) out_buffer;

        unsigned char my_state = d_start_state;

        if(d_mode == CC_TAILBITING) {
          // Grab K-1 (6) bits of data from the last input byte to add
          // onto the from of the encoding stream for tailbiting mode.
          unsigned char sym = in[d_frame_size/8 - 1];
          for(unsigned int i = 0; i < 7 - 1; ++i) {
            my_state = (my_state << 1) | ((sym >> (5 - i)) & 1);
          }
        }

        my_state = encode(out, in, d_frame_size/8, my_state);

        if(d_mode == CC_TERMINATED) {
          // encode works on bytes, but we are only adding some number
          // of bits to the end of the frame, so we abuse the encode
          // function by reshifting the start state within the
          // for-loop and only taking the last two bits out of the
          // encoded 2-bytes.
          unsigned char end_bits[16];
          for(unsigned int i = 0; i < (7 - 1); ++i) {
            my_state = (my_state << 1) | ((d_start_state >> (7 - 2 - i)) & 1);
            encode(&end_bits[0], &my_state, 1, my_state);
            out[(i + d_frame_size) * 2 + 0] = end_bits[14];
            out[(i + d_frame_size) * 2 + 1] = end_bits[15];
          }
        }

        if(d_mode == CC_TRUNCATED) {
          my_state = d_start_state;
        }

        d_start_state = my_state;
      }

    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */

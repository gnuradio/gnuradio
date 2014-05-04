/* -*- c++ -*- */
/*
 * Copyright 2013-2014 Free Software Foundation, Inc.
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

#include "ccsds_decoder_impl.h"
#include <math.h>
#include <boost/assign/list_of.hpp>
#include <volk/volk.h>
#include <sstream>
#include <vector>
#include <iostream>

namespace gr {
  namespace fec {
    namespace code {

      generic_decoder::sptr
      ccsds_decoder::make(int frame_size,
                          int start_state, int end_state,
                          cc_mode_t mode)
      {
        return generic_decoder::sptr
          (new ccsds_decoder_impl(frame_size, start_state, end_state, mode));
      }

      ccsds_decoder_impl::ccsds_decoder_impl(int frame_size,
                                             int start_state, int end_state,
                                             cc_mode_t mode)
        : generic_decoder("ccsds_decoder"),
          d_count(0), d_frame_size(frame_size),
          d_mode(mode),
          d_start_state_chaining(start_state),
          d_start_state_nonchaining(start_state),
          d_end_state_nonchaining(end_state)
      {
        // Set max frame size here; all buffers and settings will be
        // based on this value.
        d_max_frame_size = frame_size;
        set_frame_size(frame_size);

        float RATE = 0.5;
        float ebn0 = 12.0;
        float esn0 = RATE*pow(10.0, ebn0/10.0);

        gen_met(d_mettab, 100, esn0, 0.0, 4);
        viterbi_chunks_init(d_state0);
        viterbi_chunks_init(d_state1);
      }

      ccsds_decoder_impl::~ccsds_decoder_impl()
      {
      }

      int
      ccsds_decoder_impl::get_output_size()
      {
        // packed bits
        return d_frame_size/8;
      }

      int
      ccsds_decoder_impl::get_input_size()
      {
        if(d_mode == CC_TERMINATED) {
          return 2*(d_frame_size + 7 - 1);
        }
        else {
          return 2*d_frame_size;
        }
      }

      int
      ccsds_decoder_impl::get_input_item_size()
      {
        return sizeof(float);
      }

      int
      ccsds_decoder_impl::get_history()
      {
        if(d_mode == CC_STREAMING) {
          return 2*(7 - 1);
        }
        else {
          return 0;
        }
      }

      float
      ccsds_decoder_impl::get_shift()
      {
        return 0.0;
      }

      const char*
      ccsds_decoder_impl::get_input_conversion()
      {
        return "none";
      }

      const char*
      ccsds_decoder_impl::get_output_conversion()
      {
        return "unpack";
      }

      bool
      ccsds_decoder_impl::set_frame_size(unsigned int frame_size)
      {
        bool ret = true;
        if(frame_size > d_max_frame_size) {
          GR_LOG_INFO(d_logger, boost::format("tried to set frame to %1%; max possible is %2%") \
                      % frame_size % d_max_frame_size);
          frame_size = d_max_frame_size;
          ret = false;
        }

        d_frame_size = frame_size;

        return ret;
     }

      double
      ccsds_decoder_impl::rate()
      {
        return 0.5;
      }

      void
      ccsds_decoder_impl::generic_work(void *inbuffer, void *outbuffer)
      {
        const float *in = (const float*)inbuffer;
        unsigned char *out = (unsigned char*)outbuffer;

        switch(d_mode) {

        case(CC_STREAMING):
          for(unsigned int i = 0; i < d_frame_size*2; i++) {
            // Translate and clip [-1.0..1.0] to [28..228]
            float sample = in[i]*100.0+128.0;
            if(sample > 255.0)
              sample = 255.0;
            else if(sample < 0.0)
              sample = 0.0;

            d_viterbi_in[d_count % 4] = (unsigned char)(floorf(sample));
            if((d_count % 4) == 3) {
              // Every fourth symbol, perform butterfly operation
              viterbi_butterfly2(d_viterbi_in, d_mettab, d_state0, d_state1);

              // Every sixteenth symbol, read out a byte
              if(d_count % 16 == 11) {
                unsigned char tmp;
                viterbi_get_output(d_state0, out++);
                viterbi_get_output(d_state0, &tmp);
              }
            }

            d_count++;
          }
          break;

        default:
          throw std::runtime_error("ccsds_decoder: mode not recognized");
        }
      }

    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */

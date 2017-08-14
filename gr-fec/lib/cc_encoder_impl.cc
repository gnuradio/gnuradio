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

#include "cc_encoder_impl.h"
#include <math.h>
#include <boost/assign/list_of.hpp>
#include <volk/volk_typedefs.h>
#include <volk/volk.h>
#include <sstream>
#include <stdio.h>
#include <vector>
#include <gnuradio/fec/generic_encoder.h>
#include <gnuradio/fec/cc_common.h>

namespace gr {
  namespace fec {
    namespace code {

      generic_encoder::sptr
      cc_encoder::make(int frame_size, int k, int rate,
                       std::vector<int> polys, int start_state,
                       cc_mode_t mode, bool padded)
      {
        return generic_encoder::sptr
          (new cc_encoder_impl(frame_size, k, rate,
                               polys, start_state,
                               mode, padded));
      }

      cc_encoder_impl::cc_encoder_impl(int frame_size, int k, int rate,
                                       std::vector<int> polys, int start_state,
                                       cc_mode_t mode, bool padded)
        : generic_encoder("cc_encoder"),
          d_rate(rate), d_k(k), d_polys(polys),
          d_start_state(start_state),
          d_mode(mode), d_padding(0)
      {
        if(static_cast<size_t>(d_rate) != d_polys.size()) {
          throw std::runtime_error("cc_encoder: Number of polynomials must be the same as the value of rate");
        }

        partab_init();

        // set up a padding factor. If padding, extends the encoding
        // by this many bits to fit into a full byte.
        if(padded && (mode == CC_TERMINATED)) {
          d_padding = static_cast<int>(8.0f*ceilf(d_rate*(d_k-1)/8.0f) - (d_rate*(d_k-1)));
        }

        d_max_frame_size = frame_size;
        set_frame_size(frame_size);
      }

      cc_encoder_impl::~cc_encoder_impl()
      {
      }

      int
      cc_encoder_impl::get_output_size()
      {
        return d_output_size;
      }

      int
      cc_encoder_impl::get_input_size()
      {
        return d_frame_size;
      }

      bool
      cc_encoder_impl::set_frame_size(unsigned int frame_size)
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
          d_output_size = d_rate * (d_frame_size + d_k - 1) + d_padding;
        }
        /*
        else if(d_trunc_intrinsic) {
          int cnt = 0;
          for(int i = 0; i < d_rate; ++i) {
            if (d_polys[i] != 1) {
              cnt++;
            }
          }
          d_output_size = (d_rate * (d_frame_size)) + (cnt * (d_k - 1));
        }
        */
        else {
          d_output_size = d_rate * d_frame_size;
        }

        return ret;
      }

      double
      cc_encoder_impl::rate()
      {
        return static_cast<double>(d_rate);
      }

      int
      cc_encoder_impl::parity(int x)
      {
        x ^= (x >> 16);
        x ^= (x >> 8);
        return parityb(x);
      }

      int
      cc_encoder_impl::parityb(unsigned char x)
      {
        return Partab[x];
      }

      void
      cc_encoder_impl::partab_init(void)
      {
        int i,cnt,ti;

        /* Initialize parity lookup table */
        for(i=0;i<256;i++){
          cnt = 0;
          ti = i;
          while(ti){
            if(ti & 1)
              cnt++;
            ti >>= 1;
          }
          Partab[i] = cnt & 1;
        }
      }

      void
      cc_encoder_impl::generic_work(void *in_buffer, void *out_buffer)
      {
        const unsigned char *in = (const unsigned char *) in_buffer;
        unsigned char *out = (unsigned char *) out_buffer;

        unsigned char my_state = d_start_state;

        if(d_mode == CC_TAILBITING) {
          for(unsigned int i = 0; i < d_k - 1; ++i) {
            my_state = (my_state << 1) | (in[d_frame_size - (d_k - 1)  + i] & 1);
          }
        }

        for(unsigned int i = 0; i < d_frame_size; ++i) {
          my_state = (my_state << 1) | (in[i] & 1);
          for(unsigned int j = 0; j < d_rate; ++j) {
            out[i * d_rate + j] = (d_polys[j] < 0) ^ parity(my_state & abs(d_polys[j])) ? 1 : 0;
          }
        }

        if(d_mode == CC_TERMINATED) {
          for(unsigned int i = 0; i < d_k - 1; ++i) {
            my_state = (my_state << 1) | ((d_start_state >> (d_k - 2 - i)) & 1);
            for(unsigned int j = 0; j < d_rate; ++j) {
              out[(i + d_frame_size) * d_rate + j] = (d_polys[j] < 0) ^ parity(my_state & abs(d_polys[j])) ? 1 : 0;
            }
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

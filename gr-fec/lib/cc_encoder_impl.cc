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
      cc_encoder::make(int framebits, int k,
                       int rate, std::vector<int> polys,
                       int start_state, int end_state,
                       bool tailbiting, bool terminated,
                       bool truncated, bool streaming)
      {
        return generic_encoder::sptr
          (new cc_encoder_impl(framebits, k,
                               rate, polys,
                               start_state, end_state,
                               tailbiting, terminated,
                               truncated, streaming));
      }

      cc_encoder_impl::cc_encoder_impl(int framebits, int k,
                                       int rate, std::vector<int> polys,
                                       int start_state, int end_state,
                                       bool tailbiting, bool terminated,
                                       bool truncated, bool streaming)
        : d_framebits(framebits),
          d_rate(rate), d_k(k), d_polys(polys),
          d_start_state(start_state),
          d_tailbiting(tailbiting), d_terminated(terminated),
          d_truncated(truncated), d_streaming(streaming)
      {
        partab_init();
      }

      cc_encoder_impl::~cc_encoder_impl()
      {
      }

      int
      cc_encoder_impl::get_output_size()
      {
        if(d_terminated) {
          return d_rate * (d_framebits + d_k - 1);
        }
        /*
        else if(d_trunc_intrinsic) {
          int cnt = 0;
          for(int i = 0; i < d_rate; ++i) {
            if (d_polys[i] != 1) {
              cnt++;
            }
          }
          return (d_rate * (d_framebits)) + (cnt * (d_k - 1));
        }
        */
        else {
          return d_rate * d_framebits;
        }
      }

      int
      cc_encoder_impl::get_input_size()
      {
        return d_framebits;
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

        int my_state = d_start_state;
        //printf("ms: %d\n", my_state);

        if(d_tailbiting) {
          for(unsigned int i = 0; i < d_k - 1; ++i) {
            my_state = (my_state << 1) | (in[d_framebits - (d_k - 1)  + i] & 1);
          }
        }
        //printf("start... %d\n", my_state & ((1 << (d_k - 1)) - 1));

        for(unsigned int i = 0; i < d_framebits; ++i) {
          my_state = (my_state << 1) | (in[i] & 1);
          for(unsigned int j = 0; j < d_rate; ++j) {
            out[i * d_rate + j] = parity(my_state & d_polys[j]) == 0 ? 0 : 1;
          }
        }

        if(d_terminated) {
          for(unsigned int i = 0; i < d_k - 1; ++i) {
            my_state = (my_state << 1) | ((d_start_state >> (d_k - 2 - i)) & 1);
            for(unsigned int j = 0; j < d_rate; ++j) {
              out[(i + d_framebits) * d_rate + j] = parity(my_state & d_polys[j]) == 0 ? 0 : 1;
            }
          }
        }

        /*
        if(d_trunc_intrinsic) {
          for(int i = 0; i < d_k - 1; ++i) {
            my_state = (my_state << 1) | ((d_start_state >> d_k - 2 - i) & 1);
            int cnt = 0;
            for(int j = 0; j < d_rate; ++j) {
              if(d_polys[j] != 1) {
                out[(i + d_framebits) * d_rate + cnt] = parity(my_state & d_polys[j]) == 0 ? 0 : 1;
                cnt++;
              }
            }
          }
        }
        */

        if(d_truncated) {
          //printf("end... %d\n", my_state & ((1 << (d_k - 1)) - 1));
          my_state = d_start_state;
        }

        d_start_state = my_state;
        //d_start_state = my_state & (1 << d_k -1) - 1;
        //printf("ms: %d\n", d_start_state);

        /*
        for(int i = d_framebits * d_rate - 25; i < d_framebits * d_rate; ++i) {
          //for(int i = 0; i < 25; ++i) {
          printf("...%d : %u\n", out[i], i);
        }
        */
      }

    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */


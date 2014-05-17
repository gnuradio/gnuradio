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
p * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_FEC_CC_ENCODER_IMPL_H
#define INCLUDED_FEC_CC_ENCODER_IMPL_H

#include <map>
#include <string>
#include <gnuradio/fec/cc_encoder.h>

namespace gr {
  namespace fec {
    namespace code {

      class FEC_API cc_encoder_impl : public cc_encoder
      {
      private:
        //plug into the generic fec api
        void generic_work(void *inbuffer, void *outbuffer);
        int get_output_size();
        int get_input_size();

        //everything else...
        unsigned char Partab[256];
        unsigned int d_frame_size;
        unsigned int d_max_frame_size;
        unsigned int d_rate;
        unsigned int d_k;
        std::vector<int> d_polys;
        struct v* d_vp;
        int d_numstates;
        int d_decision_t_size;
        int d_start_state;
        cc_mode_t d_mode;
        int d_padding;
        int d_output_size;

        int parity(int x);
        int parityb(unsigned char x);
        void partab_init(void);

      public:
        cc_encoder_impl(int frame_size, int k, int rate,
                        std::vector<int> polys, int start_state = 0,
                        cc_mode_t mode=CC_STREAMING, bool padded=false);
        ~cc_encoder_impl();

        bool set_frame_size(unsigned int frame_size);
        double rate();
      };

    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_CC_ENCODER_IMPL_H */

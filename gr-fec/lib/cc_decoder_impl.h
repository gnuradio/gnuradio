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

#ifndef INCLUDED_FEC_CC_DECODER_IMPL_H
#define INCLUDED_FEC_CC_DECODER_IMPL_H

#include <map>
#include <string>
#include <gnuradio/fec/cc_decoder.h>

namespace gr {
  namespace fec {
    namespace code {

      class FEC_API cc_decoder_impl : public cc_decoder
      {
      private:
        //plug into the generic fec api
        int get_output_size();
        int get_input_size();
        int get_history();
        float get_shift();
        int get_input_item_size();
        const char* get_input_conversion();
        //const char* get_output_conversion();

        //everything else...
        void create_viterbi();
        int init_viterbi(struct v* vp, int starting_state);
        int init_viterbi_unbiased(struct v* vp);
        int update_viterbi_blk(unsigned char* syms, int nbits);
        int chainback_viterbi(unsigned char* data, unsigned int nbits,
                              unsigned int endstate, unsigned int tailsize);
        int find_endstate();
        int tester[12];

        unsigned char *Branchtab;
        unsigned char Partab[256];


        int d_ADDSHIFT;
        int d_SUBSHIFT;
        conv_kernel d_kernel;
        unsigned int d_max_frame_size;
        unsigned int d_frame_size;
        unsigned int d_k;
        unsigned int d_rate;
        unsigned int d_partial_rate;
        std::vector<int> d_polys;
        cc_mode_t d_mode;
        int d_padding;

        struct v* d_vp;
        unsigned char* d_managed_in;
        unsigned int d_managed_in_size;
        int d_numstates;
        int d_decision_t_size;
        int *d_start_state;
        int d_start_state_chaining;
        int d_start_state_nonchaining;
        int *d_end_state;
        int d_end_state_chaining;
        int d_end_state_nonchaining;
        unsigned int d_veclen;

        int parity(int x);
        int parityb(unsigned char x);
        void partab_init(void);

      public:
        cc_decoder_impl(int frame_size, int k,
                        int rate, std::vector<int> polys,
                        int start_state = 0, int end_state = -1,
                        cc_mode_t mode=CC_STREAMING, bool padded=false);
        ~cc_decoder_impl();

        void generic_work(void *inbuffer, void *outbuffer);
        bool set_frame_size(unsigned int frame_size);
        double rate();
      };

    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_CC_DECODER_IMPL_H */

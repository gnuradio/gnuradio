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

#ifndef INCLUDED_FEC_CCSDS_DECODER_IMPL_H
#define INCLUDED_FEC_CCSDS_DECODER_IMPL_H

#include <map>
#include <string>
#include <gnuradio/fec/ccsds_decoder.h>

extern "C" {
#include <gnuradio/fec/viterbi.h>
}

namespace gr {
  namespace fec {
    namespace code {

      class FEC_API ccsds_decoder_impl : public ccsds_decoder
      {
      private:
        //plug into the generic FECAPI
        void generic_work(void *inbuffer, void *outbuffer);
        int get_output_size();
        int get_input_size();
        int get_history();
        float get_shift();
        int get_input_item_size();
        const char* get_input_conversion();
        const char* get_output_conversion();

        // Viterbi state
        int d_mettab[2][256];
        struct viterbi_state d_state0[64];
        struct viterbi_state d_state1[64];
        unsigned char d_viterbi_in[16];
        int d_count;

        unsigned int d_max_frame_size;
        unsigned int d_frame_size;
        cc_mode_t d_mode;

        int *d_start_state;
        int d_start_state_chaining;
        int d_start_state_nonchaining;
        int *d_end_state;
        int d_end_state_chaining;
        int d_end_state_nonchaining;

      public:
        ccsds_decoder_impl(int frame_size,
                           int start_state=0, int end_state=-1,
                           cc_mode_t mode=CC_STREAMING);
        ~ccsds_decoder_impl();

        bool set_frame_size(unsigned int frame_size);
        double rate();
      };

    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_CCSDS_DECODER_IMPL_H */

/* -*- c++ -*- */
/* 
 * Copyright 2016 Free Software Foundation, Inc.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "dvb_interleaver_bb_impl.h"
#include "gnuradio/dtv/atsc_consts.h"
#include <stdio.h>

namespace gr {
  namespace dtv {

    dvb_interleaver_bb::sptr
    dvb_interleaver_bb::make(dvb_standard_t standard, int I, int J, int vlength)
    {
      return gnuradio::get_initial_sptr
        (new dvb_interleaver_bb_impl(standard, I, J, vlength));
    }

    /*
     * The private constructor
     */
    dvb_interleaver_bb_impl::dvb_interleaver_bb_impl(dvb_standard_t standard, int I, int J, int vlength)
      : gr::sync_block("dvb_interleaver_bb",
              gr::io_signature::make(1, 1, sizeof(unsigned char) * vlength),
              gr::io_signature::make(1, 1, sizeof(unsigned char) * vlength))
    {
      registers = (unsigned char *) malloc(sizeof(unsigned char) * I * ((I - 1) * J));
      if (registers == NULL) {
        fprintf(stderr, "Convolutional Interleaver, Out of memory.\n");
        exit(1);
      }

      pointers = (int *) malloc(sizeof(int) * I);
      if (pointers == NULL) {
        fprintf(stderr, "Convolutional Interleaver, Out of memory.\n");
        exit(1);
      }

      memset(registers, 0, sizeof(unsigned char) * I * ((I - 1) * J));
      memset(pointers, 0, sizeof(int) * I);

      if (standard == STANDARD_ATSC) {
        this->I = 52;
        this->J = 4;
      }
      else {
        this->I = I;
        this->J = J;
      }
      commutator = 0;
      this->standard = standard;
    }

    /*
     * Our virtual destructor.
     */
    dvb_interleaver_bb_impl::~dvb_interleaver_bb_impl()
    {
      free(pointers);
      free(registers);
    }

    int
    dvb_interleaver_bb_impl::work(int noutput_items,
                              gr_vector_const_void_star &input_items,
                              gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char *) input_items[0];
      unsigned char *out = (unsigned char *) output_items[0];
      const atsc_mpeg_packet_rs_encoded *in_atsc = (const atsc_mpeg_packet_rs_encoded *) input_items[0];
      atsc_mpeg_packet_rs_encoded *out_atsc = (atsc_mpeg_packet_rs_encoded *) output_items[0];
      int p, n = ATSC_MPEG_RS_ENCODED_LENGTH;

      if (standard == STANDARD_ATSC) {
        for (int i = 0; i < noutput_items; i++) {
          assert(in_atsc[i].pli.regular_seg_p());
          plinfo::sanity_check(in_atsc[i].pli);

          out_atsc[i].pli = in_atsc[i].pli;                 // copy pipeline info
          if (in_atsc[i].pli.first_regular_seg_p()) {       // reset commutator if required
            commutator = 0;
          }
          for (int j = 0; j < n; j++) {
            if (commutator == 0) {
              out_atsc[i].data[j] = in_atsc[i].data[j];
            }
            else {
              p = pointers[commutator];

              out_atsc[i].data[j] = registers[commutator * (I - 1) * J + p];
              registers[commutator * (I - 1) * J + p] = in_atsc[i].data[j];

              pointers[commutator] = (p + 1) % (commutator * J);
            }
            commutator = (commutator + 1) % I;
          }
        }
      }
      else {
        for (int i = 0; i < noutput_items; i++) {
          if (commutator == 0) {
            out[i] = in[i];
          }
          else {
            p = pointers[commutator];

            out[i] = registers[commutator * (I-1) * J + p];
            registers[commutator * (I-1) * J + p] = in[i];

            pointers[commutator] = (p + 1) % (commutator * J);
          }
          commutator = (commutator + 1) % I;
        }
      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace dtv */
} /* namespace gr */


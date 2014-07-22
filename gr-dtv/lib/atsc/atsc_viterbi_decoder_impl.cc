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

#include <gnuradio/io_signature.h>
#include "atsc_viterbi_decoder_impl.h"
#include "atsc_types.h"
#include <atsc_viterbi_mux.cc> // machine generated
#include <emmintrin.h>

namespace gr {
  namespace dtv {

    atsc_viterbi_decoder::sptr
    atsc_viterbi_decoder::make()
    {
      return gnuradio::get_initial_sptr
	(new atsc_viterbi_decoder_impl());
    }

    atsc_viterbi_decoder_impl::atsc_viterbi_decoder_impl()
      : sync_block("dtv_atsc_viterbi_decoder",
                   io_signature::make(1, 1, sizeof(atsc_soft_data_segment)),
                   io_signature::make(1, 1, sizeof(atsc_mpeg_packet_rs_encoded)))
    {
      set_output_multiple(NCODERS);

      /*
       * These fifo's handle the alignment problem caused by the
       * inherent decoding delay of the individual viterbi decoders.
       * The net result is that this entire block has a pipeline latency
       * of 12 complete segments.
       *
       * If anybody cares, it is possible to do it with less delay, but
       * this approach is at least somewhat understandable...
       */

      // the -4 is for the 4 sync symbols
      int fifo_size = ATSC_DATA_SEGMENT_LENGTH - 4 - viterbi[0].delay();
      for (int i = 0; i < NCODERS; i++)
        fifo[i] = new fifo_t(fifo_size);

      reset();
    }

    atsc_viterbi_decoder_impl::~atsc_viterbi_decoder_impl()
    {
      for (int i = 0; i < NCODERS; i++)
        delete fifo[i];
    }

    void
    atsc_viterbi_decoder_impl::reset()
    {
      for (int i = 0; i < NCODERS; i++)
        fifo[i]->reset();
    }

    int
    atsc_viterbi_decoder_impl::work(int noutput_items,
                                    gr_vector_const_void_star &input_items,
                                    gr_vector_void_star &output_items)
    {
      const atsc_soft_data_segment *in = (const atsc_soft_data_segment *) input_items[0];
      atsc_mpeg_packet_rs_encoded *out = (atsc_mpeg_packet_rs_encoded *) output_items[0];

      // The way the fs_checker works ensures we start getting packets
      // starting with a field sync, and out input multiple is set to
      // 12, so we should always get a mod 12 numbered first packet
      assert(noutput_items % NCODERS == 0);

      int dbwhere;
      int dbindex;
      int shift;
      float symbols[NCODERS][enco_which_max];
      unsigned char dibits[NCODERS][enco_which_max];

      unsigned char out_copy[OUTPUT_SIZE];

      for (int i = 0; i < noutput_items; i += NCODERS) {
        /* Build a continuous symbol buffer for each encoder */
        for (unsigned int encoder = 0; encoder < NCODERS; encoder++)
          for (unsigned int k = 0; k < enco_which_max; k++)
            symbols[encoder][k] = in[i + (enco_which_syms[encoder][k]/832)].data[enco_which_syms[encoder][k] % 832];

        /* Now run each of the 12 Viterbi decoders over their subset of
           the input symbols */
        for (unsigned int encoder = 0; encoder < NCODERS; encoder++)
          for (unsigned int k = 0; k < enco_which_max; k++)
            dibits[encoder][k] = viterbi[encoder].decode(symbols[encoder][k]);

        /* Move dibits into their location in the output buffer */
        for (unsigned int encoder = 0; encoder < NCODERS; encoder++) {
          for (unsigned int k = 0; k < enco_which_max; k++) {
            /* Store the dibit into the output data segment */
            dbwhere = enco_which_dibits[encoder][k];
            dbindex = dbwhere >> 3;
            shift = dbwhere & 0x7;
            out_copy[dbindex] = (out_copy[dbindex] & ~(0x03 << shift)) | (fifo[encoder]->stuff(dibits[encoder][k]) << shift);
          } /* Symbols fed into one encoder */
        } /* Encoders */

        // copy output from contiguous temp buffer into final output
        for (int j = 0; j < NCODERS; j++) {
          memcpy (&out[i + j].data[0],
                  &out_copy[j * OUTPUT_SIZE/NCODERS],
                  ATSC_MPEG_RS_ENCODED_LENGTH * sizeof(out_copy[0]));

          // adjust pipeline info to reflect 12 segment delay
          plinfo::delay(out[i + j].pli, in[i + j].pli, NCODERS);
        }
      }

      return noutput_items;
    }

  } /* namespace dtv */
} /* namespace gr */

/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
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
#include "atsc_field_sync_mux_impl.h"
#include "gnuradio/dtv/atsc_consts.h"

namespace gr {
  namespace dtv {

    static const int N_SAVED_SYMBOLS = 12;

    atsc_field_sync_mux::sptr
    atsc_field_sync_mux::make()
    {
      return gnuradio::get_initial_sptr
        (new atsc_field_sync_mux_impl());
    }

    atsc_field_sync_mux_impl::atsc_field_sync_mux_impl()
      : gr::block("atsc_field_sync_mux",
                  gr::io_signature::make(1, 1, sizeof(atsc_data_segment)),
                  gr::io_signature::make(1, 1, sizeof(atsc_data_segment)))
    {
      d_already_output_field_sync = false;
      for (int i = 0; i < N_SAVED_SYMBOLS; i++) {
          d_saved_symbols[i] = 0;
        }
    }

    atsc_field_sync_mux_impl::~atsc_field_sync_mux_impl()
    {
    }

    void
    atsc_field_sync_mux_impl::init_field_sync_common(unsigned char *p, int mask,
                                                     const unsigned char saved_symbols[N_SAVED_SYMBOLS])
    {
      static const unsigned char bin_map[2] = {1, 6};    // map binary values to 1 of 8 levels

      int i = 0;

      p[i++] = bin_map[1];    // data segment sync pulse
      p[i++] = bin_map[0];
      p[i++] = bin_map[0];
      p[i++] = bin_map[1];

      for (int j = 0; j < 511; j++) {    // PN511
        p[i++] = bin_map[atsc_pn511[j]];
      }

      for (int j = 0; j < 63; j++) {     // PN63
        p[i++] = bin_map[atsc_pn63[j]];
      }

      for (int j = 0; j < 63; j++) {     // PN63, toggled on field 2
        p[i++] = bin_map[atsc_pn63[j] ^ mask];
      }

      for (int j = 0; j < 63; j++) {     // PN63
        p[i++] = bin_map[atsc_pn63[j]];
      }

      p[i++] = bin_map[0];    // 24 bits of 8VSB mode identifier
      p[i++] = bin_map[0];
      p[i++] = bin_map[0];
      p[i++] = bin_map[0];

      p[i++] = bin_map[1];
      p[i++] = bin_map[0];
      p[i++] = bin_map[1];
      p[i++] = bin_map[0];

      p[i++] = bin_map[0];
      p[i++] = bin_map[1];
      p[i++] = bin_map[0];
      p[i++] = bin_map[1];

      p[i++] = bin_map[1];
      p[i++] = bin_map[1];
      p[i++] = bin_map[1];
      p[i++] = bin_map[1];

      p[i++] = bin_map[0];
      p[i++] = bin_map[1];
      p[i++] = bin_map[0];
      p[i++] = bin_map[1];

      p[i++] = bin_map[1];
      p[i++] = bin_map[0];
      p[i++] = bin_map[1];
      p[i++] = bin_map[0];

      for (int j = 0; j < 92; j++) {    // 92 more bits
        p[i++] = bin_map[atsc_pn63[j % 63]];
      }

      // now copy the last 12 symbols of the previous segment

      for (int j = 0; j < N_SAVED_SYMBOLS; j++) {
        p[i++] = saved_symbols[j];
      }

      assert(i == ATSC_DATA_SEGMENT_LENGTH);
    }

    inline void
    atsc_field_sync_mux_impl::init_field_sync_1(atsc_data_segment *s,
                                                const unsigned char saved_symbols[N_SAVED_SYMBOLS])
    {
      init_field_sync_common(&s->data[0], 0, saved_symbols);
    }

    inline void
    atsc_field_sync_mux_impl::init_field_sync_2(atsc_data_segment *s,
                                                const unsigned char saved_symbols[N_SAVED_SYMBOLS])
    {
      init_field_sync_common(&s->data[0], 1, saved_symbols);
    }

    void
    atsc_field_sync_mux_impl::save_last_symbols(unsigned char saved_symbols[N_SAVED_SYMBOLS],
                                                const atsc_data_segment &seg)
    {
        for (int i = 0; i < N_SAVED_SYMBOLS; i++) {
          saved_symbols[i] = seg.data[i + ATSC_DATA_SEGMENT_LENGTH - N_SAVED_SYMBOLS];
        }
    }

    inline bool
    atsc_field_sync_mux_impl::last_regular_seg_p(const plinfo &pli)
    {
      return pli.regular_seg_p() && (pli.segno() == ATSC_DSEGS_PER_FIELD - 1);
    }

    void
    atsc_field_sync_mux_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = noutput_items;
    }

    int
    atsc_field_sync_mux_impl::general_work (int noutput_items,
                                            gr_vector_int &ninput_items,
                                            gr_vector_const_void_star &input_items,
                                            gr_vector_void_star &output_items)
    {
      const atsc_data_segment *in = (const atsc_data_segment *) input_items[0];
      atsc_data_segment *out = (atsc_data_segment *) output_items[0];
      int in_length = ninput_items[0];
      int index = 0;
      int outdex = 0;

      for (outdex = 0; outdex < noutput_items && index < in_length; outdex++) {
        assert(in[index].pli.regular_seg_p());

        if (!in[index].pli.first_regular_seg_p()) {
          out[outdex] = in[index];    // just copy in to out

          if (last_regular_seg_p(in[index].pli)) {
            save_last_symbols(d_saved_symbols, in[index]);
          }
          index++;
        }
        else {   // first_regular_seg_p
          if (!d_already_output_field_sync) {
            // write out field sync...
            atsc_data_segment field_sync;

            field_sync.pli._flags = 0;
            field_sync.pli._segno = 0;
            memset(field_sync._pad_, 0, atsc_data_segment::NPAD);

            if (in[index].pli.in_field1_p()) {
              init_field_sync_1(&field_sync, d_saved_symbols);
            }
            else {
              init_field_sync_2(&field_sync, d_saved_symbols);
            }

            // note that index doesn't advance in this branch
            out[outdex] = field_sync;
            d_already_output_field_sync = true;
          }
          else {
            // already output field sync, now output first regular segment
            out[outdex] = in[index];
            index++;
            d_already_output_field_sync = false;
          }
        }
      }

      // Tell runtime system how many input items we consumed on
      // each input stream.
      consume_each (index);

      // Tell runtime system how many output items we produced.
      return outdex;
    }

  } /* namespace dtv */
} /* namespace gr */

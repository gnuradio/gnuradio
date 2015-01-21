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
#include "atsc_rs_encoder_impl.h"

namespace gr {
  namespace dtv {

    static const int rs_init_symsize =     8;
    static const int rs_init_gfpoly  = 0x11d;
    static const int rs_init_fcr     =     0;    // first consecutive root
    static const int rs_init_prim    =     1;    // primitive is 1 (alpha)
    static const int rs_init_nroots  =    20;

    static const int N = (1 << rs_init_symsize) - 1;    // 255
    static const int K = N - rs_init_nroots;            // 235

    static const int amount_of_pad = N - ATSC_MPEG_RS_ENCODED_LENGTH;    // 48

    atsc_rs_encoder::sptr
    atsc_rs_encoder::make()
    {
      return gnuradio::get_initial_sptr
        (new atsc_rs_encoder_impl());
    }

    atsc_rs_encoder_impl::atsc_rs_encoder_impl()
      : gr::sync_block("atsc_rs_encoder",
                       gr::io_signature::make(1, 1, sizeof(atsc_mpeg_packet_no_sync)),
                       gr::io_signature::make(1, 1, sizeof(atsc_mpeg_packet_rs_encoded)))
    {
      d_rs = init_rs_char(rs_init_symsize, rs_init_gfpoly, rs_init_fcr, rs_init_prim, rs_init_nroots);
      assert (d_rs != 0);
    }

    atsc_rs_encoder_impl::~atsc_rs_encoder_impl()
    {
      if (d_rs) {
        free_rs_char(d_rs);
      }

      d_rs = 0;
    }

    void
    atsc_rs_encoder_impl::encode(atsc_mpeg_packet_rs_encoded &out, const atsc_mpeg_packet_no_sync &in)
    {
      unsigned char tmp[K];

      assert((int)(amount_of_pad + sizeof (in.data)) == K);

      // add missing prefix zero padding to message
      memset(tmp, 0, amount_of_pad);
      memcpy(&tmp[amount_of_pad], in.data, sizeof (in.data));

      // copy message portion to output packet
      memcpy(out.data, in.data, sizeof (in.data));

      // now compute parity bytes and add them to tail end of output packet
      encode_rs_char(d_rs, tmp, &out.data[sizeof (in.data)]);
    }

    int
    atsc_rs_encoder_impl::work(int noutput_items,
                               gr_vector_const_void_star &input_items,
                               gr_vector_void_star &output_items)
    {
      const atsc_mpeg_packet_no_sync *in = (const atsc_mpeg_packet_no_sync *) input_items[0];
      atsc_mpeg_packet_rs_encoded *out = (atsc_mpeg_packet_rs_encoded *) output_items[0];

      for (int i = 0; i < noutput_items; i++) {
        assert(in[i].pli.regular_seg_p());
        out[i].pli = in[i].pli;                // copy pipeline info...
        encode(out[i], in[i]);
      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace dtv */
} /* namespace gr */

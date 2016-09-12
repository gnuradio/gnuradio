/* -*- c++ -*- */
/* 
 * Copyright 2015,2016 Free Software Foundation, Inc.
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
#include "dvbt_reed_solomon_dec_impl.h"

namespace gr {
  namespace dtv {

    static const int rs_init_symsize =     8;
    static const int rs_init_fcr     =     0;	// first consecutive root
    static const int rs_init_prim    =     1;	// primitive is 1 (alpha)
    static const int N = (1 << rs_init_symsize) - 1;	// 255

    dvbt_reed_solomon_dec::sptr
    dvbt_reed_solomon_dec::make(int p, int m, int gfpoly, int n, int k, int t, int s, int blocks)
    {
      return gnuradio::get_initial_sptr
        (new dvbt_reed_solomon_dec_impl(p, m, gfpoly, n, k, t, s, blocks));
    }

    /*
     * The private constructor
     */
    dvbt_reed_solomon_dec_impl::dvbt_reed_solomon_dec_impl(int p, int m, int gfpoly, int n, int k, int t, int s, int blocks)
      : block("dvbt_reed_solomon_dec",
          io_signature::make(1, 1, sizeof(unsigned char) * blocks * (n - s)),
          io_signature::make(1, 1, sizeof(unsigned char) * blocks * (k - s))),
      d_n(n), d_k(k), d_s(s), d_blocks(blocks)
    {
      d_rs = init_rs_char(rs_init_symsize, gfpoly, rs_init_fcr, rs_init_prim, (n - k));
      if (d_rs == NULL) {
        GR_LOG_FATAL(d_logger, "Reed-Solomon Decoder, cannot allocate memory for d_rs.");
        throw std::bad_alloc();
      }
      d_nerrors_corrected_count = 0;
      d_bad_packet_count = 0;
      d_total_packets = 0;
    }

    /*
     * Our virtual destructor.
     */
    dvbt_reed_solomon_dec_impl::~dvbt_reed_solomon_dec_impl()
    {
      free_rs_char(d_rs);
    }

    void
    dvbt_reed_solomon_dec_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = noutput_items;
    }

    int
    dvbt_reed_solomon_dec_impl::decode (unsigned char &out, const unsigned char &in)
    {
      unsigned char tmp[N];
      int ncorrections;

      // add missing prefix zero padding to message
      memset(tmp, 0, d_s);
      memcpy(&tmp[d_s], &in, (d_n - d_s));

      // correct message...
      ncorrections = decode_rs_char(d_rs, tmp, 0, 0);

      // copy corrected message to output, skipping prefix zero padding
      memcpy (&out, &tmp[d_s], (d_k - d_s));

      return ncorrections;
    }

    int
    dvbt_reed_solomon_dec_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char *) input_items[0];
      unsigned char *out = (unsigned char *) output_items[0];
      int j = 0;
      int k = 0;

      for (int i = 0; i < (d_blocks * noutput_items); i++) {
        int nerrors_corrected = decode(out[k], in[j]);

        if (nerrors_corrected == -1) {
          d_bad_packet_count++;
          d_nerrors_corrected_count += ((d_n - d_s) - (d_k - d_s)) / 2; // lower bound estimate; most this RS can fix
        }
        else {
          d_nerrors_corrected_count += nerrors_corrected;
        }

	d_total_packets++;
        j += (d_n - d_s);
        k += (d_k - d_s);
      }

      // Tell runtime system how many input items we consumed on
      // each input stream.
      consume_each (noutput_items);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace dtv */
} /* namespace gr */


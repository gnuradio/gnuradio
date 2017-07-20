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
#include "dvbt_reed_solomon_enc_impl.h"

namespace gr {
  namespace dtv {

    static const int rs_init_symsize =     8;
    static const int rs_init_fcr     =     0;    // first consecutive root
    static const int rs_init_prim    =     1;    // primitive is 1 (alpha)

    dvbt_reed_solomon_enc::sptr
    dvbt_reed_solomon_enc::make(int p, int m, int gfpoly, int n, int k, int t, int s, int blocks)
    {
      return gnuradio::get_initial_sptr
        (new dvbt_reed_solomon_enc_impl(p, m, gfpoly, n, k, t, s, blocks));
    }

    /*
     * The private constructor
     */
    dvbt_reed_solomon_enc_impl::dvbt_reed_solomon_enc_impl(int p, int m, int gfpoly, int n, int k, int t, int s, int blocks)
      : block("dvbt_reed_solomon",
          io_signature::make(1, 1, sizeof(unsigned char) * blocks * (k - s)),
          io_signature::make(1, 1, sizeof(unsigned char) * blocks * (n - s))),
      d_n(n), d_k(k), d_s(s), d_blocks(blocks)
    {
      d_rs = init_rs_char(rs_init_symsize, gfpoly, rs_init_fcr, rs_init_prim, (n - k));
      if (d_rs == NULL) {
        GR_LOG_FATAL(d_logger, "Reed-Solomon Encoder, cannot allocate memory for d_rs.");
        throw std::bad_alloc();
      }
      // The full input frame size (d_k) (no need to add in d_s, as the block input is the pre-shortedned K)
      d_data = (unsigned char *) malloc(sizeof(unsigned char) * (d_k));
      if (d_data == NULL) {
        GR_LOG_FATAL(d_logger, "Reed-Solomon Encoder, cannot allocate memory for d_data.");
        free_rs_char(d_rs);
        throw std::bad_alloc();
      }
    }

    /*
     * Our virtual destructor.
     */
    dvbt_reed_solomon_enc_impl::~dvbt_reed_solomon_enc_impl()
    {
      free(d_data);
      free_rs_char(d_rs);
    }

    void
    dvbt_reed_solomon_enc_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = noutput_items;
    }

    void
    dvbt_reed_solomon_enc_impl::encode(const unsigned char *in, unsigned char *out)
    {
      // Shortened Reed-Solomon: prepend zero bytes to message (discarded after encoding)
      std::memset(d_data, 0, d_s);
      // This is the number of data bytes we need from the input stream.
      int shortened_k = d_k-d_s;
      std::memcpy(&d_data[d_s], in, shortened_k);

      // Copy input message to output then append Reed-Solomon bits
      std::memcpy(out, in, shortened_k);
      encode_rs_char(d_rs, d_data, &out[shortened_k]);
    }

    int
    dvbt_reed_solomon_enc_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char *) input_items[0];
      unsigned char *out = (unsigned char *) output_items[0];
      int j = 0;
      int k = 0;

      for (int i = 0; i < noutput_items * d_blocks; i++) {
        encode(in + j, out + k);
        j += (d_k - d_s);
        k += (d_n - d_s);
      }

      // Tell runtime system how many input items we consumed on
      // each input stream.
      consume_each (noutput_items);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace dtv */
} /* namespace gr */


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
#include "catv_reed_solomon_enc_bb_impl.h"

namespace gr {
  namespace dtv {

    catv_reed_solomon_enc_bb::sptr
    catv_reed_solomon_enc_bb::make()
    {
      return gnuradio::get_initial_sptr
        (new catv_reed_solomon_enc_bb_impl());
    }

    /*
     * The private constructor
     */
    catv_reed_solomon_enc_bb_impl::catv_reed_solomon_enc_bb_impl()
      : gr::block("catv_reed_solomon_enc_bb",
              gr::io_signature::make(1, 1, sizeof(unsigned char)),
              gr::io_signature::make(1, 1, sizeof(unsigned char)))
    {
      set_output_multiple(128);
      init_rs();
    }

    /*
     * Our virtual destructor.
     */
    catv_reed_solomon_enc_bb_impl::~catv_reed_solomon_enc_bb_impl()
    {
    }

    void
    catv_reed_solomon_enc_bb_impl::init_rs()
    {
      unsigned char x;
      int i, j;

      gf_exp[0] = 1;
      gf_log[1] = 0;

      x = 1;
      for (i = 1; i < 127; i++) {
        x <<= 1;
        if (x & 0x80) {
          x = (x & 0x7F) ^ 0x09;
        }
        gf_exp[i] = x;
        gf_log[x] = i;
      }
      for (; i < 256; i++) {
        gf_exp[i] = gf_exp[i - 127];
      }

      for (i = 0; i < 128; i++) {
        for (j = 0; j < 128; j++) {
          if ((i == 0) || (j == 0)) {
            gf_mul_table[i][j] = 0;
          } else {
            gf_mul_table[i][j] = gf_exp[gf_log[i] + gf_log[j]];
          }
        }
      }
    }

    unsigned char
    catv_reed_solomon_enc_bb_impl::gf_poly_eval(unsigned char *p, int len, unsigned char x)
    {
      unsigned char y = p[0];
      int i;

      for (i = 1; i < len; i++) {
        y = gf_mul_table[y][x] ^ p[i];
      }
      return y;
    }

    void
    catv_reed_solomon_enc_bb_impl::reed_solomon_enc(const unsigned char *message, unsigned char *output)
    {
      // Generator polynomial from p.7 of ANSI/SCTE 07 2013
      unsigned char g[] = {1, gf_exp[52], gf_exp[116], gf_exp[119], gf_exp[61], gf_exp[15]};
      int i, j;

      memcpy(output, message, 122);
      memset(output + 122, 0, 6);

      for (i = 0; i < 122; i++) {
        for (j = 1; j < 6; j++) {
          output[i + j] ^= gf_mul_table[output[i]][g[j]];
        }
        output[i] = message[i];
      }

      output[127] = gf_poly_eval(output, 127, gf_exp[6]);
    }

    void
    catv_reed_solomon_enc_bb_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = (noutput_items / 128) * 122;
    }

    int
    catv_reed_solomon_enc_bb_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char *) input_items[0];
      unsigned char *out = (unsigned char *) output_items[0];
      int j = 0;

      for (int i = 0; i < noutput_items; i += 128) {
        reed_solomon_enc(in + j, out + i);
        j += 122;
      }

      // Tell runtime system how many input items we consumed on
      // each input stream.
      consume_each (j);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace dtv */
} /* namespace gr */


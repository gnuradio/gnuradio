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
#include "catv_trellis_enc_bb_impl.h"

namespace gr {
  namespace dtv {

    catv_trellis_enc_bb::sptr
    catv_trellis_enc_bb::make()
    {
      return gnuradio::get_initial_sptr
        (new catv_trellis_enc_bb_impl());
    }

    /*
     * The private constructor
     */
    catv_trellis_enc_bb_impl::catv_trellis_enc_bb_impl()
      : gr::block("catv_trellis_enc_bb",
              gr::io_signature::make(1, 1, sizeof(unsigned char)),
              gr::io_signature::make(1, 1, sizeof(unsigned char)))
    {
      set_output_multiple(5);

      init_trellis();

      Xq = 0;
      Yq = 0;
      XYp = 0;
    }

    /*
     * Our virtual destructor.
     */
    catv_trellis_enc_bb_impl::~catv_trellis_enc_bb_impl()
    {
    }

    void
    catv_trellis_enc_bb_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = noutput_items / 5 * 4;
    }

    void
    catv_trellis_enc_bb_impl::diff_precoder(unsigned char W, unsigned char Z, unsigned char *Xp, unsigned char *Yp)
    {
      unsigned char common, newX, newY;

      common = (Z & (*Xp ^ *Yp));
      newX = W ^ *Xp ^ common;
      newY = Z ^ W ^ *Yp ^ common;

      *Xp = newX;
      *Yp = newY;
    }

    void
    catv_trellis_enc_bb_impl::init_trellis()
    {
      unsigned char XYp, W, Z, X, Y, Xp, Yp, state, xy, Xq;
      int i, n;

      for (XYp = 0; XYp < 4; XYp++) {
        for (W = 0; W < 16; W++) {
          for (Z = 0; Z < 16; Z++) {
            X = 0;
            Y = 0;
            Xp = (XYp & 0x02) >> 1;
            Yp = (XYp & 0x01);
            for (i = 0; i < 4; i++) {
              diff_precoder((W >> i) & 1, (Z >> i) & 1, &Xp, &Yp);
              X |= (Xp << i);
              Y |= (Yp << i);
            }
            diff_precoder_table[XYp][W][Z][0] = (Xp << 1) + Yp;
            diff_precoder_table[XYp][W][Z][1] = X;
            diff_precoder_table[XYp][W][Z][2] = Y;
          }
        }
      }

      for (i = 0; i < 32; i++) {
        G1table[i] = (i >> 4) ^ ((i & 0x04) >> 2) ^ (i & 1);
        G2table[i] = (i >> 4) ^ ((i & 0x08) >> 3) ^ ((i & 0x04) >> 2) ^ ((i & 0x02) >> 1) ^ (i & 1);
      }

      memset(trellis_table_x, 0, 16*16*6);
      memset(trellis_table_y, 0, 16*16*6);
      for (state = 0; state < 16; state++) {
        for (xy = 0; xy < 16; xy++) {
          i = 0;
          Xq = state;
          for (n = 0; n < 4; n++) {
            Xq = (Xq << 1) + ((xy >> n) & 1);

            if (n == 3) {
              trellis_table_x[state][xy][i+1] |= G1table[Xq] << 3;
              trellis_table_y[state][xy][i+1] |= G1table[Xq];
              i += 1;
            }
            trellis_table_x[state][xy][i+1] |= G2table[Xq] << 3;
            trellis_table_y[state][xy][i+1] |= G2table[Xq];
            i += 1;

            Xq &= 0x0F;
          }

          trellis_table_x[state][xy][0] = Xq;
          trellis_table_y[state][xy][0] = Xq;
        }
      }
    }

    void
    catv_trellis_enc_bb_impl::trellis_code(const unsigned char *rs, unsigned char *qs)
    {
      unsigned char X, Y;
      int A, B, n;

      A = (rs[1] << 7) | rs[0];
      B = (rs[3] << 7) | rs[2];

      memset(qs, 0, 5);

      for (n = 0; n < 5; n++) {
        qs[n] |= ((A >> (2*n)) & 3) << 4;
        qs[n] |= ((B >> (2*n)) & 3) << 1;
      }

      X = diff_precoder_table[XYp][A >> 10][B >> 10][1];
      Y = diff_precoder_table[XYp][A >> 10][B >> 10][2];
      XYp = diff_precoder_table[XYp][A >> 10][B >> 10][0];

      for (n = 0; n < 5; n++) {
        qs[n] |= trellis_table_x[Xq][X][1+n];
        qs[n] |= trellis_table_y[Yq][Y][1+n];
      }
      Xq = trellis_table_x[Xq][X][0];
      Yq = trellis_table_y[Yq][Y][0];
    }

    int
    catv_trellis_enc_bb_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char *) input_items[0];
      unsigned char *out = (unsigned char *) output_items[0];

      int i = 0, j = 0;

      while (i < noutput_items) {
        trellis_code(in + j, out + i);
        i += 5;
        j += 4;
      }

      // Tell runtime system how many input items we consumed on
      // each input stream.
      consume_each (j);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace dtv */
} /* namespace gr */


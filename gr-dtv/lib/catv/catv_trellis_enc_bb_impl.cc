/* -*- c++ -*- */
/* 
 * Copyright 2016,2017 Free Software Foundation, Inc.
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
    catv_trellis_enc_bb::make(catv_constellation_t constellation)
    {
      return gnuradio::get_initial_sptr
        (new catv_trellis_enc_bb_impl(constellation));
    }

    /*
     * The private constructor
     */
    catv_trellis_enc_bb_impl::catv_trellis_enc_bb_impl(catv_constellation_t constellation)
      : gr::block("catv_trellis_enc_bb",
              gr::io_signature::make(1, 1, sizeof(unsigned char)),
              gr::io_signature::make(1, 1, sizeof(unsigned char)))
    {
      if (constellation == CATV_MOD_64QAM) {
        set_output_multiple(5);
      }
      else {
        set_output_multiple(5 * 6);
      }

      init_trellis();

      Xq = 0;
      Yq = 0;
      XYp = 0;
      signal_constellation = constellation;
      trellis_group = 0;
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
      if (signal_constellation == CATV_MOD_64QAM) {
        ninput_items_required[0] = noutput_items / 5 * 28;
      }
      else {
        ninput_items_required[0] = (noutput_items / (5 * 6)) * (38 * 6);
      }
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
    catv_trellis_enc_bb_impl::trellis_code_64qam(const unsigned char *rs, unsigned char *qs)
    {
      unsigned char X, Y;
      int A, B, n;

      memset(qs, 0, 5);

      qs[0] |= rs[6]  << 4;  /* A1 */
      qs[0] |= rs[5]  << 5;  /* A2 */
      qs[0] |= rs[20] << 1;  /* B1 */
      qs[0] |= rs[19] << 2;  /* B2 */
      qs[1] |= rs[4]  << 4;  /* A4 */
      qs[1] |= rs[3]  << 5;  /* A5 */
      qs[1] |= rs[18] << 1;  /* B4 */
      qs[1] |= rs[17] << 2;  /* B5 */
      qs[2] |= rs[2]  << 4;  /* A7 */
      qs[2] |= rs[1]  << 5;  /* A8 */
      qs[2] |= rs[16] << 1;  /* B7 */
      qs[2] |= rs[15] << 2;  /* B8 */
      qs[3] |= rs[0]  << 4;  /* A10 */
      qs[3] |= rs[13] << 5;  /* A11 */
      qs[3] |= rs[14] << 1;  /* B10 */
      qs[3] |= rs[27] << 2;  /* B11 */
      qs[4] |= rs[12] << 4;  /* A12 */
      qs[4] |= rs[11] << 5;  /* A13 */
      qs[4] |= rs[26] << 1;  /* B12 */
      qs[4] |= rs[25] << 2;  /* B13 */

      A = (rs[7] << 3) | (rs[8] << 2) | (rs[9] << 1) | rs[10];
      B = (rs[21] << 3) | (rs[22] << 2) | (rs[23] << 1) | rs[24];
      X = diff_precoder_table[XYp][A][B][1];
      Y = diff_precoder_table[XYp][A][B][2];
      XYp = diff_precoder_table[XYp][A][B][0];

      for (n = 0; n < 5; n++) {
        qs[n] |= trellis_table_x[Xq][X][1+n];
        qs[n] |= trellis_table_y[Yq][Y][1+n];
      }
      Xq = trellis_table_x[Xq][X][0];
      Yq = trellis_table_y[Yq][Y][0];
    }

    void
    catv_trellis_enc_bb_impl::trellis_code_256qam(const unsigned char *rs, unsigned char *qs)
    {
      unsigned char X, Y;
      int A, B, n;

      for (int i = 0; i < 6; i++) {
        memset(&qs[0 + (i * 5)], 0, 5);
        if (trellis_group == 2071) {
          for (int j = 0; j < 5; j++) {
            qs[j + (i * 5)] |= rs[38 + (j * 6)] << 5;
            qs[j + (i * 5)] |= rs[39 + (j * 6)] << 6;
            qs[j + (i * 5)] |= rs[40 + (j * 6)] << 7;
            qs[j + (i * 5)] |= rs[41 + (j * 6)] << 1;
            qs[j + (i * 5)] |= rs[42 + (j * 6)] << 2;
            qs[j + (i * 5)] |= rs[43 + (j * 6)] << 3;
          }

          A = (rs[194] << 3) | (rs[192] << 2) | (rs[190] << 1) | rs[188];
          B = (rs[195] << 3) | (rs[193] << 2) | (rs[191] << 1) | rs[189];
        }
        else if (trellis_group == 2072) {
          for (int j = 0; j < 5; j++) {
            qs[j + (i * 5)] |= rs[68 + (j * 6)] << 5;
            qs[j + (i * 5)] |= rs[69 + (j * 6)] << 6;
            qs[j + (i * 5)] |= rs[70 + (j * 6)] << 7;
            qs[j + (i * 5)] |= rs[71 + (j * 6)] << 1;
            qs[j + (i * 5)] |= rs[72 + (j * 6)] << 2;
            qs[j + (i * 5)] |= rs[73 + (j * 6)] << 3;
          }

          A = (rs[202] << 3) | (rs[200] << 2) | (rs[198] << 1) | rs[196];
          B = (rs[203] << 3) | (rs[201] << 2) | (rs[199] << 1) | rs[197];
        }
        else if (trellis_group == 2073) {
          for (int j = 0; j < 5; j++) {
            qs[j + (i * 5)] |= rs[98 + (j * 6)] << 5;
            qs[j + (i * 5)] |= rs[99 + (j * 6)] << 6;
            qs[j + (i * 5)] |= rs[100 + (j * 6)] << 7;
            qs[j + (i * 5)] |= rs[101 + (j * 6)] << 1;
            qs[j + (i * 5)] |= rs[102 + (j * 6)] << 2;
            qs[j + (i * 5)] |= rs[103 + (j * 6)] << 3;
          }

          A = (rs[210] << 3) | (rs[208] << 2) | (rs[206] << 1) | rs[204];
          B = (rs[211] << 3) | (rs[209] << 2) | (rs[207] << 1) | rs[205];
        }
        else if (trellis_group == 2074) {
          for (int j = 0; j < 5; j++) {
            qs[j + (i * 5)] |= rs[128 + (j * 6)] << 5;
            qs[j + (i * 5)] |= rs[129 + (j * 6)] << 6;
            qs[j + (i * 5)] |= rs[130 + (j * 6)] << 7;
            qs[j + (i * 5)] |= rs[131 + (j * 6)] << 1;
            qs[j + (i * 5)] |= rs[132 + (j * 6)] << 2;
            qs[j + (i * 5)] |= rs[133 + (j * 6)] << 3;
          }

          A = (rs[218] << 3) | (rs[216] << 2) | (rs[214] << 1) | rs[212];
          B = (rs[219] << 3) | (rs[217] << 2) | (rs[215] << 1) | rs[213];
        }
        else if (trellis_group == 2075) {
          for (int j = 0; j < 5; j++) {
            qs[j + (i * 5)] |= rs[158 + (j * 6)] << 5;
            qs[j + (i * 5)] |= rs[159 + (j * 6)] << 6;
            qs[j + (i * 5)] |= rs[160 + (j * 6)] << 7;
            qs[j + (i * 5)] |= rs[161 + (j * 6)] << 1;
            qs[j + (i * 5)] |= rs[162 + (j * 6)] << 2;
            qs[j + (i * 5)] |= rs[163 + (j * 6)] << 3;
          }

          A = (rs[226] << 3) | (rs[224] << 2) | (rs[222] << 1) | rs[220];
          B = (rs[227] << 3) | (rs[225] << 2) | (rs[223] << 1) | rs[221];
        }
        else {
          qs[0 + (i * 5)] |= rs[2 + (i * 38)]  << 5; /* A1 */
          qs[0 + (i * 5)] |= rs[3 + (i * 38)]  << 6; /* A2 */
          qs[0 + (i * 5)] |= rs[4 + (i * 38)]  << 7; /* A3 */
          qs[0 + (i * 5)] |= rs[5 + (i * 38)]  << 1; /* B1 */
          qs[0 + (i * 5)] |= rs[6 + (i * 38)]  << 2; /* B2 */
          qs[0 + (i * 5)] |= rs[7 + (i * 38)]  << 3; /* B3 */
          qs[1 + (i * 5)] |= rs[10 + (i * 38)] << 5; /* A5 */
          qs[1 + (i * 5)] |= rs[11 + (i * 38)] << 6; /* A6 */
          qs[1 + (i * 5)] |= rs[12 + (i * 38)] << 7; /* A7 */
          qs[1 + (i * 5)] |= rs[13 + (i * 38)] << 1; /* B5 */
          qs[1 + (i * 5)] |= rs[14 + (i * 38)] << 2; /* B6 */
          qs[1 + (i * 5)] |= rs[15 + (i * 38)] << 3; /* B7 */
          qs[2 + (i * 5)] |= rs[18 + (i * 38)] << 5; /* A9 */
          qs[2 + (i * 5)] |= rs[19 + (i * 38)] << 6; /* A10 */
          qs[2 + (i * 5)] |= rs[20 + (i * 38)] << 7; /* A11 */
          qs[2 + (i * 5)] |= rs[21 + (i * 38)] << 1; /* B9 */
          qs[2 + (i * 5)] |= rs[22 + (i * 38)] << 2; /* B10 */
          qs[2 + (i * 5)] |= rs[23 + (i * 38)] << 3; /* B11 */
          qs[3 + (i * 5)] |= rs[26 + (i * 38)] << 5; /* A13 */
          qs[3 + (i * 5)] |= rs[27 + (i * 38)] << 6; /* A14 */
          qs[3 + (i * 5)] |= rs[28 + (i * 38)] << 7; /* A15 */
          qs[3 + (i * 5)] |= rs[29 + (i * 38)] << 1; /* B13 */
          qs[3 + (i * 5)] |= rs[30 + (i * 38)] << 2; /* B14 */
          qs[3 + (i * 5)] |= rs[31 + (i * 38)] << 3; /* B15 */
          qs[4 + (i * 5)] |= rs[32 + (i * 38)] << 5; /* A16 */
          qs[4 + (i * 5)] |= rs[33 + (i * 38)] << 6; /* A17 */
          qs[4 + (i * 5)] |= rs[34 + (i * 38)] << 7; /* A18 */
          qs[4 + (i * 5)] |= rs[35 + (i * 38)] << 1; /* B16 */
          qs[4 + (i * 5)] |= rs[36 + (i * 38)] << 2; /* B17 */
          qs[4 + (i * 5)] |= rs[37 + (i * 38)] << 3; /* B18 */

          A = (rs[24 + (i * 38)] << 3) | (rs[16 + (i * 38)] << 2) | (rs[8 + (i * 38)] << 1) | rs[0 + (i * 38)];
          B = (rs[25 + (i * 38)] << 3) | (rs[17 + (i * 38)] << 2) | (rs[9 + (i * 38)] << 1) | rs[1 + (i * 38)];
        }

        X = diff_precoder_table[XYp][A][B][1];
        Y = diff_precoder_table[XYp][A][B][2];
        XYp = diff_precoder_table[XYp][A][B][0];

        for (n = 0; n < 5; n++) {
          qs[n + (i * 5)] |= trellis_table_x[Xq][X][1+n] << 1;
          qs[n + (i * 5)] |= trellis_table_y[Yq][Y][1+n];
        }
        Xq = trellis_table_x[Xq][X][0];
        Yq = trellis_table_y[Yq][Y][0];
        trellis_group++;
        if (trellis_group == 2076) {
          trellis_group = 0;
        }
      }
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
        if (signal_constellation == CATV_MOD_64QAM) {
          trellis_code_64qam(in + j, out + i);
          i += 5;
          j += 28;
        }
        else {
          trellis_code_256qam(in + j, out + i);
          i += 5 * 6;
          j += 38 * 6;
        }
      }

      // Tell runtime system how many input items we consumed on
      // each input stream.
      consume_each (j);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace dtv */
} /* namespace gr */


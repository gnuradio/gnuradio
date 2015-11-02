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

#ifndef INCLUDED_DTV_DVBT_REED_SOLOMON_H
#define INCLUDED_DTV_DVBT_REED_SOLOMON_H

namespace gr {
  namespace dtv {

    class dvbt_reed_solomon
    {
     private:
      int d_p;
      int d_m;
      int d_gfpoly;
      int d_n;
      int d_k;
      int d_t;
      int d_s;
      int d_blocks;
      unsigned char *d_gf_exp;
      unsigned char *d_gf_log;
      unsigned char *d_l;
      unsigned char *d_g;

      unsigned char *d_syn;

      int gf_add(int a, int b);
      int gf_mul(int a, int b);
      int gf_div(int a, int b);
      int gf_exp(int a);
      int gf_log(int a);
      int gf_pow(int a, int power);
      int gf_lpow(int power);

      void gf_init(int p, int m, int gfpoly);
      void gf_uninit();
      void rs_init(int lambda, int n, int k, int t);
      void rs_uninit();

     public:
      int rs_encode(unsigned char *data, unsigned char *parity);
      int rs_decode(unsigned char *data, unsigned char *eras, const int no_eras);

      dvbt_reed_solomon(int p, int m, int gfpoly, int n, int k, int t, int s, int blocks);
      ~dvbt_reed_solomon();
    };

  } // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT_REED_SOLOMON_H */


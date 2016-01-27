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

#ifndef INCLUDED_DTV_CATV_REED_SOLOMON_ENC_BB_IMPL_H
#define INCLUDED_DTV_CATV_REED_SOLOMON_ENC_BB_IMPL_H

#include <gnuradio/dtv/catv_reed_solomon_enc_bb.h>

namespace gr {
  namespace dtv {

    class catv_reed_solomon_enc_bb_impl : public catv_reed_solomon_enc_bb
    {
     private:
      unsigned char gf_mul_table[128][128];
      unsigned char gf_exp[256];
      unsigned char gf_log[128];

      void init_rs();
      unsigned char gf_poly_eval(unsigned char *p, int len, unsigned char x);
      void reed_solomon_enc(const unsigned char *message, unsigned char *output);

     public:
      catv_reed_solomon_enc_bb_impl();
      ~catv_reed_solomon_enc_bb_impl();

      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items);
    };

  } // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_CATV_REED_SOLOMON_ENC_BB_IMPL_H */


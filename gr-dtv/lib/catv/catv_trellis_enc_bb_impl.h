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

#ifndef INCLUDED_DTV_CATV_TRELLIS_ENC_BB_IMPL_H
#define INCLUDED_DTV_CATV_TRELLIS_ENC_BB_IMPL_H

#include <gnuradio/dtv/catv_trellis_enc_bb.h>

namespace gr {
  namespace dtv {

    class catv_trellis_enc_bb_impl : public catv_trellis_enc_bb
    {
     private:
      unsigned char diff_precoder_table[4][16][16][3];
      unsigned char G1table[32];
      unsigned char G2table[32];
      unsigned char trellis_table_x[16][16][6];
      unsigned char trellis_table_y[16][16][6];
      unsigned char Xq, Yq, XYp;
      int signal_constellation;
      int trellis_group;

      void diff_precoder(unsigned char W, unsigned char Z, unsigned char *Xp, unsigned char *Yp);
      void init_trellis();
      void trellis_code_64qam(const unsigned char *rs, unsigned char *qs);
      void trellis_code_256qam(const unsigned char *rs, unsigned char *qs);

     public:
      catv_trellis_enc_bb_impl(catv_constellation_t constellation);
      ~catv_trellis_enc_bb_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items);
    };

  } // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_CATV_TRELLIS_ENC_BB_IMPL_H */


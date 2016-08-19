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

#ifndef INCLUDED_DTV_DVBT_INNER_CODER_IMPL_H
#define INCLUDED_DTV_DVBT_INNER_CODER_IMPL_H

#include <gnuradio/dtv/dvbt_inner_coder.h>
#include "dvbt_configure.h"

namespace gr {
  namespace dtv {

    class dvbt_inner_coder_impl : public dvbt_inner_coder
    {
     private:
      const dvbt_configure config;

      static const int d_lookup_171[128];
      static const int d_lookup_133[128];

      int d_ninput;
      int d_noutput;

      int d_reg;

      //counts the bits in the bytes
      //in input stream
      int d_bitcount;

      // Code rate k/n
      int d_k;
      int d_n;
      // Constellation with m
      int d_m;

      // input block size in bytes
      int d_in_bs;
      // bit input buffer
      unsigned char * d_in_buff;

      // output block size in bytes
      int d_out_bs;
      // bit output buffer
      unsigned char * d_out_buff;

      inline void generate_codeword(unsigned char in, int &x, int &y);
      inline void generate_punctured_code(dvb_code_rate_t coderate, unsigned char * in, unsigned char * out);

     public:
      dvbt_inner_coder_impl(int ninput, int noutput, dvb_constellation_t constellation, dvbt_hierarchy_t hierarchy, dvb_code_rate_t coderate);
      ~dvbt_inner_coder_impl();
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work (int noutput_items,
                        gr_vector_int &ninput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items);
    };

  } // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT_INNER_CODER_IMPL_H */


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

#ifndef INCLUDED_DTV_DVBS2_PHYSICAL_CC_IMPL_H
#define INCLUDED_DTV_DVBS2_PHYSICAL_CC_IMPL_H

#include <gnuradio/dtv/dvbs2_physical_cc.h>
#include "dvb/dvb_defines.h"

#define VLSNR_HEADER_LENGTH 900

namespace gr {
  namespace dtv {

    class dvbs2_physical_cc_impl : public dvbs2_physical_cc
    {
     private:
      int frame_size;
      int signal_constellation;
      int slots;
      int pilot_mode;
      int pilot_symbols;
      int gold_code;
      int vlsnr_header;
      int vlsnr_set;
      int b[VLSNR_HEADER_LENGTH];
      gr_complex m_bpsk[4][2];
      gr_complex m_pl[90];
      gr_complex m_vlsnr_header[VLSNR_HEADER_LENGTH];
      gr_complex m_zero;
      int m_cscram[FRAME_SIZE_NORMAL];
      void b_64_8_code(unsigned char, int *);
      void pl_header_encode(unsigned char, unsigned char, int *);
      int parity_chk(long, long);
      void build_symbol_scrambler_table(void);

      const static unsigned long g[7];
      const static int ph_scram_tab[64];
      const static int ph_sync_seq[26];
      const static int ph_vlsnr_seq[16][VLSNR_HEADER_LENGTH - 4];

     public:
      dvbs2_physical_cc_impl(dvb_framesize_t framesize, dvb_code_rate_t rate, dvb_constellation_t constellation, dvbs2_pilots_t pilots, int goldcode);
      ~dvbs2_physical_cc_impl();

      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items);
    };

  } // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBS2_PHYSICAL_CC_IMPL_H */


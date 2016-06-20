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

#ifndef INCLUDED_DTV_DVB_BBHEADER_BB_IMPL_H
#define INCLUDED_DTV_DVB_BBHEADER_BB_IMPL_H

#include <gnuradio/dtv/dvb_bbheader_bb.h>
#include "dvb_defines.h"

typedef struct{
    int ts_gs;
    int sis_mis;
    int ccm_acm;
    int issyi;
    int npd;
    int ro;
    int isi;
    int upl;
    int dfl;
    int sync;
    int syncd;
}BBHeader;

typedef struct{
   BBHeader bb_header;
}FrameFormat;

namespace gr {
  namespace dtv {

    class dvb_bbheader_bb_impl : public dvb_bbheader_bb
    {
     private:
      unsigned int kbch;
      unsigned int count;
      unsigned char crc;
      unsigned int input_mode;
      unsigned int extra;
      unsigned int frame_size;
      unsigned char bsave;
      int inband_type_b;
      int fec_blocks;
      int fec_block;
      int ts_rate;
      bool dvbs2x;
      bool alternate;
      bool nibble;
      FrameFormat m_format[1];
      unsigned char crc_tab[256];
      void add_bbheader(unsigned char *, int, int, bool);
      void build_crc8_table(void);
      int add_crc8_bits(unsigned char *, int);
      void add_inband_type_b(unsigned char *, int);

     public:
      dvb_bbheader_bb_impl(dvb_standard_t standard, dvb_framesize_t framesize, dvb_code_rate_t rate, dvbs2_rolloff_factor_t rolloff, dvbt2_inputmode_t mode, dvbt2_inband_t inband, int fecblocks, int tsrate);
      ~dvb_bbheader_bb_impl();

      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items);
    };

  } // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVB_BBHEADER_BB_IMPL_H */


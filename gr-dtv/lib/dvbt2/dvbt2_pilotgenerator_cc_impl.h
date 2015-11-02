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

#ifndef INCLUDED_DTV_DVBT2_PILOTGENERATOR_CC_IMPL_H
#define INCLUDED_DTV_DVBT2_PILOTGENERATOR_CC_IMPL_H

#include <gnuradio/dtv/dvbt2_pilotgenerator_cc.h>
#include <gnuradio/fft/fft.h>
#include "dvb/dvb_defines.h"

#define CHIPS 2624
#define MAX_CARRIERS 27841

enum dvbt2_carrier_type_t {
  DATA_CARRIER = 1,
  P2PILOT_CARRIER,
  P2PAPR_CARRIER,
  TRPAPR_CARRIER,
  SCATTERED_CARRIER,
  CONTINUAL_CARRIER,
  P2PILOT_CARRIER_INVERTED,
  SCATTERED_CARRIER_INVERTED,
  CONTINUAL_CARRIER_INVERTED
};

namespace gr {
  namespace dtv {

    class dvbt2_pilotgenerator_cc_impl : public dvbt2_pilotgenerator_cc
    {
     private:
      int active_items;
      int num_symbols;
      int fft_size;
      int left_nulls;
      int right_nulls;
      int pilot_pattern;
      int carrier_mode;
      int papr_mode;
      int equalization_enable;
      float normalization;
      gr_complex p2_bpsk[2];
      gr_complex sp_bpsk[2];
      gr_complex cp_bpsk[2];
      gr_complex p2_bpsk_inverted[2];
      gr_complex sp_bpsk_inverted[2];
      gr_complex cp_bpsk_inverted[2];
      gr_complex inverse_sinc[32768];
      int prbs[MAX_CARRIERS];
      int pn_sequence[CHIPS];
      int p2_carrier_map[MAX_CARRIERS];
      int data_carrier_map[MAX_CARRIERS];
      int fc_carrier_map[MAX_CARRIERS];
      int N_P2;
      int C_P2;
      int N_FC;
      int C_FC;
      int C_DATA;
      int K_EXT;
      int C_PS;
      int K_OFFSET;
      int dx;
      int dy;
      int miso;
      int miso_group;
      void init_prbs(void);
      void init_pilots(int);

      fft::fft_complex *ofdm_fft;
      int ofdm_fft_size;

      const static unsigned char pn_sequence_table[CHIPS / 8];
      const static int p2_papr_map_1k[10];
      const static int p2_papr_map_2k[18];
      const static int p2_papr_map_4k[36];
      const static int p2_papr_map_8k[72];
      const static int p2_papr_map_16k[144];
      const static int p2_papr_map_32k[288];
      const static int tr_papr_map_1k[10];
      const static int tr_papr_map_2k[18];
      const static int tr_papr_map_4k[36];
      const static int tr_papr_map_8k[72];
      const static int tr_papr_map_16k[144];
      const static int tr_papr_map_32k[288];
      const static int pp1_cp1[20];
      const static int pp1_cp2[25];
      const static int pp1_cp5[44];
      const static int pp2_cp1[20];
      const static int pp2_cp2[22];
      const static int pp2_cp3[2];
      const static int pp2_cp4[2];
      const static int pp2_cp5[41];
      const static int pp2_cp6[88];
      const static int pp3_cp1[22];
      const static int pp3_cp2[20];
      const static int pp3_cp3[1];
      const static int pp3_cp5[44];
      const static int pp3_cp6[49];
      const static int pp4_cp1[20];
      const static int pp4_cp2[23];
      const static int pp4_cp3[1];
      const static int pp4_cp4[2];
      const static int pp4_cp5[44];
      const static int pp4_cp6[86];
      const static int pp5_cp1[19];
      const static int pp5_cp2[23];
      const static int pp5_cp3[3];
      const static int pp5_cp4[1];
      const static int pp5_cp5[44];
      const static int pp6_cp5[88];
      const static int pp6_cp6[88];
      const static int pp7_cp1[15];
      const static int pp7_cp2[30];
      const static int pp7_cp3[5];
      const static int pp7_cp4[3];
      const static int pp7_cp5[35];
      const static int pp7_cp6[92];
      const static int pp8_cp4[47];
      const static int pp8_cp5[39];
      const static int pp8_cp6[89];
      const static int pp2_8k[4];
      const static int pp3_8k[2];
      const static int pp4_8k[2];
      const static int pp7_8k[5];
      const static int pp8_8k[5];
      const static int pp1_16k[4];
      const static int pp2_16k[2];
      const static int pp3_16k[2];
      const static int pp4_16k[2];
      const static int pp5_16k[2];
      const static int pp6_16k[2];
      const static int pp7_16k[3];
      const static int pp8_16k[3];
      const static int pp2_32k[2];
      const static int pp4_32k[2];
      const static int pp6_32k[4];
      const static int pp7_32k[2];
      const static int pp8_32k[6];

     public:
      dvbt2_pilotgenerator_cc_impl(dvbt2_extended_carrier_t carriermode, dvbt2_fftsize_t fftsize, dvbt2_pilotpattern_t pilotpattern, dvb_guardinterval_t guardinterval, int numdatasyms, dvbt2_papr_t paprmode, dvbt2_version_t version, dvbt2_preamble_t preamble, dvbt2_misogroup_t misogroup, dvbt2_equalization_t equalization, dvbt2_bandwidth_t bandwidth, int vlength);
      ~dvbt2_pilotgenerator_cc_impl();

      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items);
    };

  } // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT2_PILOTGENERATOR_CC_IMPL_H */


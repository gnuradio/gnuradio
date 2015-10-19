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

#ifndef INCLUDED_DTV_DVBT_OFDM_SYM_ACQUISITION_IMPL_H
#define INCLUDED_DTV_DVBT_OFDM_SYM_ACQUISITION_IMPL_H

#include <gnuradio/dtv/dvbt_ofdm_sym_acquisition.h>

#undef SEGFAULT_FIX

namespace gr {
  namespace dtv {

    class dvbt_ofdm_sym_acquisition_impl : public dvbt_ofdm_sym_acquisition
    {
     private:
      int d_blocks;
      int d_fft_length;
      int d_cp_length;
      float d_snr;
      float d_rho;

      int d_index;

      int d_search_max;

      gr_complex * d_conj;
      float * d_norm;
      gr_complex * d_corr;
      gr_complex * d_gamma;
      float * d_lambda;
      float * d_arg;

      // For peak detector
      float d_threshold_factor_rise;
      float d_threshold_factor_fall;
      float d_avg_alpha;
      float d_avg;
      float d_phase;
      double d_phaseinc;
      int d_cp_found;
      int d_count;
      double d_nextphaseinc;
      int d_nextpos;

      int d_sym_acq_count;
      int d_sym_acq_timeout;

      int d_initial_acquisition;

      int d_freq_correction_count;
      int d_freq_correction_timeout;

      int d_cp_start;

#ifdef SEGFAULT_FIX
      int d_cp_start_initial;
      int d_cp_start_slip;
#endif

      gr_complex * d_derot;
      int d_to_consume;
      int d_to_out;

      int ml_sync(const gr_complex * in, int lookup_start, int lookup_stop, int * cp_pos, gr_complex * derot, int * to_consume, int * to_out);

      int peak_detect_init(float threshold_factor_rise, float threshold_factor_fall, int look_ahead, float alpha);

      int peak_detect_process(const float * datain, const int datain_length, int * peak_pos, int * peak_max);

      void send_sync_start();
     public:
      dvbt_ofdm_sym_acquisition_impl(int blocks, int fft_length, int occupied_tones, int cp_length, float snr);
      ~dvbt_ofdm_sym_acquisition_impl();

      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items);
    };

  } // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT_OFDM_SYM_ACQUISITION_IMPL_H */


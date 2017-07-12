/* -*- c++ -*- */
/* 
 * Copyright 2015-2017 Free Software Foundation, Inc.
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
#include "dvbt2_paprtr_cc_impl.h"
#include <volk/volk.h>

/* An early exit from the iteration loop is a very effective optimization */
/* Change this line to #undef for validation testing with BBC V&V streams */
#define EARLY_EXIT

namespace gr {
  namespace dtv {

    dvbt2_paprtr_cc::sptr
    dvbt2_paprtr_cc::make(dvbt2_extended_carrier_t carriermode, dvbt2_fftsize_t fftsize, dvbt2_pilotpattern_t pilotpattern, dvb_guardinterval_t guardinterval, int numdatasyms, dvbt2_papr_t paprmode, dvbt2_version_t version, float vclip, int iterations, int vlength)
    {
      return gnuradio::get_initial_sptr
        (new dvbt2_paprtr_cc_impl(carriermode, fftsize, pilotpattern, guardinterval, numdatasyms, paprmode, version, vclip, iterations, vlength));
    }

    /*
     * The private constructor
     */
    dvbt2_paprtr_cc_impl::dvbt2_paprtr_cc_impl(dvbt2_extended_carrier_t carriermode, dvbt2_fftsize_t fftsize, dvbt2_pilotpattern_t pilotpattern, dvb_guardinterval_t guardinterval, int numdatasyms, dvbt2_papr_t paprmode, dvbt2_version_t version, float vclip, int iterations, int vlength)
      : gr::sync_block("dvbt2_paprtr_cc",
              gr::io_signature::make(1, 1, sizeof(gr_complex) * vlength),
              gr::io_signature::make(1, 1, sizeof(gr_complex) * vlength))
    {
      switch (fftsize) {
        case FFTSIZE_1K:
          N_P2 = 16;
          C_PS = 853;
          K_EXT = 0;
          break;
        case FFTSIZE_2K:
          N_P2 = 8;
          C_PS = 1705;
          K_EXT = 0;
          break;
        case FFTSIZE_4K:
          N_P2 = 4;
          C_PS = 3409;
          K_EXT = 0;
          break;
        case FFTSIZE_8K:
        case FFTSIZE_8K_T2GI:
          N_P2 = 2;
          if (carriermode == CARRIERS_NORMAL) {
            C_PS = 6817;
            K_EXT = 0;
          }
          else {
            C_PS = 6913;
            K_EXT = 48;
          }
          break;
        case FFTSIZE_16K:
        case FFTSIZE_16K_T2GI:
          N_P2 = 1;
          if (carriermode == CARRIERS_NORMAL) {
            C_PS = 13633;
            K_EXT = 0;
          }
          else {
            C_PS = 13921;
            K_EXT = 144;
          }
          break;
        case FFTSIZE_32K:
        case FFTSIZE_32K_T2GI:
          N_P2 = 1;
          if (carriermode == CARRIERS_NORMAL) {
            C_PS = 27265;
            K_EXT = 0;
          }
          else {
            C_PS = 27841;
            K_EXT = 288;
          }
          break;
      }
      switch (fftsize) {
        case FFTSIZE_1K:
          switch (pilotpattern) {
            case PILOT_PP1:
              N_FC = 568;
              break;
            case PILOT_PP2:
              N_FC = 710;
              break;
            case PILOT_PP3:
              N_FC = 710;
              break;
            case PILOT_PP4:
              N_FC = 780;
              break;
            case PILOT_PP5:
              N_FC = 780;
              break;
            case PILOT_PP6:
              N_FC = 0;
              break;
            case PILOT_PP7:
              N_FC = 0;
              break;
            case PILOT_PP8:
              N_FC = 0;
              break;
          }
          if (paprmode == PAPR_TR || paprmode == PAPR_BOTH) {
            if (N_FC != 0) {
              N_FC -= 10;
            }
          }
          break;
        case FFTSIZE_2K:
          switch (pilotpattern) {
            case PILOT_PP1:
              N_FC = 1136;
              break;
            case PILOT_PP2:
              N_FC = 1420;
              break;
            case PILOT_PP3:
              N_FC = 1420;
              break;
            case PILOT_PP4:
              N_FC = 1562;
              break;
            case PILOT_PP5:
              N_FC = 1562;
              break;
            case PILOT_PP6:
              N_FC = 0;
              break;
            case PILOT_PP7:
              N_FC = 1632;
              break;
            case PILOT_PP8:
              N_FC = 0;
              break;
          }
          if (paprmode == PAPR_TR || paprmode == PAPR_BOTH) {
            if (N_FC != 0) {
              N_FC -= 18;
            }
          }
          break;
        case FFTSIZE_4K:
          switch (pilotpattern) {
            case PILOT_PP1:
              N_FC = 2272;
              break;
            case PILOT_PP2:
              N_FC = 2840;
              break;
            case PILOT_PP3:
              N_FC = 2840;
              break;
            case PILOT_PP4:
              N_FC = 3124;
              break;
            case PILOT_PP5:
              N_FC = 3124;
              break;
            case PILOT_PP6:
              N_FC = 0;
              break;
            case PILOT_PP7:
              N_FC = 3266;
              break;
            case PILOT_PP8:
              N_FC = 0;
              break;
          }
          if (paprmode == PAPR_TR || paprmode == PAPR_BOTH) {
            if (N_FC != 0) {
              N_FC -= 36;
            }
          }
          break;
        case FFTSIZE_8K:
        case FFTSIZE_8K_T2GI:
          if (carriermode == CARRIERS_NORMAL) {
            switch (pilotpattern) {
              case PILOT_PP1:
                N_FC = 4544;
                break;
              case PILOT_PP2:
                N_FC = 5680;
                break;
              case PILOT_PP3:
                N_FC = 5680;
                break;
              case PILOT_PP4:
                N_FC = 6248;
                break;
              case PILOT_PP5:
                N_FC = 6248;
                break;
              case PILOT_PP6:
                N_FC = 0;
                break;
              case PILOT_PP7:
                N_FC = 6532;
                break;
              case PILOT_PP8:
                N_FC = 0;
                break;
            }
          }
          else {
            switch (pilotpattern) {
              case PILOT_PP1:
                N_FC = 4608;
                break;
              case PILOT_PP2:
                N_FC = 5760;
                break;
              case PILOT_PP3:
                N_FC = 5760;
                break;
              case PILOT_PP4:
                N_FC = 6336;
                break;
              case PILOT_PP5:
                N_FC = 6336;
                break;
              case PILOT_PP6:
                N_FC = 0;
                break;
              case PILOT_PP7:
                N_FC = 6624;
                break;
              case PILOT_PP8:
                N_FC = 0;
                break;
            }
          }
          if (paprmode == PAPR_TR || paprmode == PAPR_BOTH) {
            if (N_FC != 0) {
              N_FC -= 72;
            }
          }
          break;
        case FFTSIZE_16K:
        case FFTSIZE_16K_T2GI:
          if (carriermode == CARRIERS_NORMAL) {
            switch (pilotpattern) {
              case PILOT_PP1:
                N_FC = 9088;
                break;
              case PILOT_PP2:
                N_FC = 11360;
                break;
              case PILOT_PP3:
                N_FC = 11360;
                break;
              case PILOT_PP4:
                N_FC = 12496;
                break;
              case PILOT_PP5:
                N_FC = 12496;
                break;
              case PILOT_PP6:
                N_FC = 13064;
                break;
              case PILOT_PP7:
                N_FC = 13064;
                break;
              case PILOT_PP8:
                N_FC = 0;
                break;
            }
          }
          else {
            switch (pilotpattern) {
              case PILOT_PP1:
                N_FC = 9280;
                break;
              case PILOT_PP2:
                N_FC = 11600;
                break;
              case PILOT_PP3:
                N_FC = 11600;
                break;
              case PILOT_PP4:
                N_FC = 12760;
                break;
              case PILOT_PP5:
                N_FC = 12760;
                break;
              case PILOT_PP6:
                N_FC = 13340;
                break;
              case PILOT_PP7:
                N_FC = 13340;
                break;
              case PILOT_PP8:
                N_FC = 0;
                break;
            }
          }
          if (paprmode == PAPR_TR || paprmode == PAPR_BOTH) {
            if (N_FC != 0) {
              N_FC -= 144;
            }
          }
          break;
        case FFTSIZE_32K:
        case FFTSIZE_32K_T2GI:
          if (carriermode == CARRIERS_NORMAL) {
            switch (pilotpattern) {
              case PILOT_PP1:
                N_FC = 0;
                break;
              case PILOT_PP2:
                N_FC = 22720;
                break;
              case PILOT_PP3:
                N_FC = 0;
                break;
              case PILOT_PP4:
                N_FC = 24992;
                break;
              case PILOT_PP5:
                N_FC = 0;
                break;
              case PILOT_PP6:
                N_FC = 26128;
                break;
              case PILOT_PP7:
                N_FC = 0;
                break;
              case PILOT_PP8:
                N_FC = 0;
                break;
            }
          }
          else {
            switch (pilotpattern) {
              case PILOT_PP1:
                N_FC = 0;
                break;
              case PILOT_PP2:
                N_FC = 23200;
                break;
              case PILOT_PP3:
                N_FC = 0;
                break;
              case PILOT_PP4:
                N_FC = 25520;
                break;
              case PILOT_PP5:
                N_FC = 0;
                break;
              case PILOT_PP6:
                N_FC = 26680;
                break;
              case PILOT_PP7:
                N_FC = 0;
                break;
              case PILOT_PP8:
                N_FC = 0;
                break;
            }
          }
          if (paprmode == PAPR_TR || paprmode == PAPR_BOTH) {
            if (N_FC != 0) {
              N_FC -= 288;
            }
          }
          break;
      }
      if (guardinterval == GI_1_128 && pilotpattern == PILOT_PP7) {
        N_FC = 0;
      }
      if (guardinterval == GI_1_32 && pilotpattern == PILOT_PP4) {
        N_FC = 0;
      }
      if (guardinterval == GI_1_16 && pilotpattern == PILOT_PP2) {
        N_FC = 0;
      }
      if (guardinterval == GI_19_256 && pilotpattern == PILOT_PP2) {
        N_FC = 0;
      }
      for (int i = 0; i < C_PS; i++) {
        p2_carrier_map[i] = DATA_CARRIER;
        fc_carrier_map[i] = DATA_CARRIER;
      }
      switch (fftsize) {
        case FFTSIZE_1K:
          for (int i = 0; i < 10; i++) {
            p2_carrier_map[p2_papr_map_1k[i]] = P2PAPR_CARRIER;
            fc_carrier_map[p2_papr_map_1k[i]] = TRPAPR_CARRIER;
          }
          N_TR = 10;
          p2_papr_map = p2_papr_map_1k;
          tr_papr_map = tr_papr_map_1k;
          break;
        case FFTSIZE_2K:
          for (int i = 0; i < 18; i++) {
            p2_carrier_map[p2_papr_map_2k[i]] = P2PAPR_CARRIER;
            fc_carrier_map[p2_papr_map_2k[i]] = TRPAPR_CARRIER;
          }
          N_TR = 18;
          p2_papr_map = p2_papr_map_2k;
          tr_papr_map = tr_papr_map_2k;
          break;
        case FFTSIZE_4K:
          for (int i = 0; i < 36; i++) {
            p2_carrier_map[p2_papr_map_4k[i]] = P2PAPR_CARRIER;
            fc_carrier_map[p2_papr_map_4k[i]] = TRPAPR_CARRIER;
          }
          N_TR = 36;
          p2_papr_map = p2_papr_map_4k;
          tr_papr_map = tr_papr_map_4k;
          break;
        case FFTSIZE_8K:
        case FFTSIZE_8K_T2GI:
          for (int i = 0; i < 72; i++) {
            p2_carrier_map[p2_papr_map_8k[i] + K_EXT] = P2PAPR_CARRIER;
            fc_carrier_map[p2_papr_map_8k[i] + K_EXT] = TRPAPR_CARRIER;
          }
          N_TR = 72;
          p2_papr_map = p2_papr_map_8k;
          tr_papr_map = tr_papr_map_8k;
          break;
        case FFTSIZE_16K:
        case FFTSIZE_16K_T2GI:
          for (int i = 0; i < 144; i++) {
            p2_carrier_map[p2_papr_map_16k[i] + K_EXT] = P2PAPR_CARRIER;
            fc_carrier_map[p2_papr_map_16k[i] + K_EXT] = TRPAPR_CARRIER;
          }
          N_TR = 144;
          p2_papr_map = p2_papr_map_16k;
          tr_papr_map = tr_papr_map_16k;
          break;
        case FFTSIZE_32K:
        case FFTSIZE_32K_T2GI:
          for (int i = 0; i < 288; i++) {
            p2_carrier_map[p2_papr_map_32k[i] + K_EXT] = P2PAPR_CARRIER;
            fc_carrier_map[p2_papr_map_32k[i] + K_EXT] = TRPAPR_CARRIER;
          }
          N_TR = 288;
          p2_papr_map = p2_papr_map_32k;
          tr_papr_map = tr_papr_map_32k;
          break;
      }
      switch (pilotpattern) {
        case PILOT_PP1:
          dx = 3;
          dy = 4;
          break;
        case PILOT_PP2:
          dx = 6;
          dy = 2;
          break;
        case PILOT_PP3:
          dx = 6;
          dy = 4;
          break;
        case PILOT_PP4:
          dx = 12;
          dy = 2;
          break;
        case PILOT_PP5:
          dx = 12;
          dy = 4;
          break;
        case PILOT_PP6:
          dx = 24;
          dy = 2;
          break;
        case PILOT_PP7:
          dx = 24;
          dy = 4;
          break;
        case PILOT_PP8:
          dx = 6;
          dy = 16;
          break;
      }
      shift = 0;
      papr_map = p2_papr_map;
      fft_size = fftsize;
      pilot_pattern = pilotpattern;
      carrier_mode = carriermode;
      papr_mode = paprmode;
      version_num = version;
      if (version == VERSION_131 && papr_mode == PAPR_OFF) {
        v_clip = 3.0;
        num_iterations = 1;
      }
      else {
        v_clip = vclip;
        num_iterations = iterations;
      }
      left_nulls = ((vlength - C_PS) / 2) + 1;
      right_nulls = (vlength - C_PS) / 2;
      papr_fft_size = vlength;
      papr_fft = new (std::nothrow) fft::fft_complex(papr_fft_size, false, 1);
      if (papr_fft == NULL) {
        GR_LOG_FATAL(d_logger, "Tone Reservation PAPR, cannot allocate memory for papr_fft.");
        throw std::bad_alloc();
      }
      ones_freq = (gr_complex*) volk_malloc(sizeof(gr_complex) * papr_fft_size, volk_get_alignment());
      if (ones_freq == NULL) {
        GR_LOG_FATAL(d_logger, "Tone Reservation PAPR, cannot allocate memory for ones_freq.");
        delete papr_fft;
        throw std::bad_alloc();
      }
      ones_time = (gr_complex*) volk_malloc(sizeof(gr_complex) * papr_fft_size, volk_get_alignment());
      if (ones_time == NULL) {
        GR_LOG_FATAL(d_logger, "Tone Reservation PAPR, cannot allocate memory for ones_time.");
        volk_free(ones_freq);
        delete papr_fft;
        throw std::bad_alloc();
      }
      c = (gr_complex*) volk_malloc(sizeof(gr_complex) * papr_fft_size, volk_get_alignment());
      if (c == NULL) {
        GR_LOG_FATAL(d_logger, "Tone Reservation PAPR, cannot allocate memory for c.");
        volk_free(ones_time);
        volk_free(ones_freq);
        delete papr_fft;
        throw std::bad_alloc();
      }
      ctemp = (gr_complex*) volk_malloc(sizeof(gr_complex) * papr_fft_size, volk_get_alignment());
      if (ctemp == NULL) {
        GR_LOG_FATAL(d_logger, "Tone Reservation PAPR, cannot allocate memory for ctemp.");
        volk_free(c);
        volk_free(ones_time);
        volk_free(ones_freq);
        delete papr_fft;
        throw std::bad_alloc();
      }
      magnitude = (float*) volk_malloc(sizeof(float) * papr_fft_size, volk_get_alignment());
      if (magnitude == NULL) {
        GR_LOG_FATAL(d_logger, "Tone Reservation PAPR, cannot allocate memory for magnitude.");
        volk_free(ctemp);
        volk_free(c);
        volk_free(ones_time);
        volk_free(ones_freq);
        delete papr_fft;
        throw std::bad_alloc();
      }
      r = (gr_complex*) volk_malloc(sizeof(gr_complex) * N_TR, volk_get_alignment());
      if (r == NULL) {
        GR_LOG_FATAL(d_logger, "Tone Reservation PAPR, cannot allocate memory for r.");
        volk_free(magnitude);
        volk_free(ctemp);
        volk_free(c);
        volk_free(ones_time);
        volk_free(ones_freq);
        delete papr_fft;
        throw std::bad_alloc();
      }
      rNew = (gr_complex*) volk_malloc(sizeof(gr_complex) * N_TR, volk_get_alignment());
      if (rNew == NULL) {
        GR_LOG_FATAL(d_logger, "Tone Reservation PAPR, cannot allocate memory for rNew.");
        volk_free(r);
        volk_free(magnitude);
        volk_free(ctemp);
        volk_free(c);
        volk_free(ones_time);
        volk_free(ones_freq);
        delete papr_fft;
        throw std::bad_alloc();
      }
      v = (gr_complex*) volk_malloc(sizeof(gr_complex) * N_TR, volk_get_alignment());
      if (v == NULL) {
        GR_LOG_FATAL(d_logger, "Tone Reservation PAPR, cannot allocate memory for v.");
        volk_free(rNew);
        volk_free(r);
        volk_free(magnitude);
        volk_free(ctemp);
        volk_free(c);
        volk_free(ones_time);
        volk_free(ones_freq);
        delete papr_fft;
        throw std::bad_alloc();
      }
      num_symbols = numdatasyms + N_P2;
      set_output_multiple(num_symbols);
    }

    /*
     * Our virtual destructor.
     */
    dvbt2_paprtr_cc_impl::~dvbt2_paprtr_cc_impl()
    {
      volk_free(v);
      volk_free(rNew);
      volk_free(r);
      volk_free(magnitude);
      volk_free(ctemp);
      volk_free(c);
      volk_free(ones_time);
      volk_free(ones_freq);
      delete papr_fft;
    }

    void
    dvbt2_paprtr_cc_impl::init_pilots(int symbol)
    {
      for (int i = 0; i < C_PS; i++) {
        data_carrier_map[i] = DATA_CARRIER;
      }
      if (carrier_mode == CARRIERS_NORMAL) {
        shift = dx * (symbol % dy);
      }
      else {
        shift = dx * ((symbol + (K_EXT / dx)) % dy);
      }
      switch (fft_size) {
        case FFTSIZE_1K:
          for (int i = 0; i < 10; i++) {
            data_carrier_map[tr_papr_map_1k[i] + shift] = TRPAPR_CARRIER;
          }
          break;
        case FFTSIZE_2K:
          for (int i = 0; i < 18; i++) {
            data_carrier_map[tr_papr_map_2k[i] + shift] = TRPAPR_CARRIER;
          }
          break;
        case FFTSIZE_4K:
          for (int i = 0; i < 36; i++) {
            data_carrier_map[tr_papr_map_4k[i] + shift] = TRPAPR_CARRIER;
          }
          break;
        case FFTSIZE_8K:
        case FFTSIZE_8K_T2GI:
          for (int i = 0; i < 72; i++) {
            data_carrier_map[tr_papr_map_8k[i] + shift] = TRPAPR_CARRIER;
          }
          break;
        case FFTSIZE_16K:
        case FFTSIZE_16K_T2GI:
          for (int i = 0; i < 144; i++) {
            data_carrier_map[tr_papr_map_16k[i] + shift] = TRPAPR_CARRIER;
          }
          break;
        case FFTSIZE_32K:
        case FFTSIZE_32K_T2GI:
          for (int i = 0; i < 288; i++) {
            data_carrier_map[tr_papr_map_32k[i] + shift] = TRPAPR_CARRIER;
          }
          break;
      }
    }

    int
    dvbt2_paprtr_cc_impl::work(int noutput_items,
                          gr_vector_const_void_star &input_items,
                          gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];
      const gr_complex one (1.0, 0.0);
      const gr_complex zero (0.0, 0.0);
      const float normalization = 1.0 / N_TR;
      const int L_FC = (N_FC != 0);
      const int center = (C_PS - 1) / 2;
      const float aMax = 5.0 * N_TR * std::sqrt(10.0 / (27.0 * C_PS));
      gr_complex *dst;
      int m = 0, index, valid;
      float y, a, alpha;
      gr_complex u, result;
      double vtemp;

      for (int i = 0; i < noutput_items; i += num_symbols) {
        if (papr_mode == PAPR_TR || papr_mode == PAPR_BOTH || (version_num == VERSION_131 && papr_mode == PAPR_OFF)) {
          for (int j = 0; j < num_symbols; j++) {
            init_pilots(j);
            valid = FALSE;
            if (j < N_P2) {
              index = 0;
              memset(&ones_freq[index], 0, sizeof(gr_complex) * left_nulls);
              index = left_nulls;
              for (int n = 0; n < C_PS; n++) {
                if (p2_carrier_map[n] == P2PAPR_CARRIER) {
                  ones_freq[index++] = one;
                }
                else {
                  ones_freq[index++] = zero;
                }
              }
              memset(&ones_freq[index], 0, sizeof(gr_complex) * right_nulls);
              papr_map = p2_papr_map;
              valid = TRUE;
            }
            else if (j == (num_symbols - L_FC) && (papr_mode == PAPR_TR || papr_mode == PAPR_BOTH)) {
              index = 0;
              memset(&ones_freq[index], 0, sizeof(gr_complex) * left_nulls);
              index = left_nulls;
              for (int n = 0; n < C_PS; n++) {
                if (fc_carrier_map[n] == TRPAPR_CARRIER) {
                  ones_freq[index++] = one;
                }
                else {
                  ones_freq[index++] = zero;
                }
              }
              memset(&ones_freq[index], 0, sizeof(gr_complex) * right_nulls);
              papr_map = p2_papr_map;
              valid = TRUE;
            }
            else if (papr_mode == PAPR_TR || papr_mode == PAPR_BOTH) {
              index = 0;
              memset(&ones_freq[index], 0, sizeof(gr_complex) * left_nulls);
              index = left_nulls;
              for (int n = 0; n < C_PS; n++) {
                if (data_carrier_map[n] == TRPAPR_CARRIER) {
                  ones_freq[index++] = one;
                }
                else {
                  ones_freq[index++] = zero;
                }
              }
              memset(&ones_freq[index], 0, sizeof(gr_complex) * right_nulls);
              papr_map = tr_papr_map;
              valid = TRUE;
            }
            if (valid == TRUE) {
              dst = papr_fft->get_inbuf();
              memcpy(&dst[papr_fft_size / 2], &ones_freq[0], sizeof(gr_complex) * papr_fft_size / 2);
              memcpy(&dst[0], &ones_freq[papr_fft_size / 2], sizeof(gr_complex) * papr_fft_size / 2);
              papr_fft->execute();
              memcpy(ones_time, papr_fft->get_outbuf(), sizeof(gr_complex) * papr_fft_size);
              volk_32fc_s32fc_multiply_32fc(ones_time, ones_time, normalization, papr_fft_size);
              memset(&r[0], 0, sizeof(gr_complex) * N_TR);
              memset(&c[0], 0, sizeof(gr_complex) * papr_fft_size);
              for (int k = 1; k <= num_iterations; k++) {
                y = 0.0;
                volk_32f_x2_add_32f((float*)ctemp, (float*)in, (float*)c, papr_fft_size * 2);
                volk_32fc_magnitude_32f(magnitude, ctemp, papr_fft_size);
                for (int n = 0; n < papr_fft_size; n++) {
                  if (magnitude[n] > y) {
                    y = magnitude[n];
                    m = n;
                  }
                }
#ifdef EARLY_EXIT
                if (y < v_clip + 0.01) {
                  break;
                }
#else
                if (y < v_clip) {
                  break;
                }
#endif
                u = (in[m] + c[m]) / y;
                alpha = y - v_clip;
                for (int n = 0; n < N_TR; n++) {
                  vtemp = (-2.0 * M_PI * m * ((papr_map[n] + shift) - center)) / papr_fft_size;
                  ctemp[n] = std::exp(gr_complexd(0.0, vtemp));
                }
                volk_32fc_s32fc_multiply_32fc(v, ctemp, u, N_TR);
                volk_32f_s32f_multiply_32f((float*)rNew, (float*)v, alpha, N_TR * 2);
                volk_32f_x2_subtract_32f((float*)rNew, (float*)r, (float*)rNew, N_TR * 2);
                volk_32fc_x2_multiply_conjugate_32fc(ctemp, r, v, N_TR);
                for (int n = 0; n < N_TR; n++) {
                  alphaLimit[n] = std::sqrt((aMax * aMax) - (ctemp[n].imag() * ctemp[n].imag())) + ctemp[n].real();
                }
                index = 0;
                volk_32fc_magnitude_32f(magnitude, rNew, N_TR);
                for (int n = 0; n < N_TR; n++) {
                  if (magnitude[n] > aMax) {
                    alphaLimitMax[index++] = alphaLimit[n];
                  }
                }
                if (index != 0) {
                  a = 1.0e+30;
                  for (int n = 0; n < index; n++) {
                    if (alphaLimitMax[n] < a) {
                      a = alphaLimitMax[n];
                    }
                  }
                  alpha = a;
                  volk_32f_s32f_multiply_32f((float*)rNew, (float*)v, alpha, N_TR * 2);
                  volk_32f_x2_subtract_32f((float*)rNew, (float*)r, (float*)rNew, N_TR * 2);
                }
                for (int n = 0; n < papr_fft_size; n++) {
                  ones_freq[(n + m) % papr_fft_size] = ones_time[n];
                }
                result = u * alpha;
                volk_32fc_s32fc_multiply_32fc(ctemp, ones_freq, result, papr_fft_size);
                volk_32f_x2_subtract_32f((float*)c, (float*)c, (float*)ctemp, papr_fft_size * 2);
                memcpy(r, rNew, sizeof(gr_complex) * N_TR);
              }
              volk_32f_x2_add_32f((float*)out, (float*)in, (float*)c, papr_fft_size * 2);
              in = in + papr_fft_size;
              out = out + papr_fft_size;
            }
            else {
              memcpy(out, in, sizeof(gr_complex) * papr_fft_size);
              in = in + papr_fft_size;
              out = out + papr_fft_size;
            }
          }
        }
        else {
          for (int j = 0; j < num_symbols; j++) {
            memcpy(out, in, sizeof(gr_complex) * papr_fft_size);
            in = in + papr_fft_size;
            out = out + papr_fft_size;
          }
        }
      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

    const int dvbt2_paprtr_cc_impl::p2_papr_map_1k[10] = 
    {
      116, 130, 134, 157, 182, 256, 346, 478, 479, 532
    };

    const int dvbt2_paprtr_cc_impl::p2_papr_map_2k[18] = 
    {
      113, 124, 262, 467, 479, 727, 803, 862, 910, 946,
      980, 1201, 1322, 1342, 1396, 1397, 1562, 1565
    };

    const int dvbt2_paprtr_cc_impl::p2_papr_map_4k[36] = 
    {
      104, 116, 119, 163, 170, 173, 664, 886, 1064, 1151, 1196, 1264, 1531,
      1736, 1951, 1960, 2069, 2098, 2311, 2366, 2473, 2552, 2584, 2585, 2645,
      2774, 2846, 2882, 3004, 3034, 3107, 3127, 3148, 3191, 3283, 3289
    };

    const int dvbt2_paprtr_cc_impl::p2_papr_map_8k[72] = 
    {
      106, 109, 110, 112, 115, 118, 133, 142, 163, 184, 206, 247, 445, 461,
      503, 565, 602, 656, 766, 800, 922, 1094, 1108, 1199, 1258, 1726, 1793,
      1939, 2128, 2714, 3185, 3365, 3541, 3655, 3770, 3863, 4066, 4190, 4282,
      4565, 4628, 4727, 4882, 4885, 5143, 5192, 5210, 5257, 5261, 5459, 5651,
      5809, 5830, 5986, 6020, 6076, 6253, 6269, 6410, 6436, 6467, 6475, 6509,
      6556, 6611, 6674, 6685, 6689, 6691, 6695, 6698, 6701
    };

    const int dvbt2_paprtr_cc_impl::p2_papr_map_16k[144] = 
    {
      104, 106, 107, 109, 110, 112, 113, 115, 116, 118, 119, 121, 122, 125, 128,
      131, 134, 137, 140, 143, 161, 223, 230, 398, 482, 497, 733, 809, 850, 922,
      962, 1196, 1256, 1262, 1559, 1691, 1801, 1819, 1937, 2005, 2095, 2308, 2383,
      2408, 2425, 2428, 2479, 2579, 2893, 2902, 3086, 3554, 4085, 4127, 4139, 4151,
      4163, 4373, 4400, 4576, 4609, 4952, 4961, 5444, 5756, 5800, 6094, 6208, 6658,
      6673, 6799, 7208, 7682, 8101, 8135, 8230, 8692, 8788, 8933, 9323, 9449, 9478,
      9868, 10192, 10261, 10430, 10630, 10685, 10828, 10915, 10930, 10942, 11053,
      11185, 11324, 11369, 11468, 11507, 11542, 11561, 11794, 11912, 11974, 11978,
      12085, 12179, 12193, 12269, 12311, 12758, 12767, 12866, 12938, 12962, 12971,
      13099, 13102, 13105, 13120, 13150, 13280, 13282, 13309, 13312, 13321, 13381,
      13402, 13448, 13456, 13462, 13463, 13466, 13478, 13492, 13495, 13498, 13501,
      13502, 13504, 13507, 13510, 13513, 13514, 13516
    };

    const int dvbt2_paprtr_cc_impl::p2_papr_map_32k[288] = 
    {
      104, 106, 107, 109, 110, 112, 113, 115, 118, 121, 124, 127, 130, 133, 136,
      139, 142, 145, 148, 151, 154, 157, 160, 163, 166, 169, 172, 175, 178, 181,
      184, 187, 190, 193, 196, 199, 202, 205, 208, 211, 404, 452, 455, 467, 509,
      539, 568, 650, 749, 1001, 1087, 1286, 1637, 1823, 1835, 1841, 1889, 1898,
      1901, 2111, 2225, 2252, 2279, 2309, 2315, 2428, 2452, 2497, 2519, 3109, 3154,
      3160, 3170, 3193, 3214, 3298, 3331, 3346, 3388, 3397, 3404, 3416, 3466, 3491,
      3500, 3572, 4181, 4411, 4594, 4970, 5042, 5069, 5081, 5086, 5095, 5104, 5320,
      5465, 5491, 6193, 6541, 6778, 6853, 6928, 6934, 7030, 7198, 7351, 7712, 7826,
      7922, 8194, 8347, 8350, 8435, 8518, 8671, 8861, 8887, 9199, 9980, 10031, 10240,
      10519, 10537, 10573, 10589, 11078, 11278, 11324, 11489, 11642, 12034, 12107, 12184,
      12295, 12635, 12643, 12941, 12995, 13001, 13133, 13172, 13246, 13514, 13522, 13939,
      14362, 14720, 14926, 15338, 15524, 15565, 15662, 15775, 16358, 16613, 16688, 16760,
      17003, 17267, 17596, 17705, 18157, 18272, 18715, 18994, 19249, 19348, 20221, 20855,
      21400, 21412, 21418, 21430, 21478, 21559, 21983, 21986, 22331, 22367, 22370, 22402,
      22447, 22535, 22567, 22571, 22660, 22780, 22802, 22844, 22888, 22907, 23021, 23057,
      23086, 23213, 23240, 23263, 23333, 23369, 23453, 23594, 24143, 24176, 24319, 24325,
      24565, 24587, 24641, 24965, 25067, 25094, 25142, 25331, 25379, 25465, 25553, 25589,
      25594, 25655, 25664, 25807, 25823, 25873, 25925, 25948, 26002, 26008, 26102, 26138,
      26141, 26377, 26468, 26498, 26510, 26512, 26578, 26579, 26588, 26594, 26597, 26608,
      26627, 26642, 26767, 26776, 26800, 26876, 26882, 26900, 26917, 26927, 26951, 26957,
      26960, 26974, 26986, 27010, 27013, 27038, 27044, 27053, 27059, 27061, 27074, 27076,
      27083, 27086, 27092, 27094, 27098, 27103, 27110, 27115, 27118, 27119, 27125, 27128,
      27130, 27133, 27134, 27140, 27143, 27145, 27146, 27148, 27149
    };

    const int dvbt2_paprtr_cc_impl::tr_papr_map_1k[10] = 
    {
      109, 117, 122, 129, 139, 321, 350, 403, 459, 465
    };

    const int dvbt2_paprtr_cc_impl::tr_papr_map_2k[18] = 
    {
      250, 404, 638, 677, 700, 712, 755, 952, 1125, 1145,
      1190, 1276, 1325, 1335, 1406, 1431, 1472, 1481
    };

    const int dvbt2_paprtr_cc_impl::tr_papr_map_4k[36] = 
    {
      170, 219, 405, 501, 597, 654, 661, 745, 995, 1025, 1319, 1361, 1394,
      1623, 1658, 1913, 1961, 1971, 2106, 2117, 2222, 2228, 2246, 2254, 2361,
      2468, 2469, 2482, 2637, 2679, 2708, 2825, 2915, 2996, 3033, 3119
    };

    const int dvbt2_paprtr_cc_impl::tr_papr_map_8k[72] = 
    {
      111, 115, 123, 215, 229, 392, 613, 658, 831, 842, 997, 1503, 1626, 1916,
      1924, 1961, 2233, 2246, 2302, 2331, 2778, 2822, 2913, 2927, 2963, 2994,
      3087, 3162, 3226, 3270, 3503, 3585, 3711, 3738, 3874, 3902, 4013, 4017,
      4186, 4253, 4292, 4339, 4412, 4453, 4669, 4910, 5015, 5030, 5061, 5170,
      5263, 5313, 5360, 5384, 5394, 5493, 5550, 5847, 5901, 5999, 6020, 6165,
      6174, 6227, 6245, 6314, 6316, 6327, 6503, 6507, 6545, 6565
    };

    const int dvbt2_paprtr_cc_impl::tr_papr_map_16k[144] = 
    {
      109, 122, 139, 171, 213, 214, 251, 585, 763, 1012, 1021, 1077, 1148, 1472,
      1792, 1883, 1889, 1895, 1900, 2013, 2311, 2582, 2860, 2980, 3011, 3099, 3143,
      3171, 3197, 3243, 3257, 3270, 3315, 3436, 3470, 3582, 3681, 3712, 3767, 3802,
      3979, 4045, 4112, 4197, 4409, 4462, 4756, 5003, 5007, 5036, 5246, 5483, 5535,
      5584, 5787, 5789, 6047, 6349, 6392, 6498, 6526, 6542, 6591, 6680, 6688, 6785,
      6860, 7134, 7286, 7387, 7415, 7417, 7505, 7526, 7541, 7551, 7556, 7747, 7814,
      7861, 7880, 8045, 8179, 8374, 8451, 8514, 8684, 8698, 8804, 8924, 9027, 9113,
      9211, 9330, 9479, 9482, 9487, 9619, 9829, 10326, 10394, 10407, 10450, 10528,
      10671, 10746, 10774, 10799, 10801, 10912, 11113, 11128, 11205, 11379, 11459,
      11468, 11658, 11776, 11791, 11953, 11959, 12021, 12028, 12135, 12233, 12407,
      12441, 12448, 12470, 12501, 12548, 12642, 12679, 12770, 12788, 12899, 12923,
      12939, 13050, 13103, 13147, 13256, 13339, 13409
    };

    const int dvbt2_paprtr_cc_impl::tr_papr_map_32k[288] = 
    {
      164, 320, 350, 521, 527, 578, 590, 619, 635, 651, 662, 664, 676, 691, 723,
      940, 1280, 1326, 1509, 1520, 1638, 1682, 1805, 1833, 1861, 1891, 1900, 1902,
      1949, 1967, 1978, 1998, 2006, 2087, 2134, 2165, 2212, 2427, 2475, 2555, 2874,
      3067, 3091, 3101, 3146, 3188, 3322, 3353, 3383, 3503, 3523, 3654, 3856, 4150,
      4158, 4159, 4174, 4206, 4318, 4417, 4629, 4631, 4875, 5104, 5106, 5111, 5131,
      5145, 5146, 5177, 5181, 5246, 5269, 5458, 5474, 5500, 5509, 5579, 5810, 5823,
      6058, 6066, 6098, 6411, 6741, 6775, 6932, 7103, 7258, 7303, 7413, 7586, 7591,
      7634, 7636, 7655, 7671, 7675, 7756, 7760, 7826, 7931, 7937, 7951, 8017, 8061,
      8071, 8117, 8317, 8321, 8353, 8806, 9010, 9237, 9427, 9453, 9469, 9525, 9558,
      9574, 9584, 9820, 9973, 10011, 10043, 10064, 10066, 10081, 10136, 10193, 10249,
      10511, 10537, 11083, 11350, 11369, 11428, 11622, 11720, 11924, 11974, 11979, 12944,
      12945, 13009, 13070, 13110, 13257, 13364, 13370, 13449, 13503, 13514, 13520, 13583,
      13593, 13708, 13925, 14192, 14228, 14235, 14279, 14284, 14370, 14393, 14407, 14422,
      14471, 14494, 14536, 14617, 14829, 14915, 15094, 15138, 15155, 15170, 15260, 15283,
      15435, 15594, 15634, 15810, 16178, 16192, 16196, 16297, 16366, 16498, 16501, 16861,
      16966, 17039, 17057, 17240, 17523, 17767, 18094, 18130, 18218, 18344, 18374, 18657,
      18679, 18746, 18772, 18779, 18786, 18874, 18884, 18955, 19143, 19497, 19534, 19679,
      19729, 19738, 19751, 19910, 19913, 20144, 20188, 20194, 20359, 20490, 20500, 20555,
      20594, 20633, 20656, 21099, 21115, 21597, 22139, 22208, 22244, 22530, 22547, 22562,
      22567, 22696, 22757, 22798, 22854, 22877, 23068, 23102, 23141, 23154, 23170, 23202,
      23368, 23864, 24057, 24215, 24219, 24257, 24271, 24325, 24447, 25137, 25590, 25702,
      25706, 25744, 25763, 25811, 25842, 25853, 25954, 26079, 26158, 26285, 26346, 26488,
      26598, 26812, 26845, 26852, 26869, 26898, 26909, 26927, 26931, 26946, 26975, 26991,
      27039
    };

  } /* namespace dtv */
} /* namespace gr */


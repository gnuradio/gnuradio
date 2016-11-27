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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "dvbt2_freqinterleaver_cc_impl.h"

namespace gr {
  namespace dtv {

    dvbt2_freqinterleaver_cc::sptr
    dvbt2_freqinterleaver_cc::make(dvbt2_extended_carrier_t carriermode, dvbt2_fftsize_t fftsize, dvbt2_pilotpattern_t pilotpattern, dvb_guardinterval_t guardinterval, int numdatasyms, dvbt2_papr_t paprmode, dvbt2_version_t version, dvbt2_preamble_t preamble)
    {
      return gnuradio::get_initial_sptr
        (new dvbt2_freqinterleaver_cc_impl(carriermode, fftsize, pilotpattern, guardinterval, numdatasyms, paprmode, version, preamble));
    }

    /*
     * The private constructor
     */
    dvbt2_freqinterleaver_cc_impl::dvbt2_freqinterleaver_cc_impl(dvbt2_extended_carrier_t carriermode, dvbt2_fftsize_t fftsize, dvbt2_pilotpattern_t pilotpattern, dvb_guardinterval_t guardinterval, int numdatasyms, dvbt2_papr_t paprmode, dvbt2_version_t version, dvbt2_preamble_t preamble)
      : gr::sync_block("dvbt2_freqinterleaver_cc",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(gr_complex)))
    {
      int max_states, xor_size, pn_mask, result;
      int q_even = 0;
      int q_odd = 0;
      int q_evenP2 = 0;
      int q_oddP2 = 0;
      int q_evenFC = 0;
      int q_oddFC = 0;
      int lfsr = 0;
      int logic1k[2] = {0, 4};
      int logic2k[2] = {0, 3};
      int logic4k[2] = {0, 2};
      int logic8k[4] = {0, 1, 4, 6};
      int logic16k[6] = {0, 1, 4, 5, 9, 11};
      int logic32k[4] = {0, 1, 2, 12};
      int *logic;
      const int *bitpermeven, *bitpermodd;
      int pn_degree, even, odd;
      if ((preamble == PREAMBLE_T2_SISO) || (preamble == PREAMBLE_T2_LITE_SISO)) {
        switch (fftsize) {
          case FFTSIZE_1K:
            N_P2 = 16;
            C_P2 = 558;
            break;
          case FFTSIZE_2K:
            N_P2 = 8;
            C_P2 = 1118;
            break;
          case FFTSIZE_4K:
            N_P2 = 4;
            C_P2 = 2236;
            break;
          case FFTSIZE_8K:
          case FFTSIZE_8K_T2GI:
            N_P2 = 2;
            C_P2 = 4472;
            break;
          case FFTSIZE_16K:
          case FFTSIZE_16K_T2GI:
            N_P2 = 1;
            C_P2 = 8944;
            break;
          case FFTSIZE_32K:
          case FFTSIZE_32K_T2GI:
            N_P2 = 1;
            C_P2 = 22432;
            break;
        }
      }
      else {
        switch (fftsize) {
          case FFTSIZE_1K:
            N_P2 = 16;
            C_P2 = 546;
            break;
          case FFTSIZE_2K:
            N_P2 = 8;
            C_P2 = 1098;
            break;
          case FFTSIZE_4K:
            N_P2 = 4;
            C_P2 = 2198;
            break;
          case FFTSIZE_8K:
          case FFTSIZE_8K_T2GI:
            N_P2 = 2;
            C_P2 = 4398;
            break;
          case FFTSIZE_16K:
          case FFTSIZE_16K_T2GI:
            N_P2 = 1;
            C_P2 = 8814;
            break;
          case FFTSIZE_32K:
          case FFTSIZE_32K_T2GI:
            N_P2 = 1;
            C_P2 = 17612;
            break;
        }
      }
      switch (fftsize) {
        case FFTSIZE_1K:
          pn_degree = 9;
          pn_mask = 0x1ff;
          max_states = 1024;
          logic = &logic1k[0];
          xor_size = 2;
          bitpermeven = &bitperm1keven[0];
          bitpermodd = &bitperm1kodd[0];
          break;
        case FFTSIZE_2K:
          pn_degree = 10;
          pn_mask = 0x3ff;
          max_states = 2048;
          logic = &logic2k[0];
          xor_size = 2;
          bitpermeven = &bitperm2keven[0];
          bitpermodd = &bitperm2kodd[0];
          break;
        case FFTSIZE_4K:
          pn_degree = 11;
          pn_mask = 0x7ff;
          max_states = 4096;
          logic = &logic4k[0];
          xor_size = 2;
          bitpermeven = &bitperm4keven[0];
          bitpermodd = &bitperm4kodd[0];
          break;
        case FFTSIZE_8K:
        case FFTSIZE_8K_T2GI:
          pn_degree = 12;
          pn_mask = 0xfff;
          max_states = 8192;
          logic = &logic8k[0];
          xor_size = 4;
          bitpermeven = &bitperm8keven[0];
          bitpermodd = &bitperm8kodd[0];
          break;
        case FFTSIZE_16K:
        case FFTSIZE_16K_T2GI:
          pn_degree = 13;
          pn_mask = 0x1fff;
          max_states = 16384;
          logic = &logic16k[0];
          xor_size = 6;
          bitpermeven = &bitperm16keven[0];
          bitpermodd = &bitperm16kodd[0];
          break;
        case FFTSIZE_32K:
        case FFTSIZE_32K_T2GI:
          pn_degree = 14;
          pn_mask = 0x3fff;
          max_states = 32768;
          logic = &logic32k[0];
          xor_size = 4;
          bitpermeven = &bitperm32k[0];
          bitpermodd = &bitperm32k[0];
          break;
        default:
          pn_degree = 0;
          pn_mask = 0;
          max_states = 0;
          logic = &logic1k[0];
          xor_size = 0;
          bitpermeven = &bitperm1keven[0];
          bitpermodd = &bitperm1kodd[0];
          break;
      }
      switch (fftsize) {
        case FFTSIZE_1K:
          switch (pilotpattern) {
            case PILOT_PP1:
              C_DATA = 764;
              N_FC = 568;
              C_FC = 402;
              break;
            case PILOT_PP2:
              C_DATA = 768;
              N_FC = 710;
              C_FC = 654;
              break;
            case PILOT_PP3:
              C_DATA = 798;
              N_FC = 710;
              C_FC = 490;
              break;
            case PILOT_PP4:
              C_DATA = 804;
              N_FC = 780;
              C_FC = 707;
              break;
            case PILOT_PP5:
              C_DATA = 818;
              N_FC = 780;
              C_FC = 544;
              break;
            case PILOT_PP6:
              C_DATA = 0;
              N_FC = 0;
              C_FC = 0;
              break;
            case PILOT_PP7:
              C_DATA = 0;
              N_FC = 0;
              C_FC = 0;
              break;
            case PILOT_PP8:
              C_DATA = 0;
              N_FC = 0;
              C_FC = 0;
              break;
          }
          if (paprmode == PAPR_TR || paprmode == PAPR_BOTH) {
            if (C_DATA != 0) {
              C_DATA -= 10;
            }
            if (N_FC != 0) {
              N_FC -= 10;
            }
            if (C_FC != 0) {
              C_FC -= 10;
            }
          }
          break;
        case FFTSIZE_2K:
          switch (pilotpattern) {
            case PILOT_PP1:
              C_DATA = 1522;
              N_FC = 1136;
              C_FC = 804;
              break;
            case PILOT_PP2:
              C_DATA = 1532;
              N_FC = 1420;
              C_FC = 1309;
              break;
            case PILOT_PP3:
              C_DATA = 1596;
              N_FC = 1420;
              C_FC = 980;
              break;
            case PILOT_PP4:
              C_DATA = 1602;
              N_FC = 1562;
              C_FC = 1415;
              break;
            case PILOT_PP5:
              C_DATA = 1632;
              N_FC = 1562;
              C_FC = 1088;
              break;
            case PILOT_PP6:
              C_DATA = 0;
              N_FC = 0;
              C_FC = 0;
              break;
            case PILOT_PP7:
              C_DATA = 1646;
              N_FC = 1632;
              C_FC = 1396;
              break;
            case PILOT_PP8:
              C_DATA = 0;
              N_FC = 0;
              C_FC = 0;
              break;
          }
          if (paprmode == PAPR_TR || paprmode == PAPR_BOTH) {
            if (C_DATA != 0) {
              C_DATA -= 18;
            }
            if (N_FC != 0) {
              N_FC -= 18;
            }
            if (C_FC != 0) {
              C_FC -= 18;
            }
          }
          break;
        case FFTSIZE_4K:
          switch (pilotpattern) {
            case PILOT_PP1:
              C_DATA = 3084;
              N_FC = 2272;
              C_FC = 1609;
              break;
            case PILOT_PP2:
              C_DATA = 3092;
              N_FC = 2840;
              C_FC = 2619;
              break;
            case PILOT_PP3:
              C_DATA = 3228;
              N_FC = 2840;
              C_FC = 1961;
              break;
            case PILOT_PP4:
              C_DATA = 3234;
              N_FC = 3124;
              C_FC = 2831;
              break;
            case PILOT_PP5:
              C_DATA = 3298;
              N_FC = 3124;
              C_FC = 2177;
              break;
            case PILOT_PP6:
              C_DATA = 0;
              N_FC = 0;
              C_FC = 0;
              break;
            case PILOT_PP7:
              C_DATA = 3328;
              N_FC = 3266;
              C_FC = 2792;
              break;
            case PILOT_PP8:
              C_DATA = 0;
              N_FC = 0;
              C_FC = 0;
              break;
          }
          if (paprmode == PAPR_TR || paprmode == PAPR_BOTH) {
            if (C_DATA != 0) {
              C_DATA -= 36;
            }
            if (N_FC != 0) {
              N_FC -= 36;
            }
            if (C_FC != 0) {
              C_FC -= 36;
            }
          }
          break;
        case FFTSIZE_8K:
        case FFTSIZE_8K_T2GI:
          if (carriermode == CARRIERS_NORMAL) {
            switch (pilotpattern) {
              case PILOT_PP1:
                C_DATA = 6208;
                N_FC = 4544;
                C_FC = 3218;
                break;
              case PILOT_PP2:
                C_DATA = 6214;
                N_FC = 5680;
                C_FC = 5238;
                break;
              case PILOT_PP3:
                C_DATA = 6494;
                N_FC = 5680;
                C_FC = 3922;
                break;
              case PILOT_PP4:
                C_DATA = 6498;
                N_FC = 6248;
                C_FC = 5662;
                break;
              case PILOT_PP5:
                C_DATA = 6634;
                N_FC = 6248;
                C_FC = 4354;
                break;
              case PILOT_PP6:
                C_DATA = 0;
                N_FC = 0;
                C_FC = 0;
                break;
              case PILOT_PP7:
                C_DATA = 6698;
                N_FC = 6532;
                C_FC = 5585;
                break;
              case PILOT_PP8:
                C_DATA = 6698;
                N_FC = 0;
                C_FC = 0;
                break;
            }
          }
          else {
            switch (pilotpattern) {
              case PILOT_PP1:
                C_DATA = 6296;
                N_FC = 4608;
                C_FC = 3264;
                break;
              case PILOT_PP2:
                C_DATA = 6298;
                N_FC = 5760;
                C_FC = 5312;
                break;
              case PILOT_PP3:
                C_DATA = 6584;
                N_FC = 5760;
                C_FC = 3978;
                break;
              case PILOT_PP4:
                C_DATA = 6588;
                N_FC = 6336;
                C_FC = 5742;
                break;
              case PILOT_PP5:
                C_DATA = 6728;
                N_FC = 6336;
                C_FC = 4416;
                break;
              case PILOT_PP6:
                C_DATA = 0;
                N_FC = 0;
                C_FC = 0;
                break;
              case PILOT_PP7:
                C_DATA = 6788;
                N_FC = 6624;
                C_FC = 5664;
                break;
              case PILOT_PP8:
                C_DATA = 6788;
                N_FC = 0;
                C_FC = 0;
                break;
            }
          }
          if (paprmode == PAPR_TR || paprmode == PAPR_BOTH) {
            if (C_DATA != 0) {
              C_DATA -= 72;
            }
            if (N_FC != 0) {
              N_FC -= 72;
            }
            if (C_FC != 0) {
              C_FC -= 72;
            }
          }
          break;
        case FFTSIZE_16K:
        case FFTSIZE_16K_T2GI:
          if (carriermode == CARRIERS_NORMAL) {
            switch (pilotpattern) {
              case PILOT_PP1:
                C_DATA = 12418;
                N_FC = 9088;
                C_FC = 6437;
                break;
              case PILOT_PP2:
                C_DATA = 12436;
                N_FC = 11360;
                C_FC = 10476;
                break;
              case PILOT_PP3:
                C_DATA = 12988;
                N_FC = 11360;
                C_FC = 7845;
                break;
              case PILOT_PP4:
                C_DATA = 13002;
                N_FC = 12496;
                C_FC = 11324;
                break;
              case PILOT_PP5:
                C_DATA = 13272;
                N_FC = 12496;
                C_FC = 8709;
                break;
              case PILOT_PP6:
                C_DATA = 13288;
                N_FC = 13064;
                C_FC = 11801;
                break;
              case PILOT_PP7:
                C_DATA = 13416;
                N_FC = 13064;
                C_FC = 11170;
                break;
              case PILOT_PP8:
                C_DATA = 13406;
                N_FC = 0;
                C_FC = 0;
                break;
            }
          }
          else {
            switch (pilotpattern) {
              case PILOT_PP1:
                C_DATA = 12678;
                N_FC = 9280;
                C_FC = 6573;
                break;
              case PILOT_PP2:
                C_DATA = 12698;
                N_FC = 11600;
                C_FC = 10697;
                break;
              case PILOT_PP3:
                C_DATA = 13262;
                N_FC = 11600;
                C_FC = 8011;
                break;
              case PILOT_PP4:
                C_DATA = 13276;
                N_FC = 12760;
                C_FC = 11563;
                break;
              case PILOT_PP5:
                C_DATA = 13552;
                N_FC = 12760;
                C_FC = 8893;
                break;
              case PILOT_PP6:
                C_DATA = 13568;
                N_FC = 13340;
                C_FC = 12051;
                break;
              case PILOT_PP7:
                C_DATA = 13698;
                N_FC = 13340;
                C_FC = 11406;
                break;
              case PILOT_PP8:
                C_DATA = 13688;
                N_FC = 0;
                C_FC = 0;
                break;
            }
          }
          if (paprmode == PAPR_TR || paprmode == PAPR_BOTH) {
            if (C_DATA != 0) {
              C_DATA -= 144;
            }
            if (N_FC != 0) {
               N_FC -= 144;
            }
            if (C_FC != 0) {
               C_FC -= 144;
            }
          }
          break;
        case FFTSIZE_32K:
        case FFTSIZE_32K_T2GI:
          if (carriermode == CARRIERS_NORMAL) {
            switch (pilotpattern) {
              case PILOT_PP1:
                C_DATA = 0;
                N_FC = 0;
                C_FC = 0;
                break;
              case PILOT_PP2:
                C_DATA = 24886;
                N_FC = 22720;
                C_FC = 20952;
                break;
              case PILOT_PP3:
                C_DATA = 0;
                N_FC = 0;
                C_FC = 0;
                break;
              case PILOT_PP4:
                C_DATA = 26022;
                N_FC = 24992;
                C_FC = 22649;
                break;
              case PILOT_PP5:
                C_DATA = 0;
                N_FC = 0;
                C_FC = 0;
                break;
              case PILOT_PP6:
                C_DATA = 26592;
                N_FC = 26128;
                C_FC = 23603;
                break;
              case PILOT_PP7:
                C_DATA = 26836;
                N_FC = 0;
                C_FC = 0;
                break;
              case PILOT_PP8:
                C_DATA = 26812;
                N_FC = 0;
                C_FC = 0;
                break;
            }
          }
          else {
            switch (pilotpattern) {
              case PILOT_PP1:
                C_DATA = 0;
                N_FC = 0;
                C_FC = 0;
                break;
              case PILOT_PP2:
                C_DATA = 25412;
                N_FC = 23200;
                C_FC = 21395;
                break;
              case PILOT_PP3:
                C_DATA = 0;
                N_FC = 0;
                C_FC = 0;
                break;
              case PILOT_PP4:
                C_DATA = 26572;
                N_FC = 25520;
                C_FC = 23127;
                break;
              case PILOT_PP5:
                C_DATA = 0;
                N_FC = 0;
                C_FC = 0;
                break;
              case PILOT_PP6:
                C_DATA = 27152;
                N_FC = 26680;
                C_FC = 24102;
                break;
              case PILOT_PP7:
                C_DATA = 27404;
                N_FC = 0;
                C_FC = 0;
                break;
              case PILOT_PP8:
                C_DATA = 27376;
                N_FC = 0;
                C_FC = 0;
                break;
            }
          }
          if (paprmode == PAPR_TR || paprmode == PAPR_BOTH) {
            if (C_DATA != 0) {
              C_DATA -= 288;
            }
            if (N_FC != 0) {
              N_FC -= 288;
            }
            if (C_FC != 0) {
              C_FC -= 288;
            }
          }
          break;
      }
      if ((preamble == PREAMBLE_T2_SISO) || (preamble == PREAMBLE_T2_LITE_SISO)) {
        if (guardinterval == GI_1_128 && pilotpattern == PILOT_PP7) {
          N_FC = 0;
          C_FC = 0;
        }
        if (guardinterval == GI_1_32 && pilotpattern == PILOT_PP4) {
          N_FC = 0;
          C_FC = 0;
        }
        if (guardinterval == GI_1_16 && pilotpattern == PILOT_PP2) {
          N_FC = 0;
          C_FC = 0;
        }
        if (guardinterval == GI_19_256 && pilotpattern == PILOT_PP2) {
          N_FC = 0;
          C_FC = 0;
        }
      }
      for (int i = 0; i < max_states; i++) {
        if (i == 0 || i == 1) {
          lfsr = 0;
        }
        else if (i == 2) {
          lfsr = 1;
        }
        else {
          result = 0;
          for (int k = 0; k < xor_size; k++) {
            result ^= (lfsr >> logic[k]) & 1;
          }
          lfsr &= pn_mask;
          lfsr >>= 1;
          lfsr |= result << (pn_degree - 1);
        }
        even = 0;
        odd = 0;
        for (int n = 0; n < pn_degree; n++) {
          even |= ((lfsr >> n) & 0x1) << bitpermeven[n];
        }
        for (int n = 0; n < pn_degree; n++) {
          odd |= ((lfsr >> n) & 0x1) << bitpermodd[n];
        }
        even = even + ((i % 2) * (max_states / 2));
        odd = odd + ((i % 2) * (max_states / 2));
        if (even < C_DATA) {
          Heven[q_even++] = even;
        }
        if (odd < C_DATA) {
          Hodd[q_odd++] = odd;
        }
        if (even < C_P2) {
          HevenP2[q_evenP2++] = even;
        }
        if (odd < C_P2) {
          HoddP2[q_oddP2++] = odd;
        }
        if (even < N_FC) {
          HevenFC[q_evenFC++] = even;
        }
        if (odd < N_FC) {
          HoddFC[q_oddFC++] = odd;
        }
      }
      if (fftsize == FFTSIZE_32K || fftsize == FFTSIZE_32K_T2GI) {
        for (int j = 0; j < q_odd; j++) {
          int a;
          a = Hodd[j];
          Heven[a] = j;
        }
        for (int j = 0; j < q_oddP2; j++) {
          int a;
          a = HoddP2[j];
          HevenP2[a] = j;
        }
        for (int j = 0; j < q_oddFC; j++) {
          int a;
          a = HoddFC[j];
          HevenFC[a] = j;
        }
      }
      if (N_FC == 0) {
        set_output_multiple((N_P2 * C_P2) + (numdatasyms * C_DATA));
        interleaved_items = (N_P2 * C_P2) + (numdatasyms * C_DATA);
        num_data_symbols = numdatasyms;
      }
      else {
        set_output_multiple((N_P2 * C_P2) + ((numdatasyms - 1) * C_DATA) + N_FC);
        interleaved_items = (N_P2 * C_P2) + ((numdatasyms - 1) * C_DATA) + N_FC;
        num_data_symbols = numdatasyms - 1;
      }
    }

    /*
     * Our virtual destructor.
     */
    dvbt2_freqinterleaver_cc_impl::~dvbt2_freqinterleaver_cc_impl()
    {
    }

    int
    dvbt2_freqinterleaver_cc_impl::work(int noutput_items,
                          gr_vector_const_void_star &input_items,
                          gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];
      int symbol = 0;
      int *H;

      for (int i = 0; i < noutput_items; i += interleaved_items) {
        for (int j = 0; j < N_P2; j++) {
          if ((symbol % 2) == 0) {
            H = HevenP2;
          }
          else {
            H = HoddP2;
          }
          for (int j = 0; j < C_P2; j++) {
            *out++ = in[H[j]];
          }
          symbol++;
          in += C_P2;
        }
        for (int j = 0; j < num_data_symbols; j++) {
          if ((symbol % 2) == 0) {
            H = Heven;
          }
          else {
            H = Hodd;
          }
          for (int j = 0; j < C_DATA; j++) {
            *out++ = in[H[j]];
          }
          symbol++;
          in += C_DATA;
        }
        if (N_FC != 0) {
          if ((symbol % 2) == 0) {
            H = HevenFC;
          }
          else {
            H = HoddFC;
          }
          for (int j = 0; j < N_FC; j++) {
            *out++ = in[H[j]];
          }
          symbol++;
          in += N_FC;
        }
      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

    const int dvbt2_freqinterleaver_cc_impl::bitperm1keven[9] = 
    {
      8, 7, 6, 5, 0, 1, 2, 3, 4
    };

    const int dvbt2_freqinterleaver_cc_impl::bitperm1kodd[9] = 
    {
      6, 8, 7, 4, 1, 0, 5, 2, 3
    };

    const int dvbt2_freqinterleaver_cc_impl::bitperm2keven[10] = 
    {
      4, 3, 9, 6, 2, 8, 1, 5, 7, 0
    };

    const int dvbt2_freqinterleaver_cc_impl::bitperm2kodd[10] = 
    {
     6, 9, 4, 8, 5, 1, 0, 7, 2, 3
    };

    const int dvbt2_freqinterleaver_cc_impl::bitperm4keven[11] = 
    {
      6, 3, 0, 9, 4, 2, 1, 8, 5, 10, 7
    };

    const int dvbt2_freqinterleaver_cc_impl::bitperm4kodd[11] = 
    {
      5, 9, 1, 4, 3, 0, 8, 10, 7, 2, 6
    };

    const int dvbt2_freqinterleaver_cc_impl::bitperm8keven[12] = 
    {
      7, 1, 4, 2, 9, 6, 8, 10, 0, 3, 11, 5
    };

    const int dvbt2_freqinterleaver_cc_impl::bitperm8kodd[12] = 
    {
      11, 4, 9, 3, 1, 2, 5, 0, 6, 7, 10, 8
    };

    const int dvbt2_freqinterleaver_cc_impl::bitperm16keven[13] = 
    {
      9, 7, 6, 10, 12, 5, 1, 11, 0, 2, 3, 4, 8
    };

    const int dvbt2_freqinterleaver_cc_impl::bitperm16kodd[13] = 
    {
      6, 8, 10, 12, 2, 0, 4, 1, 11, 3, 5, 9, 7
    };

    const int dvbt2_freqinterleaver_cc_impl::bitperm32k[14] = 
    {
      7, 13, 3, 4, 9, 2, 12, 11, 1, 8, 10, 0, 5, 6
    };

  } /* namespace dtv */
} /* namespace gr */


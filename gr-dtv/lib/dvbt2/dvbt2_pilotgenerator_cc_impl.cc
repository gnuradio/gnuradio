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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "dvbt2_pilotgenerator_cc_impl.h"
#include <volk/volk.h>

namespace gr {
  namespace dtv {

    dvbt2_pilotgenerator_cc::sptr
    dvbt2_pilotgenerator_cc::make(dvbt2_extended_carrier_t carriermode, dvbt2_fftsize_t fftsize, dvbt2_pilotpattern_t pilotpattern, dvb_guardinterval_t guardinterval, int numdatasyms, dvbt2_papr_t paprmode, dvbt2_version_t version, dvbt2_preamble_t preamble, dvbt2_misogroup_t misogroup, dvbt2_equalization_t equalization, dvbt2_bandwidth_t bandwidth, int vlength)
    {
      return gnuradio::get_initial_sptr
        (new dvbt2_pilotgenerator_cc_impl(carriermode, fftsize, pilotpattern, guardinterval, numdatasyms, paprmode, version, preamble, misogroup, equalization, bandwidth, vlength));
    }

    /*
     * The private constructor
     */
    dvbt2_pilotgenerator_cc_impl::dvbt2_pilotgenerator_cc_impl(dvbt2_extended_carrier_t carriermode, dvbt2_fftsize_t fftsize, dvbt2_pilotpattern_t pilotpattern, dvb_guardinterval_t guardinterval, int numdatasyms, dvbt2_papr_t paprmode, dvbt2_version_t version, dvbt2_preamble_t preamble, dvbt2_misogroup_t misogroup, dvbt2_equalization_t equalization, dvbt2_bandwidth_t bandwidth, int vlength)
      : gr::block("dvbt2_pilotgenerator_cc",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(gr_complex) * vlength))
    {
      int step, ki;
      double x, sinc, sincrms = 0.0;
      double fs, fstep, f = 0.0;
      miso_group = misogroup;
      if ((preamble == PREAMBLE_T2_SISO) || (preamble == PREAMBLE_T2_LITE_SISO)) {
        miso = FALSE;
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
        miso = TRUE;
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
          C_PS = 853;
          K_EXT = 0;
          K_OFFSET = 0;
          break;
        case FFTSIZE_2K:
          C_PS = 1705;
          K_EXT = 0;
          K_OFFSET = 0;
          break;
        case FFTSIZE_4K:
          C_PS = 3409;
          K_EXT = 0;
          K_OFFSET = 0;
          break;
        case FFTSIZE_8K:
        case FFTSIZE_8K_T2GI:
          if (carriermode == CARRIERS_NORMAL) {
            C_PS = 6817;
            K_EXT = 0;
            K_OFFSET = 48;
          }
          else {
            C_PS = 6913;
            K_EXT = 48;
            K_OFFSET = 0;
          }
          break;
        case FFTSIZE_16K:
        case FFTSIZE_16K_T2GI:
          if (carriermode == CARRIERS_NORMAL) {
            C_PS = 13633;
            K_EXT = 0;
            K_OFFSET = 144;
          }
          else {
            C_PS = 13921;
            K_EXT = 144;
            K_OFFSET = 0;
          }
          break;
        case FFTSIZE_32K:
        case FFTSIZE_32K_T2GI:
          if (carriermode == CARRIERS_NORMAL) {
            C_PS = 27265;
            K_EXT = 0;
            K_OFFSET = 288;
          }
          else {
            C_PS = 27841;
            K_EXT = 288;
            K_OFFSET = 0;
          }
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
      if (miso == FALSE) {
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
      init_prbs();
      for (int i = 0; i < C_PS; i++) {
        p2_carrier_map[i] = DATA_CARRIER;
      }
      if ((fftsize == FFTSIZE_32K || fftsize == FFTSIZE_32K_T2GI) && (miso == FALSE)) {
        step = 6;
      }
      else {
        step = 3;
      }
      for (int i = 0; i < C_PS; i += step) {
        if (miso == TRUE && miso_group == MISO_TX2) {
          if (((i / 3) % 2) && (i % 3 == 0)) {
            p2_carrier_map[i] = P2PILOT_CARRIER_INVERTED;
          }
          else {
            p2_carrier_map[i] = P2PILOT_CARRIER;
          }
        }
        else {
          p2_carrier_map[i] = P2PILOT_CARRIER;
        }
      }
      if (carriermode == CARRIERS_EXTENDED) {
        for (int i = 0; i < K_EXT; i++) {
          if (miso == TRUE && miso_group == MISO_TX2) {
            if (((i / 3) % 2) && (i % 3 == 0)) {
              p2_carrier_map[i] = P2PILOT_CARRIER_INVERTED;
            }
            else {
              p2_carrier_map[i] = P2PILOT_CARRIER;
            }
            if ((((i + (C_PS - K_EXT)) / 3) % 2) && ((i + (C_PS - K_EXT)) % 3 == 0)) {
              p2_carrier_map[i + (C_PS - K_EXT)] = P2PILOT_CARRIER_INVERTED;
            }
            else {
              p2_carrier_map[i + (C_PS - K_EXT)] = P2PILOT_CARRIER;
            }
          }
          else {
            p2_carrier_map[i] = P2PILOT_CARRIER;
            p2_carrier_map[i + (C_PS - K_EXT)] = P2PILOT_CARRIER;
          }
        }
      }
      if (miso == TRUE) {
        p2_carrier_map[K_EXT + 1] = P2PILOT_CARRIER;
        p2_carrier_map[K_EXT + 2] = P2PILOT_CARRIER;
        p2_carrier_map[C_PS - K_EXT - 2] = P2PILOT_CARRIER;
        p2_carrier_map[C_PS - K_EXT - 3] = P2PILOT_CARRIER;
      }
      switch (fftsize) {
        case FFTSIZE_1K:
          for (int i = 0; i < 10; i++) {
            p2_carrier_map[p2_papr_map_1k[i]] = P2PAPR_CARRIER;
          }
          if (miso == TRUE) {
            for (int i = 0; i < 10; i++) {
              ki = p2_papr_map_1k[i] + K_EXT;
              if (i < 9) {
                if (((ki % 3) == 1) && ((ki + 1) != (p2_papr_map_1k[i + 1] + K_EXT))) {
                  p2_carrier_map[ki + 1] = P2PILOT_CARRIER;
                }
              }
              else {
                if ((ki % 3) == 1) {
                  p2_carrier_map[ki + 1] = P2PILOT_CARRIER;
                }
              }
              if (i > 0) {
                if (((ki % 3) == 2) && ((ki - 1) != (p2_papr_map_1k[i - 1] + K_EXT))) {
                  p2_carrier_map[ki - 1] = P2PILOT_CARRIER;
                }
              }
              else {
                if ((ki % 3) == 2) {
                  p2_carrier_map[ki - 1] = P2PILOT_CARRIER;
                }
              }
            }
          }
          cp_bpsk[0] = gr_complex(4.0 / 3.0, 0.0);
          cp_bpsk[1] = gr_complex(-4.0 / 3.0, 0.0);
          cp_bpsk_inverted[0] = gr_complex(-4.0 / 3.0, 0.0);
          cp_bpsk_inverted[1] = gr_complex(4.0 / 3.0, 0.0);
          break;
        case FFTSIZE_2K:
          for (int i = 0; i < 18; i++) {
            p2_carrier_map[p2_papr_map_2k[i]] = P2PAPR_CARRIER;
          }
          if (miso == TRUE) {
            for (int i = 0; i < 18; i++) {
              ki = p2_papr_map_2k[i] + K_EXT;
              if (i < 17) {
                if (((ki % 3) == 1) && ((ki + 1) != (p2_papr_map_2k[i + 1] + K_EXT))) {
                  p2_carrier_map[ki + 1] = P2PILOT_CARRIER;
                }
              }
              else {
                if ((ki % 3) == 1) {
                  p2_carrier_map[ki + 1] = P2PILOT_CARRIER;
                }
              }
              if (i > 0) {
                if (((ki % 3) == 2) && ((ki - 1) != (p2_papr_map_2k[i - 1] + K_EXT))) {
                  p2_carrier_map[ki - 1] = P2PILOT_CARRIER;
                }
              }
              else {
                if ((ki % 3) == 2) {
                  p2_carrier_map[ki - 1] = P2PILOT_CARRIER;
                }
              }
            }
          }
          cp_bpsk[0] = gr_complex(4.0 / 3.0, 0.0);
          cp_bpsk[1] = gr_complex(-4.0 / 3.0, 0.0);
          cp_bpsk_inverted[0] = gr_complex(-4.0 / 3.0, 0.0);
          cp_bpsk_inverted[1] = gr_complex(4.0 / 3.0, 0.0);
          break;
        case FFTSIZE_4K:
          for (int i = 0; i < 36; i++) {
            p2_carrier_map[p2_papr_map_4k[i]] = P2PAPR_CARRIER;
          }
          if (miso == TRUE) {
            for (int i = 0; i < 36; i++) {
              ki = p2_papr_map_4k[i] + K_EXT;
              if (i < 35) {
                if (((ki % 3) == 1) && ((ki + 1) != (p2_papr_map_4k[i + 1] + K_EXT))) {
                  p2_carrier_map[ki + 1] = P2PILOT_CARRIER;
                }
              }
              else {
                if ((ki % 3) == 1) {
                  p2_carrier_map[ki + 1] = P2PILOT_CARRIER;
                }
              }
              if (i > 0) {
                if (((ki % 3) == 2) && ((ki - 1) != (p2_papr_map_4k[i - 1] + K_EXT))) {
                  p2_carrier_map[ki - 1] = P2PILOT_CARRIER;
                }
              }
              else {
                if ((ki % 3) == 2) {
                  p2_carrier_map[ki - 1] = P2PILOT_CARRIER;
                }
              }
            }
          }
          cp_bpsk[0] = gr_complex((4.0 * std::sqrt(2.0)) / 3.0, 0.0);
          cp_bpsk[1] = gr_complex(-(4.0 * std::sqrt(2.0)) / 3.0, 0.0);
          cp_bpsk_inverted[0] = gr_complex(-(4.0 * std::sqrt(2.0)) / 3.0, 0.0);
          cp_bpsk_inverted[1] = gr_complex((4.0 * std::sqrt(2.0)) / 3.0, 0.0);
          break;
        case FFTSIZE_8K:
        case FFTSIZE_8K_T2GI:
          for (int i = 0; i < 72; i++) {
            p2_carrier_map[p2_papr_map_8k[i] + K_EXT] = P2PAPR_CARRIER;
          }
          if (miso == TRUE) {
            for (int i = 0; i < 72; i++) {
              ki = p2_papr_map_8k[i] + K_EXT;
              if (i < 71) {
                if (((ki % 3) == 1) && ((ki + 1) != (p2_papr_map_8k[i + 1] + K_EXT))) {
                  p2_carrier_map[ki + 1] = P2PILOT_CARRIER;
                }
              }
              else {
                if ((ki % 3) == 1) {
                  p2_carrier_map[ki + 1] = P2PILOT_CARRIER;
                }
              }
              if (i > 0) {
                if (((ki % 3) == 2) && ((ki - 1) != (p2_papr_map_8k[i - 1] + K_EXT))) {
                  p2_carrier_map[ki - 1] = P2PILOT_CARRIER;
                }
              }
              else {
                if ((ki % 3) == 2) {
                  p2_carrier_map[ki - 1] = P2PILOT_CARRIER;
                }
              }
            }
          }
          cp_bpsk[0] = gr_complex(8.0 / 3.0, 0.0);
          cp_bpsk[1] = gr_complex(-8.0 / 3.0, 0.0);
          cp_bpsk_inverted[0] = gr_complex(-8.0 / 3.0, 0.0);
          cp_bpsk_inverted[1] = gr_complex(8.0 / 3.0, 0.0);
          break;
        case FFTSIZE_16K:
        case FFTSIZE_16K_T2GI:
          for (int i = 0; i < 144; i++) {
            p2_carrier_map[p2_papr_map_16k[i] + K_EXT] = P2PAPR_CARRIER;
          }
          if (miso == TRUE) {
            for (int i = 0; i < 144; i++) {
              ki = p2_papr_map_16k[i] + K_EXT;
              if (i < 143) {
                if (((ki % 3) == 1) && ((ki + 1) != (p2_papr_map_16k[i + 1] + K_EXT))) {
                  p2_carrier_map[ki + 1] = P2PILOT_CARRIER;
                }
              }
              else {
                if ((ki % 3) == 1) {
                  p2_carrier_map[ki + 1] = P2PILOT_CARRIER;
                }
              }
              if (i > 0) {
                if (((ki % 3) == 2) && ((ki - 1) != (p2_papr_map_16k[i - 1] + K_EXT))) {
                  p2_carrier_map[ki - 1] = P2PILOT_CARRIER;
                }
              }
              else {
                if ((ki % 3) == 2) {
                  p2_carrier_map[ki - 1] = P2PILOT_CARRIER;
                }
              }
            }
          }
          cp_bpsk[0] = gr_complex(8.0 / 3.0, 0.0);
          cp_bpsk[1] = gr_complex(-8.0 / 3.0, 0.0);
          cp_bpsk_inverted[0] = gr_complex(-8.0 / 3.0, 0.0);
          cp_bpsk_inverted[1] = gr_complex(8.0 / 3.0, 0.0);
          break;
        case FFTSIZE_32K:
        case FFTSIZE_32K_T2GI:
          for (int i = 0; i < 288; i++) {
            p2_carrier_map[p2_papr_map_32k[i] + K_EXT] = P2PAPR_CARRIER;
          }
          if (miso == TRUE) {
            for (int i = 0; i < 288; i++) {
              ki = p2_papr_map_32k[i] + K_EXT;
              if (i < 287) {
                if (((ki % 3) == 1) && ((ki + 1) != (p2_papr_map_32k[i + 1] + K_EXT))) {
                  p2_carrier_map[ki + 1] = P2PILOT_CARRIER;
                }
              }
              else {
                if ((ki % 3) == 1) {
                  p2_carrier_map[ki + 1] = P2PILOT_CARRIER;
                }
              }
              if (i > 0) {
                if (((ki % 3) == 2) && ((ki - 1) != (p2_papr_map_32k[i - 1] + K_EXT))) {
                  p2_carrier_map[ki - 1] = P2PILOT_CARRIER;
                }
              }
              else {
                if ((ki % 3) == 2) {
                  p2_carrier_map[ki - 1] = P2PILOT_CARRIER;
                }
              }
            }
          }
          cp_bpsk[0] = gr_complex(8.0 / 3.0, 0.0);
          cp_bpsk[1] = gr_complex(-8.0 / 3.0, 0.0);
          cp_bpsk_inverted[0] = gr_complex(-8.0 / 3.0, 0.0);
          cp_bpsk_inverted[1] = gr_complex(8.0 / 3.0, 0.0);
          break;
      }
      switch (pilotpattern) {
        case PILOT_PP1:
          sp_bpsk[0] = gr_complex(4.0 / 3.0, 0.0);
          sp_bpsk[1] = gr_complex(-4.0 / 3.0, 0.0);
          sp_bpsk_inverted[0] = gr_complex(-4.0 / 3.0, 0.0);
          sp_bpsk_inverted[1] = gr_complex(4.0 / 3.0, 0.0);
          dx = 3;
          dy = 4;
          break;
        case PILOT_PP2:
          sp_bpsk[0] = gr_complex(4.0 / 3.0, 0.0);
          sp_bpsk[1] = gr_complex(-4.0 / 3.0, 0.0);
          sp_bpsk_inverted[0] = gr_complex(-4.0 / 3.0, 0.0);
          sp_bpsk_inverted[1] = gr_complex(4.0 / 3.0, 0.0);
          dx = 6;
          dy = 2;
          break;
        case PILOT_PP3:
          sp_bpsk[0] = gr_complex(7.0 / 4.0, 0.0);
          sp_bpsk[1] = gr_complex(-7.0 / 4.0, 0.0);
          sp_bpsk_inverted[0] = gr_complex(-7.0 / 4.0, 0.0);
          sp_bpsk_inverted[1] = gr_complex(7.0 / 4.0, 0.0);
          dx = 6;
          dy = 4;
          break;
        case PILOT_PP4:
          sp_bpsk[0] = gr_complex(7.0 / 4.0, 0.0);
          sp_bpsk[1] = gr_complex(-7.0 / 4.0, 0.0);
          sp_bpsk_inverted[0] = gr_complex(-7.0 / 4.0, 0.0);
          sp_bpsk_inverted[1] = gr_complex(7.0 / 4.0, 0.0);
          dx = 12;
          dy = 2;
          break;
        case PILOT_PP5:
          sp_bpsk[0] = gr_complex(7.0 / 3.0, 0.0);
          sp_bpsk[1] = gr_complex(-7.0 / 3.0, 0.0);
          sp_bpsk_inverted[0] = gr_complex(-7.0 / 3.0, 0.0);
          sp_bpsk_inverted[1] = gr_complex(7.0 / 3.0, 0.0);
          dx = 12;
          dy = 4;
          break;
        case PILOT_PP6:
          sp_bpsk[0] = gr_complex(7.0 / 3.0, 0.0);
          sp_bpsk[1] = gr_complex(-7.0 / 3.0, 0.0);
          sp_bpsk_inverted[0] = gr_complex(-7.0 / 3.0, 0.0);
          sp_bpsk_inverted[1] = gr_complex(7.0 / 3.0, 0.0);
          dx = 24;
          dy = 2;
          break;
        case PILOT_PP7:
          sp_bpsk[0] = gr_complex(7.0 / 3.0, 0.0);
          sp_bpsk[1] = gr_complex(-7.0 / 3.0, 0.0);
          sp_bpsk_inverted[0] = gr_complex(-7.0 / 3.0, 0.0);
          sp_bpsk_inverted[1] = gr_complex(7.0 / 3.0, 0.0);
          dx = 24;
          dy = 4;
          break;
        case PILOT_PP8:
          sp_bpsk[0] = gr_complex(7.0 / 3.0, 0.0);
          sp_bpsk[1] = gr_complex(-7.0 / 3.0, 0.0);
          sp_bpsk_inverted[0] = gr_complex(-7.0 / 3.0, 0.0);
          sp_bpsk_inverted[1] = gr_complex(7.0 / 3.0, 0.0);
          dx = 6;
          dy = 16;
          break;
      }
      for (int i = 0; i < C_PS; i++) {
        fc_carrier_map[i] = DATA_CARRIER;
      }
      for (int i = 0; i < C_PS; i++) {
        if (i % dx == 0) {
          if (miso == TRUE && miso_group == MISO_TX2) {
            if ((i / dx) % 2) {
              fc_carrier_map[i] = SCATTERED_CARRIER_INVERTED;
            }
            else {
              fc_carrier_map[i] = SCATTERED_CARRIER;
            }
          }
          else {
            fc_carrier_map[i] = SCATTERED_CARRIER;
          }
        }
      }
      if (fftsize == FFTSIZE_1K && pilotpattern == PILOT_PP4) {
        fc_carrier_map[C_PS - 2] = SCATTERED_CARRIER;
      }
      else if (fftsize == FFTSIZE_1K && pilotpattern == PILOT_PP5) {
        fc_carrier_map[C_PS - 2] = SCATTERED_CARRIER;
      }
      else if (fftsize == FFTSIZE_2K && pilotpattern == PILOT_PP7) {
        fc_carrier_map[C_PS - 2] = SCATTERED_CARRIER;
      }
      if (miso == TRUE && miso_group == MISO_TX2) {
        if ((numdatasyms + N_P2 - 1) % 2) {
          fc_carrier_map[0] = SCATTERED_CARRIER_INVERTED;
          fc_carrier_map[C_PS - 1] = SCATTERED_CARRIER_INVERTED;
        }
        else {
          fc_carrier_map[0] = SCATTERED_CARRIER;
          fc_carrier_map[C_PS - 1] = SCATTERED_CARRIER;
        }
      }
      else {
        fc_carrier_map[0] = SCATTERED_CARRIER;
        fc_carrier_map[C_PS - 1] = SCATTERED_CARRIER;
      }
      if (paprmode == PAPR_TR || paprmode == PAPR_BOTH) {
        switch (fftsize) {
          case FFTSIZE_1K:
            for (int i = 0; i < 10; i++) {
              fc_carrier_map[p2_papr_map_1k[i]] = TRPAPR_CARRIER;
            }
            break;
          case FFTSIZE_2K:
            for (int i = 0; i < 18; i++) {
              fc_carrier_map[p2_papr_map_2k[i]] = TRPAPR_CARRIER;
            }
            break;
          case FFTSIZE_4K:
            for (int i = 0; i < 36; i++) {
              fc_carrier_map[p2_papr_map_4k[i]] = TRPAPR_CARRIER;
            }
            break;
          case FFTSIZE_8K:
          case FFTSIZE_8K_T2GI:
            for (int i = 0; i < 72; i++) {
              fc_carrier_map[p2_papr_map_8k[i] + K_EXT] = TRPAPR_CARRIER;
            }
            break;
          case FFTSIZE_16K:
          case FFTSIZE_16K_T2GI:
            for (int i = 0; i < 144; i++) {
              fc_carrier_map[p2_papr_map_16k[i] + K_EXT] = TRPAPR_CARRIER;
            }
            break;
          case FFTSIZE_32K:
          case FFTSIZE_32K_T2GI:
            for (int i = 0; i < 288; i++) {
              fc_carrier_map[p2_papr_map_32k[i] + K_EXT] = TRPAPR_CARRIER;
            }
            break;
        }
      }
      if (N_FC == 0) {
        active_items = (N_P2 * C_P2) + (numdatasyms * C_DATA);
      }
      else {
        active_items = (N_P2 * C_P2) + ((numdatasyms - 1) * C_DATA) + N_FC;
      }
      fft_size = fftsize;
      pilot_pattern = pilotpattern;
      carrier_mode = carriermode;
      papr_mode = paprmode;
      left_nulls = ((vlength - C_PS) / 2) + 1;
      right_nulls = (vlength - C_PS) / 2;
      if ((fftsize == FFTSIZE_32K || fftsize == FFTSIZE_32K_T2GI) && (miso == FALSE)) {
        p2_bpsk[0] = gr_complex(std::sqrt(37.0) / 5.0, 0.0);
        p2_bpsk[1] = gr_complex(-(std::sqrt(37.0) / 5.0), 0.0);
        p2_bpsk_inverted[0] = gr_complex(-(std::sqrt(37.0) / 5.0), 0.0);
        p2_bpsk_inverted[1] = gr_complex(std::sqrt(37.0) / 5.0, 0.0);
      }
      else {
        p2_bpsk[0] = gr_complex(std::sqrt(31.0) / 5.0, 0.0);
        p2_bpsk[1] = gr_complex(-(std::sqrt(31.0) / 5.0), 0.0);
        p2_bpsk_inverted[0] = gr_complex(-(std::sqrt(31.0) / 5.0), 0.0);
        p2_bpsk_inverted[1] = gr_complex(std::sqrt(31.0) / 5.0, 0.0);
      }
      normalization = 5.0 / std::sqrt(27.0 * C_PS);
      switch (bandwidth) {
        case BANDWIDTH_1_7_MHZ:
          fs = 131.0 * 1000000.0 / 71.0;
          break;
        case BANDWIDTH_5_0_MHZ:
          fs = 5.0 * 8000000.0 / 7.0;
          break;
        case BANDWIDTH_6_0_MHZ:
          fs = 6.0 * 8000000.0 / 7.0;
          break;
        case BANDWIDTH_7_0_MHZ:
          fs = 7.0 * 8000000.0 / 7.0;
          break;
        case BANDWIDTH_8_0_MHZ:
          fs = 8.0 * 8000000.0 / 7.0;
          break;
        case BANDWIDTH_10_0_MHZ:
          fs = 10.0 * 8000000.0 / 7.0;
          break;
        default:
          fs = 1.0;
          break;
      }
      fstep = fs / vlength;
      for (int i = 0; i < vlength / 2; i++) {
        x = M_PI * f / fs;
        if (i == 0) {
          sinc = 1.0;
        }
        else {
          sinc = sin(x) / x;
        }
        sincrms += sinc * sinc;
        inverse_sinc[i + (vlength / 2)] = gr_complex(1.0 / sinc, 0.0);
        inverse_sinc[(vlength / 2) - i - 1] = gr_complex(1.0 / sinc, 0.0);
        f = f + fstep;
      }
      sincrms = std::sqrt(sincrms / (vlength / 2));
      for (int i = 0; i < vlength; i++) {
        inverse_sinc[i] *= sincrms;
      }
      equalization_enable = equalization;
      ofdm_fft_size = vlength;
      ofdm_fft = new (std::nothrow) fft::fft_complex(ofdm_fft_size, false, 1);
      if (ofdm_fft == NULL) {
        GR_LOG_FATAL(d_logger, "Pilot Generator and IFFT, cannot allocate memory for ofdm_fft.");
        throw std::bad_alloc();
      }
      num_symbols = numdatasyms + N_P2;
      set_output_multiple(num_symbols);
    }

    /*
     * Our virtual destructor.
     */
    dvbt2_pilotgenerator_cc_impl::~dvbt2_pilotgenerator_cc_impl()
    {
      delete ofdm_fft;
    }

    void
    dvbt2_pilotgenerator_cc_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = active_items * (noutput_items / num_symbols);
    }

    void
    dvbt2_pilotgenerator_cc_impl::init_prbs(void)
    {
      int sr = 0x7ff;
      int j = 0;

      for (int i = 0; i < MAX_CARRIERS; i++) {
        int b = ((sr) ^ (sr >> 2)) & 1;
        prbs[i] = sr & 1;
        sr >>= 1;
        if(b) {
          sr |= 0x400;
        }
      }

      for (int i = 0; i < (CHIPS / 8); i++) {
        for (int k = 7; k >= 0; k--) {
          pn_sequence[j] = (pn_sequence_table[i] >> k) & 0x1;
          j = j + 1;
        }
      }
    }

    void
    dvbt2_pilotgenerator_cc_impl::init_pilots(int symbol)
    {
      int remainder, shift;
      for (int i = 0; i < C_PS; i++) {
        data_carrier_map[i] = DATA_CARRIER;
      }
      switch (fft_size) {
        case FFTSIZE_1K:
          switch (pilot_pattern) {
            case PILOT_PP1:
              for (int i = 0; i < 20; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp1_cp1[i] % 1632) / dx)) % 2 && (((pp1_cp1[i] % 1632) % dx) == 0)) {
                    data_carrier_map[pp1_cp1[i] % 1632] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp1_cp1[i] % 1632] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp1_cp1[i] % 1632] = CONTINUAL_CARRIER;
                }
              }
              break;
            case PILOT_PP2:
              for (int i = 0; i < 20; i++) {
                data_carrier_map[pp2_cp1[i] % 1632] = CONTINUAL_CARRIER;
              }
              break;
            case PILOT_PP3:
              for (int i = 0; i < 22; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp3_cp1[i] % 1632) / dx)) % 2 && (((pp3_cp1[i] % 1632) % dx) == 0)) {
                    data_carrier_map[pp3_cp1[i] % 1632] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp3_cp1[i] % 1632] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp3_cp1[i] % 1632] = CONTINUAL_CARRIER;
                }
              }
              break;
            case PILOT_PP4:
              for (int i = 0; i < 20; i++) {
                data_carrier_map[pp4_cp1[i] % 1632] = CONTINUAL_CARRIER;
              }
              break;
            case PILOT_PP5:
              for (int i = 0; i < 19; i++) {
                data_carrier_map[pp5_cp1[i] % 1632] = CONTINUAL_CARRIER;
              }
              break;
            case PILOT_PP6:
              break;
            case PILOT_PP7:
              for (int i = 0; i < 15; i++) {
                data_carrier_map[pp7_cp1[i] % 1632] = CONTINUAL_CARRIER;
              }
              break;
            case PILOT_PP8:
              break;
          }
          break;
        case FFTSIZE_2K:
          switch (pilot_pattern) {
            case PILOT_PP1:
              for (int i = 0; i < 20; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp1_cp1[i] % 1632) / dx)) % 2 && (((pp1_cp1[i] % 1632) % dx) == 0)) {
                    data_carrier_map[pp1_cp1[i] % 1632] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp1_cp1[i] % 1632] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp1_cp1[i] % 1632] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 25; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp1_cp2[i] % 1632) / dx)) % 2 && (((pp1_cp2[i] % 1632) % dx) == 0)) {
                    data_carrier_map[pp1_cp2[i] % 1632] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp1_cp2[i] % 1632] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp1_cp2[i] % 1632] = CONTINUAL_CARRIER;
                }
              }
              break;
            case PILOT_PP2:
              for (int i = 0; i < 20; i++) {
                data_carrier_map[pp2_cp1[i] % 1632] = CONTINUAL_CARRIER;
              }
              for (int i = 0; i < 22; i++) {
                data_carrier_map[pp2_cp2[i] % 1632] = CONTINUAL_CARRIER;
              }
              break;
            case PILOT_PP3:
              for (int i = 0; i < 22; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp3_cp1[i] % 1632) / dx)) % 2 && (((pp3_cp1[i] % 1632) % dx) == 0)) {
                    data_carrier_map[pp3_cp1[i] % 1632] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp3_cp1[i] % 1632] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp3_cp1[i] % 1632] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 20; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp3_cp2[i] % 1632) / dx)) % 2 && (((pp3_cp2[i] % 1632) % dx) == 0)) {
                    data_carrier_map[pp3_cp2[i] % 1632] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp3_cp2[i] % 1632] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp3_cp2[i] % 1632] = CONTINUAL_CARRIER;
                }
              }
              break;
            case PILOT_PP4:
              for (int i = 0; i < 20; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp4_cp1[i] % 1632) / dx)) % 2 && (((pp4_cp1[i] % 1632) % dx) == 0)) {
                    data_carrier_map[pp4_cp1[i] % 1632] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp4_cp1[i] % 1632] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp4_cp1[i] % 1632] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 23; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp4_cp2[i] % 1632) / dx)) % 2 && (((pp4_cp2[i] % 1632) % dx) == 0)) {
                    data_carrier_map[pp4_cp2[i] % 1632] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp4_cp2[i] % 1632] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp4_cp2[i] % 1632] = CONTINUAL_CARRIER;
                }
              }
              break;
            case PILOT_PP5:
              for (int i = 0; i < 19; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp5_cp1[i] % 1632) / dx)) % 2 && (((pp5_cp1[i] % 1632) % dx) == 0)) {
                    data_carrier_map[pp5_cp1[i] % 1632] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp5_cp1[i] % 1632] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp5_cp1[i] % 1632] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 23; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp5_cp2[i] % 1632) / dx)) % 2 && (((pp5_cp2[i] % 1632) % dx) == 0)) {
                    data_carrier_map[pp5_cp2[i] % 1632] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp5_cp2[i] % 1632] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp5_cp2[i] % 1632] = CONTINUAL_CARRIER;
                }
              }
              break;
            case PILOT_PP6:
              break;
            case PILOT_PP7:
              for (int i = 0; i < 15; i++) {
                data_carrier_map[pp7_cp1[i] % 1632] = CONTINUAL_CARRIER;
              }
              for (int i = 0; i < 30; i++) {
                data_carrier_map[pp7_cp2[i] % 1632] = CONTINUAL_CARRIER;
              }
              break;
            case PILOT_PP8:
              break;
          }
          break;
        case FFTSIZE_4K:
          switch (pilot_pattern) {
            case PILOT_PP1:
              for (int i = 0; i < 20; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp1_cp1[i] % 3264) / dx)) % 2 && (((pp1_cp1[i] % 3264) % dx) == 0)) {
                    data_carrier_map[pp1_cp1[i] % 3264] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp1_cp1[i] % 3264] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp1_cp1[i] % 3264] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 25; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp1_cp2[i] % 3264) / dx)) % 2 && (((pp1_cp2[i] % 3264) % dx) == 0)) {
                    data_carrier_map[pp1_cp2[i] % 3264] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp1_cp2[i] % 3264] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp1_cp2[i] % 3264] = CONTINUAL_CARRIER;
                }
              }
              break;
            case PILOT_PP2:
              for (int i = 0; i < 20; i++) {
                data_carrier_map[pp2_cp1[i] % 3264] = CONTINUAL_CARRIER;
              }
              for (int i = 0; i < 22; i++) {
                data_carrier_map[pp2_cp2[i] % 3264] = CONTINUAL_CARRIER;
              }
              for (int i = 0; i < 2; i++) {
                data_carrier_map[pp2_cp3[i] % 3264] = CONTINUAL_CARRIER;
              }
              break;
            case PILOT_PP3:
              for (int i = 0; i < 22; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp3_cp1[i] % 3264) / dx)) % 2 && (((pp3_cp1[i] % 3264) % dx) == 0)) {
                    data_carrier_map[pp3_cp1[i] % 3264] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp3_cp1[i] % 3264] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp3_cp1[i] % 3264] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 20; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp3_cp2[i] % 3264) / dx)) % 2 && (((pp3_cp2[i] % 3264) % dx) == 0)) {
                    data_carrier_map[pp3_cp2[i] % 3264] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp3_cp2[i] % 3264] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp3_cp2[i] % 3264] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 1; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp3_cp3[i] % 3264) / dx)) % 2 && (((pp3_cp3[i] % 3264) % dx) == 0)) {
                    data_carrier_map[pp3_cp3[i] % 3264] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp3_cp3[i] % 3264] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp3_cp3[i] % 3264] = CONTINUAL_CARRIER;
                }
              }
              break;
            case PILOT_PP4:
              for (int i = 0; i < 20; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp4_cp1[i] % 3264) / dx)) % 2 && (((pp4_cp1[i] % 3264) % dx) == 0)) {
                    data_carrier_map[pp4_cp1[i] % 3264] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp4_cp1[i] % 3264] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp4_cp1[i] % 3264] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 23; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp4_cp2[i] % 3264) / dx)) % 2 && (((pp4_cp2[i] % 3264) % dx) == 0)) {
                    data_carrier_map[pp4_cp2[i] % 3264] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp4_cp2[i] % 3264] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp4_cp2[i] % 3264] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 1; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp4_cp3[i] % 3264) / dx)) % 2 && (((pp4_cp3[i] % 3264) % dx) == 0)) {
                    data_carrier_map[pp4_cp3[i] % 3264] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp4_cp3[i] % 3264] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp4_cp3[i] % 3264] = CONTINUAL_CARRIER;
                }
              }
              break;
            case PILOT_PP5:
              for (int i = 0; i < 19; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp5_cp1[i] % 3264) / dx)) % 2 && (((pp5_cp1[i] % 3264) % dx) == 0)) {
                    data_carrier_map[pp5_cp1[i] % 3264] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp5_cp1[i] % 3264] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp5_cp1[i] % 3264] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 23; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp5_cp2[i] % 3264) / dx)) % 2 && (((pp5_cp2[i] % 3264) % dx) == 0)) {
                    data_carrier_map[pp5_cp2[i] % 3264] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp5_cp2[i] % 3264] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp5_cp2[i] % 3264] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 3; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp5_cp3[i] % 3264) / dx)) % 2 && (((pp5_cp3[i] % 3264) % dx) == 0)) {
                    data_carrier_map[pp5_cp3[i] % 3264] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp5_cp3[i] % 3264] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp5_cp3[i] % 3264] = CONTINUAL_CARRIER;
                }
              }
              break;
            case PILOT_PP6:
              break;
            case PILOT_PP7:
              for (int i = 0; i < 15; i++) {
                data_carrier_map[pp7_cp1[i] % 3264] = CONTINUAL_CARRIER;
              }
              for (int i = 0; i < 30; i++) {
                data_carrier_map[pp7_cp2[i] % 3264] = CONTINUAL_CARRIER;
              }
              for (int i = 0; i < 5; i++) {
                data_carrier_map[pp7_cp3[i] % 3264] = CONTINUAL_CARRIER;
              }
              break;
            case PILOT_PP8:
              break;
          }
          break;
        case FFTSIZE_8K:
        case FFTSIZE_8K_T2GI:
          switch (pilot_pattern) {
            case PILOT_PP1:
              for (int i = 0; i < 20; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp1_cp1[i] % 6528) / dx)) % 2 && (((pp1_cp1[i] % 6528) % dx) == 0)) {
                    data_carrier_map[pp1_cp1[i] % 6528] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp1_cp1[i] % 6528] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp1_cp1[i] % 6528] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 25; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp1_cp2[i] % 6528) / dx)) % 2 && (((pp1_cp2[i] % 6528) % dx) == 0)) {
                    data_carrier_map[pp1_cp2[i] % 6528] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp1_cp2[i] % 6528] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp1_cp2[i] % 6528] = CONTINUAL_CARRIER;
                }
              }
              break;
            case PILOT_PP2:
              for (int i = 0; i < 20; i++) {
                data_carrier_map[pp2_cp1[i] % 6528] = CONTINUAL_CARRIER;
              }
              for (int i = 0; i < 22; i++) {
                data_carrier_map[pp2_cp2[i] % 6528] = CONTINUAL_CARRIER;
              }
              for (int i = 0; i < 2; i++) {
                data_carrier_map[pp2_cp3[i] % 6528] = CONTINUAL_CARRIER;
              }
              for (int i = 0; i < 2; i++) {
                data_carrier_map[pp2_cp4[i] % 6528] = CONTINUAL_CARRIER;
              }
              if (carrier_mode == CARRIERS_EXTENDED) {
                for (int i = 0; i < 4; i++) {
                  data_carrier_map[pp2_8k[i]] = CONTINUAL_CARRIER;
                }
              }
              break;
            case PILOT_PP3:
              for (int i = 0; i < 22; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp3_cp1[i] % 6528) / dx)) % 2 && (((pp3_cp1[i] % 6528) % dx) == 0)) {
                    data_carrier_map[pp3_cp1[i] % 6528] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp3_cp1[i] % 6528] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp3_cp1[i] % 6528] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 20; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp3_cp2[i] % 6528) / dx)) % 2 && (((pp3_cp2[i] % 6528) % dx) == 0)) {
                    data_carrier_map[pp3_cp2[i] % 6528] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp3_cp2[i] % 6528] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp3_cp2[i] % 6528] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 1; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp3_cp3[i] % 6528) / dx)) % 2 && (((pp3_cp3[i] % 6528) % dx) == 0)) {
                    data_carrier_map[pp3_cp3[i] % 6528] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp3_cp3[i] % 6528] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp3_cp3[i] % 6528] = CONTINUAL_CARRIER;
                }
              }
              if (carrier_mode == CARRIERS_EXTENDED) {
                for (int i = 0; i < 2; i++) {
                  if (miso == TRUE && miso_group == MISO_TX2) {
                    if (((pp3_8k[i] / dx)) % 2 && ((pp3_8k[i] % dx) == 0)) {
                      data_carrier_map[pp3_8k[i]] = CONTINUAL_CARRIER_INVERTED;
                    }
                    else {
                      data_carrier_map[pp3_8k[i]] = CONTINUAL_CARRIER;
                    }
                  }
                  else {
                    data_carrier_map[pp3_8k[i]] = CONTINUAL_CARRIER;
                  }
                }
              }
              break;
            case PILOT_PP4:
              for (int i = 0; i < 20; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp4_cp1[i] % 6528) / dx)) % 2 && (((pp4_cp1[i] % 6528) % dx) == 0)) {
                    data_carrier_map[pp4_cp1[i] % 6528] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp4_cp1[i] % 6528] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp4_cp1[i] % 6528] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 23; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp4_cp2[i] % 6528) / dx)) % 2 && (((pp4_cp2[i] % 6528) % dx) == 0)) {
                    data_carrier_map[pp4_cp2[i] % 6528] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp4_cp2[i] % 6528] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp4_cp2[i] % 6528] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 1; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp4_cp3[i] % 6528) / dx)) % 2 && (((pp4_cp3[i] % 6528) % dx) == 0)) {
                    data_carrier_map[pp4_cp3[i] % 6528] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp4_cp3[i] % 6528] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp4_cp3[i] % 6528] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 2; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp4_cp4[i] % 6528) / dx)) % 2 && (((pp4_cp4[i] % 6528) % dx) == 0)) {
                    data_carrier_map[pp4_cp4[i] % 6528] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp4_cp4[i] % 6528] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp4_cp4[i] % 6528] = CONTINUAL_CARRIER;
                }
              }
              if (carrier_mode == CARRIERS_EXTENDED) {
                for (int i = 0; i < 2; i++) {
                  if (miso == TRUE && miso_group == MISO_TX2) {
                    if (((pp4_8k[i] / dx)) % 2 && ((pp4_8k[i] % dx) == 0)) {
                      data_carrier_map[pp4_8k[i]] = CONTINUAL_CARRIER_INVERTED;
                    }
                    else {
                      data_carrier_map[pp4_8k[i]] = CONTINUAL_CARRIER;
                    }
                  }
                  else {
                    data_carrier_map[pp4_8k[i]] = CONTINUAL_CARRIER;
                  }
                }
              }
              break;
            case PILOT_PP5:
              for (int i = 0; i < 19; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp5_cp1[i] % 6528) / dx)) % 2 && (((pp5_cp1[i] % 6528) % dx) == 0)) {
                    data_carrier_map[pp5_cp1[i] % 6528] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp5_cp1[i] % 6528] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp5_cp1[i] % 6528] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 23; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp5_cp2[i] % 6528) / dx)) % 2 && (((pp5_cp2[i] % 6528) % dx) == 0)) {
                    data_carrier_map[pp5_cp2[i] % 6528] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp5_cp2[i] % 6528] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp5_cp2[i] % 6528] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 3; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp5_cp3[i] % 6528) / dx)) % 2 && (((pp5_cp3[i] % 6528) % dx) == 0)) {
                    data_carrier_map[pp5_cp3[i] % 6528] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp5_cp3[i] % 6528] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp5_cp3[i] % 6528] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 1; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp5_cp4[i] % 6528) / dx)) % 2 && (((pp5_cp4[i] % 6528) % dx) == 0)) {
                    data_carrier_map[pp5_cp4[i] % 6528] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp5_cp4[i] % 6528] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp5_cp4[i] % 6528] = CONTINUAL_CARRIER;
                }
              }
              break;
            case PILOT_PP6:
              break;
            case PILOT_PP7:
              for (int i = 0; i < 15; i++) {
                data_carrier_map[pp7_cp1[i] % 6528] = CONTINUAL_CARRIER;
              }
              for (int i = 0; i < 30; i++) {
                data_carrier_map[pp7_cp2[i] % 6528] = CONTINUAL_CARRIER;
              }
              for (int i = 0; i < 5; i++) {
                data_carrier_map[pp7_cp3[i] % 6528] = CONTINUAL_CARRIER;
              }
              for (int i = 0; i < 3; i++) {
                data_carrier_map[pp7_cp4[i] % 6528] = CONTINUAL_CARRIER;
              }
              if (carrier_mode == CARRIERS_EXTENDED) {
                for (int i = 0; i < 5; i++) {
                  data_carrier_map[pp7_8k[i]] = CONTINUAL_CARRIER;
                }
              }
              break;
            case PILOT_PP8:
              for (int i = 0; i < 47; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp8_cp4[i] % 6528) / dx)) % 2 && (((pp8_cp4[i] % 6528) % dx) == 0)) {
                    data_carrier_map[pp8_cp4[i] % 6528] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp8_cp4[i] % 6528] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp8_cp4[i] % 6528] = CONTINUAL_CARRIER;
                }
              }
              if (carrier_mode == CARRIERS_EXTENDED) {
                for (int i = 0; i < 5; i++) {
                  if (miso == TRUE && miso_group == MISO_TX2) {
                    if (((pp8_8k[i] / dx)) % 2 && ((pp8_8k[i] % dx) == 0)) {
                      data_carrier_map[pp8_8k[i]] = CONTINUAL_CARRIER_INVERTED;
                    }
                    else {
                      data_carrier_map[pp8_8k[i]] = CONTINUAL_CARRIER;
                    }
                  }
                  else {
                    data_carrier_map[pp8_8k[i]] = CONTINUAL_CARRIER;
                  }
                }
              }
              break;
          }
          break;
        case FFTSIZE_16K:
        case FFTSIZE_16K_T2GI:
          switch (pilot_pattern) {
            case PILOT_PP1:
              for (int i = 0; i < 20; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp1_cp1[i] % 13056) / dx)) % 2 && (((pp1_cp1[i] % 13056) % dx) == 0)) {
                    data_carrier_map[pp1_cp1[i] % 13056] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp1_cp1[i] % 13056] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp1_cp1[i] % 13056] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 25; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp1_cp2[i] % 13056) / dx)) % 2 && (((pp1_cp2[i] % 13056) % dx) == 0)) {
                    data_carrier_map[pp1_cp2[i] % 13056] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp1_cp2[i] % 13056] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp1_cp2[i] % 13056] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 44; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp1_cp5[i] % 13056) / dx)) % 2 && (((pp1_cp5[i] % 13056) % dx) == 0)) {
                    data_carrier_map[pp1_cp5[i] % 13056] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp1_cp5[i] % 13056] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp1_cp5[i] % 13056] = CONTINUAL_CARRIER;
                }
              }
              if (carrier_mode == CARRIERS_EXTENDED) {
                for (int i = 0; i < 4; i++) {
                  if (miso == TRUE && miso_group == MISO_TX2) {
                    if (((pp1_16k[i] / dx)) % 2 && ((pp1_16k[i] % dx) == 0)) {
                      data_carrier_map[pp1_16k[i]] = CONTINUAL_CARRIER_INVERTED;
                    }
                    else {
                      data_carrier_map[pp1_16k[i]] = CONTINUAL_CARRIER;
                    }
                  }
                  else {
                    data_carrier_map[pp1_16k[i]] = CONTINUAL_CARRIER;
                  }
                }
              }
              break;
            case PILOT_PP2:
              for (int i = 0; i < 20; i++) {
                data_carrier_map[pp2_cp1[i] % 13056] = CONTINUAL_CARRIER;
              }
              for (int i = 0; i < 22; i++) {
                data_carrier_map[pp2_cp2[i] % 13056] = CONTINUAL_CARRIER;
              }
              for (int i = 0; i < 2; i++) {
                data_carrier_map[pp2_cp3[i] % 13056] = CONTINUAL_CARRIER;
              }
              for (int i = 0; i < 2; i++) {
                data_carrier_map[pp2_cp4[i] % 13056] = CONTINUAL_CARRIER;
              }
              for (int i = 0; i < 41; i++) {
                data_carrier_map[pp2_cp5[i] % 13056] = CONTINUAL_CARRIER;
              }
              if (carrier_mode == CARRIERS_EXTENDED) {
                for (int i = 0; i < 2; i++) {
                  data_carrier_map[pp2_16k[i]] = CONTINUAL_CARRIER;
                }
              }
              break;
            case PILOT_PP3:
              for (int i = 0; i < 22; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp3_cp1[i] % 13056) / dx)) % 2 && (((pp3_cp1[i] % 13056) % dx) == 0)) {
                    data_carrier_map[pp3_cp1[i] % 13056] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp3_cp1[i] % 13056] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp3_cp1[i] % 13056] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 20; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp3_cp2[i] % 13056) / dx)) % 2 && (((pp3_cp2[i] % 13056) % dx) == 0)) {
                    data_carrier_map[pp3_cp2[i] % 13056] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp3_cp2[i] % 13056] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp3_cp2[i] % 13056] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 1; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp3_cp3[i] % 13056) / dx)) % 2 && (((pp3_cp3[i] % 13056) % dx) == 0)) {
                    data_carrier_map[pp3_cp3[i] % 13056] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp3_cp3[i] % 13056] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp3_cp3[i] % 13056] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 44; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp3_cp5[i] % 13056) / dx)) % 2 && (((pp3_cp5[i] % 13056) % dx) == 0)) {
                    data_carrier_map[pp3_cp5[i] % 13056] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp3_cp5[i] % 13056] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp3_cp5[i] % 13056] = CONTINUAL_CARRIER;
                }
              }
              if (carrier_mode == CARRIERS_EXTENDED) {
                for (int i = 0; i < 2; i++) {
                  if (miso == TRUE && miso_group == MISO_TX2) {
                    if (((pp3_16k[i] / dx)) % 2 && ((pp3_16k[i] % dx) == 0)) {
                      data_carrier_map[pp3_16k[i]] = CONTINUAL_CARRIER_INVERTED;
                    }
                    else {
                      data_carrier_map[pp3_16k[i]] = CONTINUAL_CARRIER;
                    }
                  }
                  else {
                    data_carrier_map[pp3_16k[i]] = CONTINUAL_CARRIER;
                  }
                }
              }
              break;
            case PILOT_PP4:
              for (int i = 0; i < 20; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp4_cp1[i] % 13056) / dx)) % 2 && (((pp4_cp1[i] % 13056) % dx) == 0)) {
                    data_carrier_map[pp4_cp1[i] % 13056] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp4_cp1[i] % 13056] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp4_cp1[i] % 13056] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 23; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp4_cp2[i] % 13056) / dx)) % 2 && (((pp4_cp2[i] % 13056) % dx) == 0)) {
                    data_carrier_map[pp4_cp2[i] % 13056] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp4_cp2[i] % 13056] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp4_cp2[i] % 13056] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 1; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp4_cp3[i] % 13056) / dx)) % 2 && (((pp4_cp3[i] % 13056) % dx) == 0)) {
                    data_carrier_map[pp4_cp3[i] % 13056] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp4_cp3[i] % 13056] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp4_cp3[i] % 13056] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 2; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp4_cp4[i] % 13056) / dx)) % 2 && (((pp4_cp4[i] % 13056) % dx) == 0)) {
                    data_carrier_map[pp4_cp4[i] % 13056] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp4_cp4[i] % 13056] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp4_cp4[i] % 13056] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 44; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp4_cp5[i] % 13056) / dx)) % 2 && (((pp4_cp5[i] % 13056) % dx) == 0)) {
                    data_carrier_map[pp4_cp5[i] % 13056] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp4_cp5[i] % 13056] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp4_cp5[i] % 13056] = CONTINUAL_CARRIER;
                }
              }
              if (carrier_mode == CARRIERS_EXTENDED) {
                for (int i = 0; i < 2; i++) {
                  if (miso == TRUE && miso_group == MISO_TX2) {
                    if (((pp4_16k[i] / dx)) % 2 && ((pp4_16k[i] % dx) == 0)) {
                      data_carrier_map[pp4_16k[i]] = CONTINUAL_CARRIER_INVERTED;
                    }
                    else {
                      data_carrier_map[pp4_16k[i]] = CONTINUAL_CARRIER;
                    }
                  }
                  else {
                    data_carrier_map[pp4_16k[i]] = CONTINUAL_CARRIER;
                  }
                }
              }
              break;
            case PILOT_PP5:
              for (int i = 0; i < 19; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp5_cp1[i] % 13056) / dx)) % 2 && (((pp5_cp1[i] % 13056) % dx) == 0)) {
                    data_carrier_map[pp5_cp1[i] % 13056] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp5_cp1[i] % 13056] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp5_cp1[i] % 13056] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 23; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp5_cp2[i] % 13056) / dx)) % 2 && (((pp5_cp2[i] % 13056) % dx) == 0)) {
                    data_carrier_map[pp5_cp2[i] % 13056] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp5_cp2[i] % 13056] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp5_cp2[i] % 13056] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 3; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp5_cp3[i] % 13056) / dx)) % 2 && (((pp5_cp3[i] % 13056) % dx) == 0)) {
                    data_carrier_map[pp5_cp3[i] % 13056] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp5_cp3[i] % 13056] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp5_cp3[i] % 13056] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 1; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp5_cp4[i] % 13056) / dx)) % 2 && (((pp5_cp4[i] % 13056) % dx) == 0)) {
                    data_carrier_map[pp5_cp4[i] % 13056] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp5_cp4[i] % 13056] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp5_cp4[i] % 13056] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 44; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp5_cp5[i] % 13056) / dx)) % 2 && (((pp5_cp5[i] % 13056) % dx) == 0)) {
                    data_carrier_map[pp5_cp5[i] % 13056] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp5_cp5[i] % 13056] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp5_cp5[i] % 13056] = CONTINUAL_CARRIER;
                }
              }
              if (carrier_mode == CARRIERS_EXTENDED) {
                for (int i = 0; i < 2; i++) {
                  if (miso == TRUE && miso_group == MISO_TX2) {
                    if (((pp5_16k[i] / dx)) % 2 && ((pp5_16k[i] % dx) == 0)) {
                      data_carrier_map[pp5_16k[i]] = CONTINUAL_CARRIER_INVERTED;
                    }
                    else {
                      data_carrier_map[pp5_16k[i]] = CONTINUAL_CARRIER;
                    }
                  }
                  else {
                    data_carrier_map[pp5_16k[i]] = CONTINUAL_CARRIER;
                  }
                }
              }
              break;
            case PILOT_PP6:
              for (int i = 0; i < 88; i++) {
                data_carrier_map[pp6_cp5[i] % 13056] = CONTINUAL_CARRIER;
              }
              if (carrier_mode == CARRIERS_EXTENDED) {
                for (int i = 0; i < 2; i++) {
                  data_carrier_map[pp6_16k[i]] = CONTINUAL_CARRIER;
                }
              }
              break;
            case PILOT_PP7:
              for (int i = 0; i < 15; i++) {
                data_carrier_map[pp7_cp1[i] % 13056] = CONTINUAL_CARRIER;
              }
              for (int i = 0; i < 30; i++) {
                data_carrier_map[pp7_cp2[i] % 13056] = CONTINUAL_CARRIER;
              }
              for (int i = 0; i < 5; i++) {
                data_carrier_map[pp7_cp3[i] % 13056] = CONTINUAL_CARRIER;
              }
              for (int i = 0; i < 3; i++) {
                data_carrier_map[pp7_cp4[i] % 13056] = CONTINUAL_CARRIER;
              }
              for (int i = 0; i < 35; i++) {
                data_carrier_map[pp7_cp5[i] % 13056] = CONTINUAL_CARRIER;
              }
              if (carrier_mode == CARRIERS_EXTENDED) {
                for (int i = 0; i < 3; i++) {
                  data_carrier_map[pp7_16k[i]] = CONTINUAL_CARRIER;
                }
              }
              break;
            case PILOT_PP8:
              for (int i = 0; i < 47; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp8_cp4[i] % 13056) / dx)) % 2 && (((pp8_cp4[i] % 13056) % dx) == 0)) {
                    data_carrier_map[pp8_cp4[i] % 13056] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp8_cp4[i] % 13056] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp8_cp4[i] % 13056] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 39; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if ((((pp8_cp5[i] % 13056) / dx)) % 2 && (((pp8_cp5[i] % 13056) % dx) == 0)) {
                    data_carrier_map[pp8_cp5[i] % 13056] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp8_cp5[i] % 13056] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp8_cp5[i] % 13056] = CONTINUAL_CARRIER;
                }
              }
              if (carrier_mode == CARRIERS_EXTENDED) {
                for (int i = 0; i < 3; i++) {
                  if (miso == TRUE && miso_group == MISO_TX2) {
                    if (((pp8_16k[i] / dx)) % 2 && ((pp8_16k[i] % dx) == 0)) {
                      data_carrier_map[pp8_16k[i]] = CONTINUAL_CARRIER_INVERTED;
                    }
                    else {
                      data_carrier_map[pp8_16k[i]] = CONTINUAL_CARRIER;
                    }
                  }
                  else {
                    data_carrier_map[pp8_16k[i]] = CONTINUAL_CARRIER;
                  }
                }
              }
              break;
          }
          break;
        case FFTSIZE_32K:
        case FFTSIZE_32K_T2GI:
          switch (pilot_pattern) {
            case PILOT_PP1:
              for (int i = 0; i < 20; i++) {
                data_carrier_map[pp1_cp1[i]] = CONTINUAL_CARRIER;
              }
              for (int i = 0; i < 25; i++) {
                data_carrier_map[pp1_cp2[i]] = CONTINUAL_CARRIER;
              }
              for (int i = 0; i < 44; i++) {
                data_carrier_map[pp1_cp5[i]] = CONTINUAL_CARRIER;
              }
              break;
            case PILOT_PP2:
              for (int i = 0; i < 20; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if (((pp2_cp1[i] / dx)) % 2 && ((pp2_cp1[i] % dx) == 0)) {
                    data_carrier_map[pp2_cp1[i]] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp2_cp1[i]] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp2_cp1[i]] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 22; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if (((pp2_cp2[i] / dx)) % 2 && ((pp2_cp2[i] % dx) == 0)) {
                    data_carrier_map[pp2_cp2[i]] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp2_cp2[i]] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp2_cp2[i]] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 2; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if (((pp2_cp3[i] / dx)) % 2 && ((pp2_cp3[i] % dx) == 0)) {
                    data_carrier_map[pp2_cp3[i]] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp2_cp3[i]] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp2_cp3[i]] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 2; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if (((pp2_cp4[i] / dx)) % 2 && ((pp2_cp4[i] % dx) == 0)) {
                    data_carrier_map[pp2_cp4[i]] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp2_cp4[i]] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp2_cp4[i]] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 41; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if (((pp2_cp5[i] / dx)) % 2 && ((pp2_cp5[i] % dx) == 0)) {
                    data_carrier_map[pp2_cp5[i]] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp2_cp5[i]] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp2_cp5[i]] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 88; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if (((pp2_cp6[i] / dx)) % 2 && ((pp2_cp6[i] % dx) == 0)) {
                    data_carrier_map[pp2_cp6[i]] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp2_cp6[i]] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp2_cp6[i]] = CONTINUAL_CARRIER;
                }
              }
              if (carrier_mode == CARRIERS_EXTENDED) {
                for (int i = 0; i < 2; i++) {
                  if (miso == TRUE && miso_group == MISO_TX2) {
                    if (((pp2_32k[i] / dx)) % 2 && ((pp2_32k[i] % dx) == 0)) {
                      data_carrier_map[pp2_32k[i]] = CONTINUAL_CARRIER_INVERTED;
                    }
                    else {
                      data_carrier_map[pp2_32k[i]] = CONTINUAL_CARRIER;
                    }
                  }
                  else {
                    data_carrier_map[pp2_32k[i]] = CONTINUAL_CARRIER;
                  }
                }
              }
              break;
            case PILOT_PP3:
              for (int i = 0; i < 22; i++) {
                data_carrier_map[pp3_cp1[i]] = CONTINUAL_CARRIER;
              }
              for (int i = 0; i < 20; i++) {
                data_carrier_map[pp3_cp2[i]] = CONTINUAL_CARRIER;
              }
              for (int i = 0; i < 1; i++) {
                data_carrier_map[pp3_cp3[i]] = CONTINUAL_CARRIER;
              }
              for (int i = 0; i < 44; i++) {
                data_carrier_map[pp3_cp5[i]] = CONTINUAL_CARRIER;
              }
              for (int i = 0; i < 49; i++) {
                data_carrier_map[pp3_cp6[i]] = CONTINUAL_CARRIER;
              }
              break;
            case PILOT_PP4:
              for (int i = 0; i < 20; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if (((pp4_cp1[i] / dx)) % 2 && ((pp4_cp1[i] % dx) == 0)) {
                    data_carrier_map[pp4_cp1[i]] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp4_cp1[i]] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp4_cp1[i]] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 23; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if (((pp4_cp2[i] / dx)) % 2 && ((pp4_cp2[i] % dx) == 0)) {
                    data_carrier_map[pp4_cp2[i]] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp4_cp2[i]] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp4_cp2[i]] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 1; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if (((pp4_cp3[i] / dx)) % 2 && ((pp4_cp3[i] % dx) == 0)) {
                    data_carrier_map[pp4_cp3[i]] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp4_cp3[i]] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp4_cp3[i]] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 2; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if (((pp4_cp4[i] / dx)) % 2 && ((pp4_cp4[i] % dx) == 0)) {
                    data_carrier_map[pp4_cp4[i]] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp4_cp4[i]] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp4_cp4[i]] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 44; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if (((pp4_cp5[i] / dx)) % 2 && ((pp4_cp5[i] % dx) == 0)) {
                    data_carrier_map[pp4_cp5[i]] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp4_cp5[i]] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp4_cp5[i]] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 86; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if (((pp4_cp6[i] / dx)) % 2 && ((pp4_cp6[i] % dx) == 0)) {
                    data_carrier_map[pp4_cp6[i]] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp4_cp6[i]] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp4_cp6[i]] = CONTINUAL_CARRIER;
                }
              }
              if (carrier_mode == CARRIERS_EXTENDED) {
                for (int i = 0; i < 2; i++) {
                  if (miso == TRUE && miso_group == MISO_TX2) {
                    if (((pp4_32k[i] / dx)) % 2 && ((pp4_32k[i] % dx) == 0)) {
                      data_carrier_map[pp4_32k[i]] = CONTINUAL_CARRIER_INVERTED;
                    }
                    else {
                      data_carrier_map[pp4_32k[i]] = CONTINUAL_CARRIER;
                    }
                  }
                  else {
                    data_carrier_map[pp4_32k[i]] = CONTINUAL_CARRIER;
                  }
                }
              }
              break;
            case PILOT_PP5:
              for (int i = 0; i < 19; i++) {
                data_carrier_map[pp5_cp1[i]] = CONTINUAL_CARRIER;
              }
              for (int i = 0; i < 23; i++) {
                data_carrier_map[pp5_cp2[i]] = CONTINUAL_CARRIER;
              }
              for (int i = 0; i < 3; i++) {
                data_carrier_map[pp5_cp3[i]] = CONTINUAL_CARRIER;
              }
              for (int i = 0; i < 1; i++) {
                data_carrier_map[pp5_cp4[i]] = CONTINUAL_CARRIER;
              }
              for (int i = 0; i < 44; i++) {
                data_carrier_map[pp5_cp5[i]] = CONTINUAL_CARRIER;
              }
              break;
            case PILOT_PP6:
              for (int i = 0; i < 88; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if (((pp6_cp5[i] / dx)) % 2 && ((pp6_cp5[i] % dx) == 0)) {
                    data_carrier_map[pp6_cp5[i]] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp6_cp5[i]] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp6_cp5[i]] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 88; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if (((pp6_cp6[i] / dx)) % 2 && ((pp6_cp6[i] % dx) == 0)) {
                    data_carrier_map[pp6_cp6[i]] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp6_cp6[i]] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp6_cp6[i]] = CONTINUAL_CARRIER;
                }
              }
              if (carrier_mode == CARRIERS_EXTENDED) {
                for (int i = 0; i < 4; i++) {
                  if (miso == TRUE && miso_group == MISO_TX2) {
                    if (((pp6_32k[i] / dx)) % 2 && ((pp6_32k[i] % dx) == 0)) {
                      data_carrier_map[pp6_32k[i]] = CONTINUAL_CARRIER_INVERTED;
                    }
                    else {
                      data_carrier_map[pp6_32k[i]] = CONTINUAL_CARRIER;
                    }
                  }
                  else {
                    data_carrier_map[pp6_32k[i]] = CONTINUAL_CARRIER;
                  }
                }
              }
              break;
            case PILOT_PP7:
              for (int i = 0; i < 15; i++) {
                data_carrier_map[pp7_cp1[i]] = CONTINUAL_CARRIER;
              }
              for (int i = 0; i < 30; i++) {
                data_carrier_map[pp7_cp2[i]] = CONTINUAL_CARRIER;
              }
              for (int i = 0; i < 5; i++) {
                data_carrier_map[pp7_cp3[i]] = CONTINUAL_CARRIER;
              }
              for (int i = 0; i < 3; i++) {
                data_carrier_map[pp7_cp4[i]] = CONTINUAL_CARRIER;
              }
              for (int i = 0; i < 35; i++) {
                data_carrier_map[pp7_cp5[i]] = CONTINUAL_CARRIER;
              }
              for (int i = 0; i < 92; i++) {
                data_carrier_map[pp7_cp6[i]] = CONTINUAL_CARRIER;
              }
              if (carrier_mode == CARRIERS_EXTENDED) {
                for (int i = 0; i < 2; i++) {
                  data_carrier_map[pp7_32k[i]] = CONTINUAL_CARRIER;
                }
              }
              break;
            case PILOT_PP8:
              for (int i = 0; i < 47; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if (((pp8_cp4[i] / dx)) % 2 && ((pp8_cp4[i] % dx) == 0)) {
                    data_carrier_map[pp8_cp4[i]] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp8_cp4[i]] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp8_cp4[i]] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 39; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if (((pp8_cp5[i] / dx)) % 2 && ((pp8_cp5[i] % dx) == 0)) {
                    data_carrier_map[pp8_cp5[i]] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp8_cp5[i]] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp8_cp5[i]] = CONTINUAL_CARRIER;
                }
              }
              for (int i = 0; i < 89; i++) {
                if (miso == TRUE && miso_group == MISO_TX2) {
                  if (((pp8_cp6[i] / dx)) % 2 && ((pp8_cp6[i] % dx) == 0)) {
                    data_carrier_map[pp8_cp6[i]] = CONTINUAL_CARRIER_INVERTED;
                  }
                  else {
                    data_carrier_map[pp8_cp6[i]] = CONTINUAL_CARRIER;
                  }
                }
                else {
                  data_carrier_map[pp8_cp6[i]] = CONTINUAL_CARRIER;
                }
              }
              if (carrier_mode == CARRIERS_EXTENDED) {
                for (int i = 0; i < 6; i++) {
                  if (miso == TRUE && miso_group == MISO_TX2) {
                    if (((pp8_32k[i] / dx)) % 2 && ((pp8_32k[i] % dx) == 0)) {
                      data_carrier_map[pp8_32k[i]] = CONTINUAL_CARRIER_INVERTED;
                    }
                    else {
                      data_carrier_map[pp8_32k[i]] = CONTINUAL_CARRIER;
                    }
                  }
                  else {
                    data_carrier_map[pp8_32k[i]] = CONTINUAL_CARRIER;
                  }
                }
              }
              break;
          }
          break;
      }
      for (int i = 0; i < C_PS; i++) {
        remainder = (i - K_EXT) % (dx * dy);
        if (remainder < 0) {
          remainder += (dx * dy);
        }
        if (remainder == (dx * (symbol % dy))) {
          if (miso == TRUE && miso_group == MISO_TX2) {
            if ((i / dx) % 2) {
              data_carrier_map[i] = SCATTERED_CARRIER_INVERTED;
            }
            else {
              data_carrier_map[i] = SCATTERED_CARRIER;
            }
          }
          else {
            data_carrier_map[i] = SCATTERED_CARRIER;
          }
        }
      }
      if (miso == TRUE && miso_group == MISO_TX2) {
        if (symbol % 2) {
          data_carrier_map[0] = SCATTERED_CARRIER_INVERTED;
          data_carrier_map[C_PS - 1] = SCATTERED_CARRIER_INVERTED;
        }
        else {
          data_carrier_map[0] = SCATTERED_CARRIER;
          data_carrier_map[C_PS - 1] = SCATTERED_CARRIER;
        }
      }
      else {
        data_carrier_map[0] = SCATTERED_CARRIER;
        data_carrier_map[C_PS - 1] = SCATTERED_CARRIER;
      }
      if (papr_mode == PAPR_TR || papr_mode == PAPR_BOTH) {
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
    }

    int
    dvbt2_pilotgenerator_cc_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];
      gr_complex zero;
      gr_complex *dst;
      int L_FC = 0;

      zero = gr_complex(0.0, 0.0);
      if (N_FC != 0) {
        L_FC = 1;
      }
      for (int i = 0; i < noutput_items; i += num_symbols) {
        for (int j = 0; j < num_symbols; j++) {
          init_pilots(j);
          if (j < N_P2) {
            for (int n = 0; n < left_nulls; n++) {
              *out++ = zero;
            }
            for (int n = 0; n < C_PS; n++) {
              if (p2_carrier_map[n] == P2PILOT_CARRIER) {
                *out++ = p2_bpsk[prbs[n + K_OFFSET] ^ pn_sequence[j]];
              }
              else if (p2_carrier_map[n] == P2PILOT_CARRIER_INVERTED) {
                *out++ = p2_bpsk_inverted[prbs[n + K_OFFSET] ^ pn_sequence[j]];
              }
              else if (p2_carrier_map[n] == P2PAPR_CARRIER) {
                *out++ = zero;
              }
              else {
                *out++ = *in++;
              }
            }
            for (int n = 0; n < right_nulls; n++) {
              *out++ = zero;
            }
          }
          else if (j == (num_symbols - L_FC)) {
            for (int n = 0; n < left_nulls; n++) {
              *out++ = zero;
            }
            for (int n = 0; n < C_PS; n++) {
              if (fc_carrier_map[n] == SCATTERED_CARRIER) {
                *out++ = sp_bpsk[prbs[n + K_OFFSET] ^ pn_sequence[j]];
              }
              else if (fc_carrier_map[n] == SCATTERED_CARRIER_INVERTED) {
                *out++ = sp_bpsk_inverted[prbs[n + K_OFFSET] ^ pn_sequence[j]];
              }
              else if (fc_carrier_map[n] == TRPAPR_CARRIER) {
                *out++ = zero;
              }
              else {
                *out++ = *in++;
              }
            }
            for (int n = 0; n < right_nulls; n++) {
              *out++ = zero;
            }
          }
          else {
            for (int n = 0; n < left_nulls; n++) {
              *out++ = zero;
            }
            for (int n = 0; n < C_PS; n++) {
              if (data_carrier_map[n] == SCATTERED_CARRIER) {
                *out++ = sp_bpsk[prbs[n + K_OFFSET] ^ pn_sequence[j]];
              }
              else if (data_carrier_map[n] == SCATTERED_CARRIER_INVERTED) {
                *out++ = sp_bpsk_inverted[prbs[n + K_OFFSET] ^ pn_sequence[j]];
              }
              else if (data_carrier_map[n] == CONTINUAL_CARRIER) {
                *out++ = cp_bpsk[prbs[n + K_OFFSET] ^ pn_sequence[j]];
              }
              else if (data_carrier_map[n] == CONTINUAL_CARRIER_INVERTED) {
                *out++ = cp_bpsk_inverted[prbs[n + K_OFFSET] ^ pn_sequence[j]];
              }
              else if (data_carrier_map[n] == TRPAPR_CARRIER) {
                *out++ = zero;
              }
              else {
                *out++ = *in++;
              }
            }
            for (int n = 0; n < right_nulls; n++) {
              *out++ = zero;
            }
          }
          out -= ofdm_fft_size;
          if (equalization_enable == EQUALIZATION_ON) {
            volk_32fc_x2_multiply_32fc(out, out, inverse_sinc, ofdm_fft_size);
          }
          dst = ofdm_fft->get_inbuf();
          memcpy(&dst[ofdm_fft_size / 2], &out[0], sizeof(gr_complex) * ofdm_fft_size / 2);
          memcpy(&dst[0], &out[ofdm_fft_size / 2], sizeof(gr_complex) * ofdm_fft_size / 2);
          ofdm_fft->execute();
          volk_32fc_s32fc_multiply_32fc(out, ofdm_fft->get_outbuf(), normalization, ofdm_fft_size);
          out += ofdm_fft_size;
        }
      }

      // Tell runtime system how many input items we consumed on
      // each input stream.
      consume_each (active_items);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

    const unsigned char dvbt2_pilotgenerator_cc_impl::pn_sequence_table[CHIPS / 8] = 
    {
      0x4D, 0xC2, 0xAF, 0x7B, 0xD8, 0xC3, 0xC9, 0xA1, 0xE7, 0x6C, 0x9A, 0x09, 0x0A, 0xF1, 0xC3, 0x11,
      0x4F, 0x07, 0xFC, 0xA2, 0x80, 0x8E, 0x94, 0x62, 0xE9, 0xAD, 0x7B, 0x71, 0x2D, 0x6F, 0x4A, 0xC8,
      0xA5, 0x9B, 0xB0, 0x69, 0xCC, 0x50, 0xBF, 0x11, 0x49, 0x92, 0x7E, 0x6B, 0xB1, 0xC9, 0xFC, 0x8C,
      0x18, 0xBB, 0x94, 0x9B, 0x30, 0xCD, 0x09, 0xDD, 0xD7, 0x49, 0xE7, 0x04, 0xF5, 0x7B, 0x41, 0xDE,
      0xC7, 0xE7, 0xB1, 0x76, 0xE1, 0x2C, 0x56, 0x57, 0x43, 0x2B, 0x51, 0xB0, 0xB8, 0x12, 0xDF, 0x0E,
      0x14, 0x88, 0x7E, 0x24, 0xD8, 0x0C, 0x97, 0xF0, 0x93, 0x74, 0xAD, 0x76, 0x27, 0x0E, 0x58, 0xFE,
      0x17, 0x74, 0xB2, 0x78, 0x1D, 0x8D, 0x38, 0x21, 0xE3, 0x93, 0xF2, 0xEA, 0x0F, 0xFD, 0x4D, 0x24,
      0xDE, 0x20, 0xC0, 0x5D, 0x0B, 0xA1, 0x70, 0x3D, 0x10, 0xE5, 0x2D, 0x61, 0xE0, 0x13, 0xD8, 0x37,
      0xAA, 0x62, 0xD0, 0x07, 0xCC, 0x2F, 0xD7, 0x6D, 0x23, 0xA3, 0xE1, 0x25, 0xBD, 0xE8, 0xA9, 0xA7,
      0xC0, 0x2A, 0x98, 0xB7, 0x02, 0x51, 0xC5, 0x56, 0xF6, 0x34, 0x1E, 0xBD, 0xEC, 0xB8, 0x01, 0xAA,
      0xD5, 0xD9, 0xFB, 0x8C, 0xBE, 0xA8, 0x0B, 0xB6, 0x19, 0x09, 0x65, 0x27, 0xA8, 0xC4, 0x75, 0xB3,
      0xD8, 0xDB, 0x28, 0xAF, 0x85, 0x43, 0xA0, 0x0E, 0xC3, 0x48, 0x0D, 0xFF, 0x1E, 0x2C, 0xDA, 0x9F,
      0x98, 0x5B, 0x52, 0x3B, 0x87, 0x90, 0x07, 0xAA, 0x5D, 0x0C, 0xE5, 0x8D, 0x21, 0xB1, 0x86, 0x31,
      0x00, 0x66, 0x17, 0xF6, 0xF7, 0x69, 0xEB, 0x94, 0x7F, 0x92, 0x4E, 0xA5, 0x16, 0x1E, 0xC2, 0xC0,
      0x48, 0x8B, 0x63, 0xED, 0x79, 0x93, 0xBA, 0x8E, 0xF4, 0xE5, 0x52, 0xFA, 0x32, 0xFC, 0x3F, 0x1B,
      0xDB, 0x19, 0x92, 0x39, 0x02, 0xBC, 0xBB, 0xE5, 0xDD, 0xAB, 0xB8, 0x24, 0x12, 0x6E, 0x08, 0x45,
      0x9C, 0xA6, 0xCF, 0xA0, 0x26, 0x7E, 0x52, 0x94, 0xA9, 0x8C, 0x63, 0x25, 0x69, 0x79, 0x1E, 0x60,
      0xEF, 0x65, 0x9A, 0xEE, 0x95, 0x18, 0xCD, 0xF0, 0x8D, 0x87, 0x83, 0x36, 0x90, 0xC1, 0xB7, 0x91,
      0x83, 0xED, 0x12, 0x7E, 0x53, 0x36, 0x0C, 0xD8, 0x65, 0x14, 0x85, 0x9A, 0x28, 0xB5, 0x49, 0x4F,
      0x51, 0xAA, 0x48, 0x82, 0x41, 0x9A, 0x25, 0xA2, 0xD0, 0x1A, 0x5F, 0x47, 0xAA, 0x27, 0x30, 0x1E,
      0x79, 0xA5, 0x37, 0x0C, 0xCB, 0x3E, 0x19, 0x7F
    };

    const int dvbt2_pilotgenerator_cc_impl::p2_papr_map_1k[10] = 
    {
      116, 130, 134, 157, 182, 256, 346, 478, 479, 532
    };

    const int dvbt2_pilotgenerator_cc_impl::p2_papr_map_2k[18] = 
    {
      113, 124, 262, 467, 479, 727, 803, 862, 910, 946,
      980, 1201, 1322, 1342, 1396, 1397, 1562, 1565
    };

    const int dvbt2_pilotgenerator_cc_impl::p2_papr_map_4k[36] = 
    {
      104, 116, 119, 163, 170, 173, 664, 886, 1064, 1151, 1196, 1264, 1531,
      1736, 1951, 1960, 2069, 2098, 2311, 2366, 2473, 2552, 2584, 2585, 2645,
      2774, 2846, 2882, 3004, 3034, 3107, 3127, 3148, 3191, 3283, 3289
    };

    const int dvbt2_pilotgenerator_cc_impl::p2_papr_map_8k[72] = 
    {
      106, 109, 110, 112, 115, 118, 133, 142, 163, 184, 206, 247, 445, 461,
      503, 565, 602, 656, 766, 800, 922, 1094, 1108, 1199, 1258, 1726, 1793,
      1939, 2128, 2714, 3185, 3365, 3541, 3655, 3770, 3863, 4066, 4190, 4282,
      4565, 4628, 4727, 4882, 4885, 5143, 5192, 5210, 5257, 5261, 5459, 5651,
      5809, 5830, 5986, 6020, 6076, 6253, 6269, 6410, 6436, 6467, 6475, 6509,
      6556, 6611, 6674, 6685, 6689, 6691, 6695, 6698, 6701
    };

    const int dvbt2_pilotgenerator_cc_impl::p2_papr_map_16k[144] = 
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

    const int dvbt2_pilotgenerator_cc_impl::p2_papr_map_32k[288] = 
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

    const int dvbt2_pilotgenerator_cc_impl::tr_papr_map_1k[10] = 
    {
      109, 117, 122, 129, 139, 321, 350, 403, 459, 465
    };

    const int dvbt2_pilotgenerator_cc_impl::tr_papr_map_2k[18] = 
    {
      250, 404, 638, 677, 700, 712, 755, 952, 1125, 1145,
      1190, 1276, 1325, 1335, 1406, 1431, 1472, 1481
    };

    const int dvbt2_pilotgenerator_cc_impl::tr_papr_map_4k[36] = 
    {
      170, 219, 405, 501, 597, 654, 661, 745, 995, 1025, 1319, 1361, 1394,
      1623, 1658, 1913, 1961, 1971, 2106, 2117, 2222, 2228, 2246, 2254, 2361,
      2468, 2469, 2482, 2637, 2679, 2708, 2825, 2915, 2996, 3033, 3119
    };

    const int dvbt2_pilotgenerator_cc_impl::tr_papr_map_8k[72] = 
    {
      111, 115, 123, 215, 229, 392, 613, 658, 831, 842, 997, 1503, 1626, 1916,
      1924, 1961, 2233, 2246, 2302, 2331, 2778, 2822, 2913, 2927, 2963, 2994,
      3087, 3162, 3226, 3270, 3503, 3585, 3711, 3738, 3874, 3902, 4013, 4017,
      4186, 4253, 4292, 4339, 4412, 4453, 4669, 4910, 5015, 5030, 5061, 5170,
      5263, 5313, 5360, 5384, 5394, 5493, 5550, 5847, 5901, 5999, 6020, 6165,
      6174, 6227, 6245, 6314, 6316, 6327, 6503, 6507, 6545, 6565
    };

    const int dvbt2_pilotgenerator_cc_impl::tr_papr_map_16k[144] = 
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

    const int dvbt2_pilotgenerator_cc_impl::tr_papr_map_32k[288] = 
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

    const int dvbt2_pilotgenerator_cc_impl::pp1_cp1[20] = 
    {
      116, 255, 285, 430, 518, 546, 601, 646, 744, 1662, 1893, 1995, 2322, 3309, 3351,
      3567, 3813, 4032, 5568, 5706
    };

    const int dvbt2_pilotgenerator_cc_impl::pp1_cp2[25] = 
    {
      1022, 1224, 1302, 1371, 1495, 2261, 2551, 2583, 2649, 2833, 2925, 3192, 4266, 5395,
      5710, 5881, 8164, 10568, 11069, 11560, 12631, 12946, 13954, 16745, 21494
    };

    const int dvbt2_pilotgenerator_cc_impl::pp1_cp5[44] = 
    {
      1369, 7013, 7215, 7284, 7649, 7818, 8025, 8382, 8733, 8880, 9249, 9432, 9771, 10107,
      10110, 10398, 10659, 10709, 10785, 10872, 11115, 11373, 11515, 11649, 11652, 12594,
      12627, 12822, 12984, 15760, 16612, 17500, 18358, 19078, 19930, 20261, 20422, 22124,
      22867, 23239, 24934, 25879, 26308, 26674
    };

    const int dvbt2_pilotgenerator_cc_impl::pp2_cp1[20] = 
    {
      116, 318, 390, 430, 474, 518, 601, 646, 708, 726, 1752, 1758, 1944, 2100, 2208, 2466,
      3792, 5322, 5454, 5640
    };

    const int dvbt2_pilotgenerator_cc_impl::pp2_cp2[22] = 
    {
      1022, 1092, 1369, 1416, 1446, 1495, 2598, 2833, 2928, 3144, 4410, 4800, 5710, 5881,
      6018, 6126, 10568, 11515, 12946, 13954, 15559, 16681
    };

    const int dvbt2_pilotgenerator_cc_impl::pp2_cp3[2] = 
    {
      2261, 8164
    };

    const int dvbt2_pilotgenerator_cc_impl::pp2_cp4[2] = 
    {
      10709, 19930
    };

    const int dvbt2_pilotgenerator_cc_impl::pp2_cp5[41] = 
    {
      6744, 7013, 7020, 7122, 7308, 7649, 7674, 7752, 7764, 8154, 8190, 8856, 8922, 9504,
      9702, 9882, 9924, 10032, 10092, 10266, 10302, 10494, 10530, 10716, 11016, 11076,
      11160, 11286, 11436, 11586, 12582, 13002, 17500, 18358, 19078, 22124, 23239, 24073,
      24934, 25879, 26308
    };

    const int dvbt2_pilotgenerator_cc_impl::pp2_cp6[88] = 
    {
      13164, 13206, 13476, 13530, 13536, 13764, 13848, 13938, 13968, 14028, 14190, 14316,
      14526, 14556, 14562, 14658, 14910, 14946, 15048, 15186, 15252, 15468, 15540, 15576,
      15630, 15738, 15840, 16350, 16572, 16806, 17028, 17064, 17250, 17472, 17784, 17838,
      18180, 18246, 18480, 18900, 18960, 19254, 19482, 19638, 19680, 20082, 20310, 20422,
      20454, 20682, 20874, 21240, 21284, 21444, 21450, 21522, 21594, 21648, 21696, 21738,
      22416, 22824, 23016, 23124, 23196, 23238, 23316, 23418, 23922, 23940, 24090, 24168,
      24222, 24324, 24342, 24378, 24384, 24540, 24744, 24894, 24990, 25002, 25194, 25218,
      25260, 25566, 26674, 26944
    };

    const int dvbt2_pilotgenerator_cc_impl::pp3_cp1[22] = 
    {
      116, 318, 342, 426, 430, 518, 582, 601, 646, 816, 1758, 1764, 2400, 3450, 3504,
      3888, 4020, 4932, 5154, 5250, 5292, 5334
    };

    const int dvbt2_pilotgenerator_cc_impl::pp3_cp2[20] = 
    {
      1022, 1495, 2261, 2551, 2802, 2820, 2833, 2922, 4422, 4752, 4884, 5710, 8164,
      10568, 11069, 11560, 12631, 12946, 16745, 21494
    };

    const int dvbt2_pilotgenerator_cc_impl::pp3_cp3[1] = 
    {
      13954
    };

    const int dvbt2_pilotgenerator_cc_impl::pp3_cp5[44] = 
    {
      1369, 5395, 5881, 6564, 6684, 7013, 7649, 8376, 8544, 8718, 8856, 9024, 9132, 9498,
      9774, 9840, 10302, 10512, 10566, 10770, 10914, 11340, 11418, 11730, 11742, 12180,
      12276, 12474, 12486, 15760, 16612, 17500, 18358, 19078, 19930, 20261, 20422, 22124,
      22867, 23239, 24934, 25879, 26308, 26674
    };

    const int dvbt2_pilotgenerator_cc_impl::pp3_cp6[49] = 
    {
      13320, 13350, 13524, 13566, 13980, 14148, 14340, 14964, 14982, 14994, 15462, 15546,
      15984, 16152, 16314, 16344, 16488, 16614, 16650, 16854, 17028, 17130, 17160, 17178,
      17634, 17844, 17892, 17958, 18240, 18270, 18288, 18744, 18900, 18930, 18990, 19014,
      19170, 19344, 19662, 19698, 20022, 20166, 20268, 20376, 20466, 20550, 20562, 20904,
      21468
    };

    const int dvbt2_pilotgenerator_cc_impl::pp4_cp1[20] = 
    {
      108, 116, 144, 264, 288, 430, 518, 564, 636, 646, 828, 2184, 3360, 3396, 3912, 4032,
      4932, 5220, 5676, 5688
    };

    const int dvbt2_pilotgenerator_cc_impl::pp4_cp2[23] = 
    {
      601, 1022, 1092, 1164, 1369, 1392, 1452, 1495, 2261, 2580, 2833, 3072, 4320, 4452,
      5710, 5881, 6048, 10568, 11515, 12946, 13954, 15559, 16681
    };

    const int dvbt2_pilotgenerator_cc_impl::pp4_cp3[1] = 
    {
      8164
    };

    const int dvbt2_pilotgenerator_cc_impl::pp4_cp4[2] = 
    {
      10709, 19930
    };

    const int dvbt2_pilotgenerator_cc_impl::pp4_cp5[44] = 
    {
      6612, 6708, 7013, 7068, 7164, 7224, 7308, 7464, 7649, 7656, 7716, 7752, 7812, 7860,
      8568, 8808, 8880, 9072, 9228, 9516, 9696, 9996, 10560, 10608, 10728, 11148, 11232,
      11244, 11496, 11520, 11664, 11676, 11724, 11916, 17500, 18358, 19078, 21284, 22124,
      23239, 24073, 24934, 25879, 26308
    };

    const int dvbt2_pilotgenerator_cc_impl::pp4_cp6[86] = 
    {
      13080, 13152, 13260, 13380, 13428, 13572, 13884, 13956, 14004, 14016, 14088, 14232,
      14304, 14532, 14568, 14760, 14940, 15168, 15288, 15612, 15684, 15888, 16236, 16320,
      16428, 16680, 16812, 16908, 17184, 17472, 17508, 17580, 17892, 17988, 18000, 18336,
      18480, 18516, 19020, 19176, 19188, 19320, 19776, 19848, 20112, 20124, 20184, 20388,
      20532, 20556, 20676, 20772, 21156, 21240, 21276, 21336, 21384, 21816, 21888, 22068,
      22092, 22512, 22680, 22740, 22800, 22836, 22884, 23304, 23496, 23568, 23640, 24120,
      24168, 24420, 24444, 24456, 24492, 24708, 24864, 25332, 25536, 25764, 25992, 26004,
      26674, 26944
    };

    const int dvbt2_pilotgenerator_cc_impl::pp5_cp1[19] = 
    {
      108, 116, 228, 430, 518, 601, 646, 804, 1644, 1680, 1752, 1800, 1836, 3288, 3660,
      4080, 4932, 4968, 5472
    };

    const int dvbt2_pilotgenerator_cc_impl::pp5_cp2[23] = 
    {
      852, 1022, 1495, 2508, 2551, 2604, 2664, 2736, 2833, 3120, 4248, 4512, 4836, 5710,
      5940, 6108, 8164, 10568, 11069, 11560, 12946, 13954, 21494
    };

    const int dvbt2_pilotgenerator_cc_impl::pp5_cp3[3] = 
    {
      648, 4644, 16745
    };

    const int dvbt2_pilotgenerator_cc_impl::pp5_cp4[1] = 
    {
      12631
    };

    const int dvbt2_pilotgenerator_cc_impl::pp5_cp5[44] = 
    {
      1369, 2261, 5395, 5881, 6552, 6636, 6744, 6900, 7032, 7296, 7344, 7464, 7644, 7649,
      7668, 7956, 8124, 8244, 8904, 8940, 8976, 9216, 9672, 9780, 10224, 10332, 10709,
      10776, 10944, 11100, 11292, 11364, 11496, 11532, 11904, 12228, 12372, 12816, 15760,
      16612, 17500, 19078, 22867, 25879
    };

    const int dvbt2_pilotgenerator_cc_impl::pp6_cp5[88] = 
    {
      116, 384, 408, 518, 601, 646, 672, 960, 1022, 1272, 1344, 1369, 1495, 1800, 2040,
      2261, 2833, 3192, 3240, 3768, 3864, 3984, 4104, 4632, 4728, 4752, 4944, 5184, 5232,
      5256, 5376, 5592, 5616, 5710, 5808, 5881, 6360, 6792, 6960, 7013, 7272, 7344, 7392,
      7536, 7649, 7680, 7800, 8064, 8160, 8164, 8184, 8400, 8808, 8832, 9144, 9648, 9696,
      9912, 10008, 10200, 10488, 10568, 10656, 10709, 11088, 11160, 11515, 11592, 12048,
      12264, 12288, 12312, 12552, 12672, 12946, 13954, 15559, 16681, 17500, 19078, 20422,
      21284, 22124, 23239, 24934, 25879, 26308, 26674
    };

    const int dvbt2_pilotgenerator_cc_impl::pp6_cp6[88] = 
    {
      13080, 13368, 13464, 13536, 13656, 13728, 13824, 14112, 14232, 14448, 14472, 14712,
      14808, 14952, 15000, 15336, 15360, 15408, 15600, 15624, 15648, 16128, 16296, 16320,
      16416, 16536, 16632, 16824, 16848, 17184, 17208, 17280, 17352, 17520, 17664, 17736,
      17784, 18048, 18768, 18816, 18840, 19296, 19392, 19584, 19728, 19752, 19776, 20136,
      20184, 20208, 20256, 21096, 21216, 21360, 21408, 21744, 21768, 22200, 22224, 22320,
      22344, 22416, 22848, 22968, 23016, 23040, 23496, 23688, 23904, 24048, 24168, 24360,
      24408, 24984, 25152, 25176, 25224, 25272, 25344, 25416, 25488, 25512, 25536, 25656,
      25680, 25752, 25992, 26016
    };

    const int dvbt2_pilotgenerator_cc_impl::pp7_cp1[15] = 
    {
      264, 360, 1848, 2088, 2112, 2160, 2256, 2280, 3936, 3960, 3984, 5016, 5136, 5208,
      5664
    };

    const int dvbt2_pilotgenerator_cc_impl::pp7_cp2[30] = 
    {
      116, 430, 518, 601, 646, 1022, 1296, 1368, 1369, 1495, 2833, 3024, 4416, 4608,
      4776, 5710, 5881, 6168, 7013, 8164, 10568, 10709, 11515, 12946, 15559, 23239,
      24934, 25879, 26308, 26674
    };

    const int dvbt2_pilotgenerator_cc_impl::pp7_cp3[5] = 
    {
      456, 480, 2261, 6072, 17500
    };

    const int dvbt2_pilotgenerator_cc_impl::pp7_cp4[3] = 
    {
      1008, 6120, 13954
    };

    const int dvbt2_pilotgenerator_cc_impl::pp7_cp5[35] = 
    {
      6984, 7032, 7056, 7080, 7152, 7320, 7392, 7536, 7649, 7704, 7728, 7752, 8088, 8952,
      9240, 9288, 9312, 9480, 9504, 9840, 9960, 10320, 10368, 10728, 10752, 11448, 11640,
      11688, 11808, 12192, 12240, 12480, 12816, 16681, 22124
    };

    const int dvbt2_pilotgenerator_cc_impl::pp7_cp6[92] = 
    {
      13416, 13440, 13536, 13608, 13704, 13752, 14016, 14040, 14112, 14208, 14304, 14376,
      14448, 14616, 14712, 14760, 14832, 14976, 15096, 15312, 15336, 15552, 15816, 15984,
      16224, 16464, 16560, 17088, 17136, 17256, 17352, 17400, 17448, 17544, 17928, 18048,
      18336, 18456, 18576, 18864, 19032, 19078, 19104, 19320, 19344, 19416, 19488, 19920,
      19930, 19992, 20424, 20664, 20808, 21168, 21284, 21360, 21456, 21816, 22128, 22200,
      22584, 22608, 22824, 22848, 22944, 22992, 23016, 23064, 23424, 23448, 23472, 23592,
      24192, 24312, 24360, 24504, 24552, 24624, 24648, 24672, 24768, 24792, 25080, 25176,
      25224, 25320, 25344, 25584, 25680, 25824, 26064, 26944
    };

    const int dvbt2_pilotgenerator_cc_impl::pp8_cp4[47] = 
    {
      116, 132, 180, 430, 518, 601, 646, 1022, 1266, 1369, 1495, 2261, 2490, 2551, 2712,
      2833, 3372, 3438, 4086, 4098, 4368, 4572, 4614, 4746, 4830, 4968, 5395, 5710, 5881,
      7649, 8164, 10568, 11069, 11560, 12631, 12946, 13954, 15760, 16612, 16745, 17500,
      19078, 19930, 21494, 22867, 25879, 26308
    };

    const int dvbt2_pilotgenerator_cc_impl::pp8_cp5[39] = 
    {
      6720, 6954, 7013, 7026, 7092, 7512, 7536, 7596, 7746, 7758, 7818, 7986, 8160, 8628,
      9054, 9096, 9852, 9924, 10146, 10254, 10428, 10704, 11418, 11436, 11496, 11550,
      11766, 11862, 12006, 12132, 12216, 12486, 12762, 18358, 20261, 20422, 22124,
      23239, 24934
    };

    const int dvbt2_pilotgenerator_cc_impl::pp8_cp6[89] = 
    {
      10709, 11515, 13254, 13440, 13614, 13818, 14166, 14274, 14304, 14364, 14586, 14664,
      15030, 15300, 15468, 15474, 15559, 15732, 15774, 16272, 16302, 16428, 16500, 16662,
      16681, 16872, 17112, 17208, 17862, 18036, 18282, 18342, 18396, 18420, 18426, 18732,
      19050, 19296, 19434, 19602, 19668, 19686, 19728, 19938, 20034, 21042, 21120, 21168,
      21258, 21284, 21528, 21594, 21678, 21930, 21936, 21990, 22290, 22632, 22788, 23052,
      23358, 23448, 23454, 23706, 23772, 24048, 24072, 24073, 24222, 24384, 24402, 24444,
      24462, 24600, 24738, 24804, 24840, 24918, 24996, 25038, 25164, 25314, 25380, 25470,
      25974, 26076, 26674, 26753, 26944
    };

    const int dvbt2_pilotgenerator_cc_impl::pp2_8k[4] = 
    {
      6820, 6847, 6869, 6898
    };

    const int dvbt2_pilotgenerator_cc_impl::pp3_8k[2] = 
    {
      6820, 6869
    };

    const int dvbt2_pilotgenerator_cc_impl::pp4_8k[2] = 
    {
      6820, 6869
    };

    const int dvbt2_pilotgenerator_cc_impl::pp7_8k[5] = 
    {
      6820, 6833, 6869, 6887, 6898
    };

    const int dvbt2_pilotgenerator_cc_impl::pp8_8k[5] = 
    {
      6820, 6833, 6869, 6887, 6898
    };

    const int dvbt2_pilotgenerator_cc_impl::pp1_16k[4] = 
    {
      3636, 13724, 13790, 13879
    };

    const int dvbt2_pilotgenerator_cc_impl::pp2_16k[2] = 
    {
      13636, 13790
    };

    const int dvbt2_pilotgenerator_cc_impl::pp3_16k[2] = 
    {
      13636, 13790
    };

    const int dvbt2_pilotgenerator_cc_impl::pp4_16k[2] = 
    {
      13636, 13790
    };

    const int dvbt2_pilotgenerator_cc_impl::pp5_16k[2] = 
    {
      13636, 13790
    };

    const int dvbt2_pilotgenerator_cc_impl::pp6_16k[2] = 
    {
      13636, 13790
    };

    const int dvbt2_pilotgenerator_cc_impl::pp7_16k[3] = 
    {
      13636, 13724, 13879
    };

    const int dvbt2_pilotgenerator_cc_impl::pp8_16k[3] = 
    {
      13636, 13724, 13879
    };

    const int dvbt2_pilotgenerator_cc_impl::pp2_32k[2] = 
    {
      27268, 27688
    };

    const int dvbt2_pilotgenerator_cc_impl::pp4_32k[2] = 
    {
      27268, 27688
    };

    const int dvbt2_pilotgenerator_cc_impl::pp6_32k[4] = 
    {
      27268, 27448, 27688, 27758
    };

    const int dvbt2_pilotgenerator_cc_impl::pp7_32k[2] = 
    {
      27268, 27688
    };

    const int dvbt2_pilotgenerator_cc_impl::pp8_32k[6] = 
    {
      27268, 27368, 27448, 27580, 27688, 27758
    };

  } /* namespace dtv */
} /* namespace gr */

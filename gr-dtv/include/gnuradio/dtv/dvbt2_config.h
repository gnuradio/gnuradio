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

#ifndef INCLUDED_DTV_DVBT2_CONFIG_H
#define INCLUDED_DTV_DVBT2_CONFIG_H

namespace gr {
  namespace dtv {
    enum dvbt2_rotation_t {
      ROTATION_OFF = 0,
      ROTATION_ON,
    };

    enum dvbt2_streamtype_t {
      STREAMTYPE_TS = 0,
      STREAMTYPE_GS,
      STREAMTYPE_BOTH,
    };

    enum dvbt2_inputmode_t {
      INPUTMODE_NORMAL = 0,
      INPUTMODE_HIEFF,
    };

    enum dvbt2_extended_carrier_t {
      CARRIERS_NORMAL = 0,
      CARRIERS_EXTENDED,
    };

    enum dvbt2_preamble_t {
      PREAMBLE_T2_SISO = 0,
      PREAMBLE_T2_MISO,
      PREAMBLE_NON_T2,
      PREAMBLE_T2_LITE_SISO,
      PREAMBLE_T2_LITE_MISO,
    };

    enum dvbt2_fftsize_t {
      FFTSIZE_2K = 0,
      FFTSIZE_8K,
      FFTSIZE_4K,
      FFTSIZE_1K,
      FFTSIZE_16K,
      FFTSIZE_32K,
      FFTSIZE_8K_T2GI,
      FFTSIZE_32K_T2GI,
      FFTSIZE_16K_T2GI = 11,
    };

    enum dvbt2_papr_t {
      PAPR_OFF = 0,
      PAPR_ACE,
      PAPR_TR,
      PAPR_BOTH,
    };

    enum dvbt2_l1constellation_t {
      L1_MOD_BPSK = 0,
      L1_MOD_QPSK,
      L1_MOD_16QAM,
      L1_MOD_64QAM,
    };

    enum dvbt2_pilotpattern_t {
      PILOT_PP1 = 0,
      PILOT_PP2,
      PILOT_PP3,
      PILOT_PP4,
      PILOT_PP5,
      PILOT_PP6,
      PILOT_PP7,
      PILOT_PP8,
    };

    enum dvbt2_version_t {
      VERSION_111 = 0,
      VERSION_121,
      VERSION_131,
    };

    enum dvbt2_reservedbiasbits_t {
      RESERVED_OFF = 0,
      RESERVED_ON,
    };

    enum dvbt2_l1scrambled_t {
      L1_SCRAMBLED_OFF = 0,
      L1_SCRAMBLED_ON,
    };

    enum dvbt2_misogroup_t {
      MISO_TX1 = 0,
      MISO_TX2,
    };

    enum dvbt2_showlevels_t {
      SHOWLEVELS_OFF = 0,
      SHOWLEVELS_ON,
    };

    enum dvbt2_inband_t {
      INBAND_OFF = 0,
      INBAND_ON,
    };

    enum dvbt2_equalization_t {
      EQUALIZATION_OFF = 0,
      EQUALIZATION_ON,
    };

    enum dvbt2_bandwidth_t {
      BANDWIDTH_1_7_MHZ = 0,
      BANDWIDTH_5_0_MHZ,
      BANDWIDTH_6_0_MHZ,
      BANDWIDTH_7_0_MHZ,
      BANDWIDTH_8_0_MHZ,
      BANDWIDTH_10_0_MHZ,
    };

  } // namespace dtv
} // namespace gr

typedef gr::dtv::dvbt2_rotation_t dvbt2_rotation_t;
typedef gr::dtv::dvbt2_streamtype_t dvbt2_streamtype_t;
typedef gr::dtv::dvbt2_inputmode_t dvbt2_inputmode_t;
typedef gr::dtv::dvbt2_extended_carrier_t dvbt2_extended_carrier_t;
typedef gr::dtv::dvbt2_preamble_t dvbt2_preamble_t;
typedef gr::dtv::dvbt2_fftsize_t dvbt2_fftsize_t;
typedef gr::dtv::dvbt2_papr_t dvbt2_papr_t;
typedef gr::dtv::dvbt2_l1constellation_t dvbt2_l1constellation_t;
typedef gr::dtv::dvbt2_pilotpattern_t dvbt2_pilotpattern_t;
typedef gr::dtv::dvbt2_version_t dvbt2_version_t;
typedef gr::dtv::dvbt2_reservedbiasbits_t dvbt2_reservedbiasbits_t;
typedef gr::dtv::dvbt2_l1scrambled_t dvbt2_l1scrambled_t;
typedef gr::dtv::dvbt2_misogroup_t dvbt2_misogroup_t;
typedef gr::dtv::dvbt2_showlevels_t dvbt2_showlevels_t;
typedef gr::dtv::dvbt2_inband_t dvbt2_inband_t;
typedef gr::dtv::dvbt2_equalization_t dvbt2_equalization_t;
typedef gr::dtv::dvbt2_bandwidth_t dvbt2_bandwidth_t;

#endif /* INCLUDED_DTV_DVBT2_CONFIG_H */


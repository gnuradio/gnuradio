/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
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

using dvbt2_rotation_t = gr::dtv::dvbt2_rotation_t;
using dvbt2_streamtype_t = gr::dtv::dvbt2_streamtype_t;
using dvbt2_inputmode_t = gr::dtv::dvbt2_inputmode_t;
using dvbt2_extended_carrier_t = gr::dtv::dvbt2_extended_carrier_t;
using dvbt2_preamble_t = gr::dtv::dvbt2_preamble_t;
using dvbt2_fftsize_t = gr::dtv::dvbt2_fftsize_t;
using dvbt2_papr_t = gr::dtv::dvbt2_papr_t;
using dvbt2_l1constellation_t = gr::dtv::dvbt2_l1constellation_t;
using dvbt2_pilotpattern_t = gr::dtv::dvbt2_pilotpattern_t;
using dvbt2_version_t = gr::dtv::dvbt2_version_t;
using dvbt2_reservedbiasbits_t = gr::dtv::dvbt2_reservedbiasbits_t;
using dvbt2_l1scrambled_t = gr::dtv::dvbt2_l1scrambled_t;
using dvbt2_misogroup_t = gr::dtv::dvbt2_misogroup_t;
using dvbt2_showlevels_t = gr::dtv::dvbt2_showlevels_t;
using dvbt2_inband_t = gr::dtv::dvbt2_inband_t;
using dvbt2_equalization_t = gr::dtv::dvbt2_equalization_t;
using dvbt2_bandwidth_t = gr::dtv::dvbt2_bandwidth_t;

#endif /* INCLUDED_DTV_DVBT2_CONFIG_H */

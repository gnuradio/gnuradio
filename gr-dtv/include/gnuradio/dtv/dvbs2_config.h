/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_DVBS2_CONFIG_H
#define INCLUDED_DTV_DVBS2_CONFIG_H

namespace gr {
namespace dtv {
enum dvbs2_rolloff_factor_t {
    RO_0_35 = 0,
    RO_0_25,
    RO_0_20,
    RO_RESERVED,
    RO_0_15,
    RO_0_10,
    RO_0_05,
};

enum dvbs2_pilots_t {
    PILOTS_OFF = 0,
    PILOTS_ON,
};

enum dvbs2_interpolation_t {
    INTERPOLATION_OFF = 0,
    INTERPOLATION_ON,
};

} // namespace dtv
} // namespace gr

typedef gr::dtv::dvbs2_rolloff_factor_t dvbs2_rolloff_factor_t;
typedef gr::dtv::dvbs2_pilots_t dvbs2_pilots_t;
typedef gr::dtv::dvbs2_interpolation_t dvbs2_interpolation_t;

#endif /* INCLUDED_DTV_DVBS2_CONFIG_H */

/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_DVBT_CONFIG_H
#define INCLUDED_DTV_DVBT_CONFIG_H

namespace gr {
namespace dtv {
enum dvbt_hierarchy_t {
    NH = 0,
    ALPHA1,
    ALPHA2,
    ALPHA4,
};

enum dvbt_transmission_mode_t {
    T2k = 0,
    T8k = 1,
};

} // namespace dtv
} // namespace gr

using dvbt_hierarchy_t = gr::dtv::dvbt_hierarchy_t;
using dvbt_transmission_mode_t = gr::dtv::dvbt_transmission_mode_t;

#endif /* INCLUDED_DTV_DVBT_CONFIG_H */

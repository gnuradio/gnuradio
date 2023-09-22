/* -*- c++ -*- */
/*
 * Copyright 2017 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_CATV_CONFIG_H
#define INCLUDED_DTV_CATV_CONFIG_H

namespace gr {
namespace dtv {
enum catv_constellation_t {
    CATV_MOD_64QAM = 0,
    CATV_MOD_256QAM,
};

} // namespace dtv
} // namespace gr

typedef gr::dtv::catv_constellation_t catv_constellation_t;

#endif /* INCLUDED_DTV_CATV_CONFIG_H */

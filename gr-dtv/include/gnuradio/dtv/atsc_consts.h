/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_ATSC_CONSTS_H
#define INCLUDED_DTV_ATSC_CONSTS_H

#include <gnuradio/dtv/api.h>

namespace gr {
namespace dtv {

// These will go into an mpeg_consts.h once other mod/demods are done
static constexpr int ATSC_MPEG_DATA_LENGTH = 187;
static constexpr int ATSC_MPEG_PKT_LENGTH = 188; // sync + data
static constexpr int ATSC_MPEG_RS_ENCODED_LENGTH = 207;
static constexpr int MPEG_SYNC_BYTE = 0x47;
static constexpr int MPEG_TRANSPORT_ERROR_BIT = 0x80; // top bit of byte after SYNC

// ATSC specific constants
static constexpr double ATSC_SYMBOL_RATE = 4.5e6 / 286 * 684;            // ~10.76 MHz
static constexpr double ATSC_DATA_SEGMENT_RATE = ATSC_SYMBOL_RATE / 832; // ~12.935 kHz
static constexpr int ATSC_DATA_SEGMENT_LENGTH =
    832;                                         // includes 4 sync symbols at beginning
static constexpr int ATSC_DSEGS_PER_FIELD = 312; // regular data segs / field

} /* namespace dtv */
} /* namespace gr */

#endif /* INCLUDED_DTV_ATSC_CONSTS_H */

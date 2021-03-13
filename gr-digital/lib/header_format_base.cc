/* -*- c++ -*- */
/* Copyright 2016 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/digital/header_format_base.h>
#include <gnuradio/math.h>
#include <volk/volk.h>
#include <cstring>
#include <iostream>

namespace gr {
namespace digital {

header_format_base::header_format_base()
{
    enter_search();
    configure_default_loggers(d_logger, d_debug_logger, "packet formatter");
}

header_format_base::~header_format_base() {}

size_t header_format_base::header_nbytes() const { return header_nbits() / 8; }

inline void header_format_base::enter_search() { d_state = STATE_SYNC_SEARCH; }

inline void header_format_base::enter_have_sync() { d_state = STATE_HAVE_SYNC; }

inline void header_format_base::enter_have_header(int payload_len)
{
    d_state = STATE_SYNC_SEARCH;
}

} /* namespace digital */
} /* namespace gr */

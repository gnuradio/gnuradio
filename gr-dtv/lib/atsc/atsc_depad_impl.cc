/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "atsc_depad_impl.h"
#include "gnuradio/dtv/atsc_consts.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace dtv {

atsc_depad::sptr atsc_depad::make()
{
    return gnuradio::make_block_sptr<atsc_depad_impl>();
}

atsc_depad_impl::atsc_depad_impl()
    : gr::sync_interpolator(
          "atsc_depad",
          io_signature::make(1, 1, ATSC_MPEG_PKT_LENGTH * sizeof(uint8_t)),
          io_signature::make(1, 1, sizeof(uint8_t)),
          ATSC_MPEG_PKT_LENGTH)
{
}

int atsc_depad_impl::work(int noutput_items,
                          gr_vector_const_void_star& input_items,
                          gr_vector_void_star& output_items)
{
    auto in = static_cast<const uint8_t*>(input_items[0]);
    auto out = static_cast<uint8_t*>(output_items[0]);

    int i;

    for (i = 0; i < noutput_items / ATSC_MPEG_PKT_LENGTH; i++)
        memcpy(&out[i * ATSC_MPEG_PKT_LENGTH],
               &in[i * ATSC_MPEG_PKT_LENGTH],
               ATSC_MPEG_PKT_LENGTH);

    return i * ATSC_MPEG_PKT_LENGTH;
}

} /* namespace dtv */
} /* namespace gr */

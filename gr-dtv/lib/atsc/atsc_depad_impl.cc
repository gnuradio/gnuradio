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
    return gnuradio::get_initial_sptr(new atsc_depad_impl());
}

atsc_depad_impl::atsc_depad_impl()
    : gr::sync_interpolator("atsc_depad",
                            io_signature::make(1, 1, sizeof(atsc_mpeg_packet)),
                            io_signature::make(1, 1, sizeof(unsigned char)),
                            ATSC_MPEG_PKT_LENGTH)
{
}

int atsc_depad_impl::work(int noutput_items,
                          gr_vector_const_void_star& input_items,
                          gr_vector_void_star& output_items)
{
    const atsc_mpeg_packet* in = (const atsc_mpeg_packet*)input_items[0];
    unsigned char* out = (unsigned char*)output_items[0];

    int i;

    for (i = 0; i < noutput_items / ATSC_MPEG_PKT_LENGTH; i++)
        memcpy(&out[i * ATSC_MPEG_PKT_LENGTH], in[i].data, ATSC_MPEG_PKT_LENGTH);

    return i * ATSC_MPEG_PKT_LENGTH;
}

} /* namespace dtv */
} /* namespace gr */

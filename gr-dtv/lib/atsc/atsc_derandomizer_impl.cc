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

#include "atsc_derandomizer_impl.h"
#include "gnuradio/dtv/atsc_consts.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace dtv {

atsc_derandomizer::sptr atsc_derandomizer::make()
{
    return gnuradio::get_initial_sptr(new atsc_derandomizer_impl());
}

atsc_derandomizer_impl::atsc_derandomizer_impl()
    : gr::sync_block("dtv_atsc_derandomizer",
                     io_signature::make(1, 1, sizeof(atsc_mpeg_packet_no_sync)),
                     io_signature::make(1, 1, sizeof(atsc_mpeg_packet)))
{
    d_rand.reset();
}

int atsc_derandomizer_impl::work(int noutput_items,
                                 gr_vector_const_void_star& input_items,
                                 gr_vector_void_star& output_items)
{
    const atsc_mpeg_packet_no_sync* in = (const atsc_mpeg_packet_no_sync*)input_items[0];
    atsc_mpeg_packet* out = (atsc_mpeg_packet*)output_items[0];

    for (int i = 0; i < noutput_items; i++) {

        assert(in[i].pli.regular_seg_p());

        if (in[i].pli.first_regular_seg_p())
            d_rand.reset();

        d_rand.derandomize(out[i], in[i]);

        // Check the pipeline info for error status and and set the
        // corresponding bit in transport packet header.

        if (in[i].pli.transport_error_p())
            out[i].data[1] |= MPEG_TRANSPORT_ERROR_BIT;
        else
            out[i].data[1] &= ~MPEG_TRANSPORT_ERROR_BIT;
    }

    return noutput_items;
}

} /* namespace dtv */
} /* namespace gr */

/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "atsc_randomizer_impl.h"
#include "gnuradio/dtv/atsc_consts.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace dtv {

atsc_randomizer::sptr atsc_randomizer::make()
{
    return gnuradio::make_block_sptr<atsc_randomizer_impl>();
}

atsc_randomizer_impl::atsc_randomizer_impl()
    : gr::sync_block("atsc_randomizer",
                     gr::io_signature::make(1, 1, sizeof(atsc_mpeg_packet)),
                     gr::io_signature::make(1, 1, sizeof(atsc_mpeg_packet_no_sync)))
{
    reset();
}

atsc_randomizer_impl::~atsc_randomizer_impl() {}

void atsc_randomizer_impl::reset()
{
    d_rand.reset();
    d_field2 = false;
    d_segno = 0;
}

int atsc_randomizer_impl::work(int noutput_items,
                               gr_vector_const_void_star& input_items,
                               gr_vector_void_star& output_items)
{
    const atsc_mpeg_packet* in = (const atsc_mpeg_packet*)input_items[0];
    atsc_mpeg_packet_no_sync* out = (atsc_mpeg_packet_no_sync*)output_items[0];

    for (int i = 0; i < noutput_items; i++) {
        // sanity check incoming data.
        assert((in[i].data[0] == MPEG_SYNC_BYTE));
        assert((in[i].data[1] & MPEG_TRANSPORT_ERROR_BIT) == 0);

        // initialize plinfo for downstream
        //
        // We do this here because the randomizer is effectively
        // the head of the tx processing chain
        //
        out[i].pli.set_regular_seg(d_field2, d_segno);
        d_segno++;

        if (d_segno == 312) {
            d_segno = 0;
            d_field2 = !d_field2;
        }

        if (out[i].pli.first_regular_seg_p()) {
            d_rand.reset();
        }

        d_rand.randomize(out[i], in[i]);
    }

    // Tell runtime system how many output items we produced.
    return noutput_items;
}

} /* namespace dtv */
} /* namespace gr */

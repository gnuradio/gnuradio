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
    return gnuradio::make_block_sptr<atsc_derandomizer_impl>();
}

atsc_derandomizer_impl::atsc_derandomizer_impl()
    : gr::sync_block("dtv_atsc_derandomizer",
                     io_signature::make2(
                         2, 2, ATSC_MPEG_PKT_LENGTH * sizeof(uint8_t), sizeof(plinfo)),
                     io_signature::make(1, 1, ATSC_MPEG_PKT_LENGTH * sizeof(uint8_t)))
{
    d_rand.reset();
}

int atsc_derandomizer_impl::work(int noutput_items,
                                 gr_vector_const_void_star& input_items,
                                 gr_vector_void_star& output_items)
{
    auto in = static_cast<const uint8_t*>(input_items[0]);
    auto out = static_cast<uint8_t*>(output_items[0]);
    auto plin = static_cast<const plinfo*>(input_items[1]);

    for (int i = 0; i < noutput_items; i++) {
        assert(plin[i].regular_seg_p());

        if (plin[i].first_regular_seg_p())
            d_rand.reset();

        d_rand.derandomize(&out[i * ATSC_MPEG_PKT_LENGTH], &in[i * ATSC_MPEG_PKT_LENGTH]);

        // Check the pipeline info for error status and and set the
        // corresponding bit in transport packet header.

        if (plin[i].transport_error_p())
            out[i * ATSC_MPEG_PKT_LENGTH + 1] |= MPEG_TRANSPORT_ERROR_BIT;
        else
            out[i * ATSC_MPEG_PKT_LENGTH + 1] &= ~MPEG_TRANSPORT_ERROR_BIT;
    }

    return noutput_items;
}

} /* namespace dtv */
} /* namespace gr */

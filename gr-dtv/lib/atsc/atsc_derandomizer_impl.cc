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
                     io_signature::make(1, 1, ATSC_MPEG_PKT_LENGTH * sizeof(uint8_t)),
                     io_signature::make(1, 1, ATSC_MPEG_PKT_LENGTH * sizeof(uint8_t)))
{
    d_rand.reset();
    set_tag_propagation_policy(TPP_DONT);
}

int atsc_derandomizer_impl::work(int noutput_items,
                                 gr_vector_const_void_star& input_items,
                                 gr_vector_void_star& output_items)
{
    auto in = static_cast<const uint8_t*>(input_items[0]);
    auto out = static_cast<uint8_t*>(output_items[0]);

    std::vector<tag_t> tags;
    auto tag_pmt = pmt::intern("plinfo");
    for (int i = 0; i < noutput_items; i++) {
        plinfo pli_in;
        get_tags_in_window(tags, 0, i, i + 1, tag_pmt);
        if (tags.size() > 0) {
            pli_in.from_tag_value(pmt::to_uint64(tags[0].value));
        } else {
            throw std::runtime_error("Atsc Derandomizer: Plinfo Tag not found on sample");
        }
        assert(pli_in.regular_seg_p());

        if (pli_in.first_regular_seg_p())
            d_rand.reset();

        d_rand.derandomize(&out[i * ATSC_MPEG_PKT_LENGTH], &in[i * ATSC_MPEG_PKT_LENGTH]);

        // Check the pipeline info for error status and and set the
        // corresponding bit in transport packet header.

        if (pli_in.transport_error_p())
            out[i * ATSC_MPEG_PKT_LENGTH + 1] |= MPEG_TRANSPORT_ERROR_BIT;
        else
            out[i * ATSC_MPEG_PKT_LENGTH + 1] &= ~MPEG_TRANSPORT_ERROR_BIT;
    }

    return noutput_items;
}

} /* namespace dtv */
} /* namespace gr */

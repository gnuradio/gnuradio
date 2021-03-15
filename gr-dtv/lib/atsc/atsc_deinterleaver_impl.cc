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

#include "atsc_deinterleaver_impl.h"
#include "gnuradio/dtv/atsc_consts.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace dtv {

atsc_deinterleaver::sptr atsc_deinterleaver::make()
{
    return gnuradio::make_block_sptr<atsc_deinterleaver_impl>();
}

atsc_deinterleaver_impl::atsc_deinterleaver_impl()
    : gr::sync_block(
          "atsc_deinterleaver",
          io_signature::make2(
              2, 2, ATSC_MPEG_RS_ENCODED_LENGTH * sizeof(uint8_t), sizeof(plinfo)),
          io_signature::make2(
              2, 2, ATSC_MPEG_RS_ENCODED_LENGTH * sizeof(uint8_t), sizeof(plinfo))),
      alignment_fifo(156)
{
    m_fifo.reserve(s_interleavers);

    for (int i = 0; i < s_interleavers; i++)
        m_fifo.emplace_back((s_interleavers - 1 - i) * 4);

    sync();
}

atsc_deinterleaver_impl::~atsc_deinterleaver_impl() {}

void atsc_deinterleaver_impl::reset()
{
    sync();

    for (auto& i : m_fifo)
        i.reset();
}

int atsc_deinterleaver_impl::work(int noutput_items,
                                  gr_vector_const_void_star& input_items,
                                  gr_vector_void_star& output_items)
{
    auto in = static_cast<const uint8_t*>(input_items[0]);
    auto out = static_cast<uint8_t*>(output_items[0]);
    auto plin = static_cast<const plinfo*>(input_items[1]);
    auto plout = static_cast<plinfo*>(output_items[1]);


    for (int i = 0; i < noutput_items; i++) {
        assert(plin[i].regular_seg_p());

        // reset commutator if required using INPUT pipeline info
        if (plin[i].first_regular_seg_p())
            sync();

        // remap OUTPUT pipeline info to reflect all data segment end-to-end delay
        plout[i] = plinfo();
        plinfo::delay(plout[i], plin[i], s_interleavers);

        // now do the actual deinterleaving
        for (unsigned int j = 0; j < ATSC_MPEG_RS_ENCODED_LENGTH; j++) {
            out[i * ATSC_MPEG_RS_ENCODED_LENGTH + j] =
                alignment_fifo.stuff(transform(in[i * ATSC_MPEG_RS_ENCODED_LENGTH + j]));
        }
    }

    return noutput_items;
}

} /* namespace dtv */
} /* namespace gr */

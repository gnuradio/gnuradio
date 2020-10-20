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

#include "atsc_types.h"
#include "atsc_viterbi_decoder_impl.h"
#include "atsc_viterbi_mux.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace dtv {

atsc_viterbi_decoder::sptr atsc_viterbi_decoder::make()
{
    return gnuradio::make_block_sptr<atsc_viterbi_decoder_impl>();
}

atsc_viterbi_decoder_impl::atsc_viterbi_decoder_impl()
    : sync_block("dtv_atsc_viterbi_decoder",
                 io_signature::make(1, 1, sizeof(atsc_soft_data_segment)),
                 io_signature::make(1, 1, sizeof(atsc_mpeg_packet_rs_encoded)))
{
    set_output_multiple(NCODERS);

    /*
     * These fifo's handle the alignment problem caused by the
     * inherent decoding delay of the individual viterbi decoders.
     * The net result is that this entire block has a pipeline latency
     * of 12 complete segments.
     *
     * If anybody cares, it is possible to do it with less delay, but
     * this approach is at least somewhat understandable...
     */

    // the -4 is for the 4 sync symbols
    const int fifo_size = ATSC_DATA_SEGMENT_LENGTH - 4 - viterbi[0].delay();
    fifo.reserve(NCODERS);
    for (int i = 0; i < NCODERS; i++)
        fifo.emplace_back(fifo_size);

    reset();
}

atsc_viterbi_decoder_impl::~atsc_viterbi_decoder_impl() {}

void atsc_viterbi_decoder_impl::reset()
{
    for (int i = 0; i < NCODERS; i++)
        fifo[i].reset();
}

std::vector<float> atsc_viterbi_decoder_impl::decoder_metrics() const
{
    std::vector<float> metrics(NCODERS);
    for (int i = 0; i < NCODERS; i++)
        metrics[i] = viterbi[i].best_state_metric();
    return metrics;
}

int atsc_viterbi_decoder_impl::work(int noutput_items,
                                    gr_vector_const_void_star& input_items,
                                    gr_vector_void_star& output_items)
{
    const atsc_soft_data_segment* in = (const atsc_soft_data_segment*)input_items[0];
    atsc_mpeg_packet_rs_encoded* out = (atsc_mpeg_packet_rs_encoded*)output_items[0];

    // The way the fs_checker works ensures we start getting packets
    // starting with a field sync, and out input multiple is set to
    // 12, so we should always get a mod 12 numbered first packet
    assert(noutput_items % NCODERS == 0);

    int dbwhere;
    int dbindex;
    int shift;
    float symbols[NCODERS][enco_which_max];
    unsigned char dibits[NCODERS][enco_which_max];

    unsigned char out_copy[OUTPUT_SIZE];

    for (int i = 0; i < noutput_items; i += NCODERS) {
        /* Build a continuous symbol buffer for each encoder */
        for (unsigned int encoder = 0; encoder < NCODERS; encoder++)
            for (unsigned int k = 0; k < enco_which_max; k++)
                symbols[encoder][k] = in[i + (enco_which_syms[encoder][k] / 832)]
                                          .data[enco_which_syms[encoder][k] % 832];

        /* Now run each of the 12 Viterbi decoders over their subset of
           the input symbols */
        for (unsigned int encoder = 0; encoder < NCODERS; encoder++)
            for (unsigned int k = 0; k < enco_which_max; k++)
                dibits[encoder][k] = viterbi[encoder].decode(symbols[encoder][k]);

        /* Move dibits into their location in the output buffer */
        for (unsigned int encoder = 0; encoder < NCODERS; encoder++) {
            for (unsigned int k = 0; k < enco_which_max; k++) {
                /* Store the dibit into the output data segment */
                dbwhere = enco_which_dibits[encoder][k];
                dbindex = dbwhere >> 3;
                shift = dbwhere & 0x7;
                out_copy[dbindex] = (out_copy[dbindex] & ~(0x03 << shift)) |
                                    (fifo[encoder].stuff(dibits[encoder][k]) << shift);
            } /* Symbols fed into one encoder */
        }     /* Encoders */

        // copy output from contiguous temp buffer into final output
        for (int j = 0; j < NCODERS; j++) {
            memcpy(&out[i + j].data[0],
                   &out_copy[j * OUTPUT_SIZE / NCODERS],
                   ATSC_MPEG_RS_ENCODED_LENGTH * sizeof(out_copy[0]));

            // adjust pipeline info to reflect 12 segment delay
            plinfo::delay(out[i + j].pli, in[i + j].pli, NCODERS);
        }
    }

    return noutput_items;
}

void atsc_viterbi_decoder_impl::setup_rpc()
{
#ifdef GR_CTRLPORT
    add_rpc_variable(
        rpcbasic_sptr(new rpcbasic_register_get<atsc_viterbi_decoder, std::vector<float>>(
            alias(),
            "decoder_metrics",
            &atsc_viterbi_decoder::decoder_metrics,
            pmt::make_f32vector(1, 0),
            pmt::make_f32vector(1, 100000),
            pmt::make_f32vector(1, 0),
            "",
            "Viterbi decoder metrics",
            RPC_PRIVLVL_MIN,
            DISPTIME)));
#endif /* GR_CTRLPORT */
}

} /* namespace dtv */
} /* namespace gr */

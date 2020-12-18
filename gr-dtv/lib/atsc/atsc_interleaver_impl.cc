/* -*- c++ -*- */
/*
 * Copyright 2015,2016 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "atsc_interleaver_impl.h"
#include "gnuradio/dtv/atsc_consts.h"

namespace gr {
namespace dtv {

atsc_interleaver::sptr atsc_interleaver::make()
{
    return gnuradio::make_block_sptr<atsc_interleaver_impl>();
}

atsc_interleaver_impl::atsc_interleaver_impl()
    : gr::sync_block("atsc_interleaver",
                     gr::io_signature::make(1, 1, sizeof(atsc_mpeg_packet_rs_encoded)),
                     gr::io_signature::make(1, 1, sizeof(atsc_mpeg_packet_rs_encoded)))
{
    I = 52; /* ATSC interleaver */
    J = 4;
    registers = (unsigned char*)malloc(sizeof(unsigned char) * I * ((I - 1) * J));
    if (registers == NULL) {
        GR_LOG_FATAL(d_logger, "ATSC Interleaver, cannot allocate memory for registers.");
        throw std::bad_alloc();
    }

    pointers = (int*)malloc(sizeof(int) * I);
    if (pointers == NULL) {
        free(registers);
        GR_LOG_FATAL(d_logger, "ATSC Interleaver, cannot allocate memory for pointers");
        throw std::bad_alloc();
    }

    memset(registers, 0, sizeof(unsigned char) * I * ((I - 1) * J));
    memset(pointers, 0, sizeof(int) * I);

    commutator = 0;
}

atsc_interleaver_impl::~atsc_interleaver_impl()
{
    free(pointers);
    free(registers);
}

int atsc_interleaver_impl::work(int noutput_items,
                                gr_vector_const_void_star& input_items,
                                gr_vector_void_star& output_items)
{
    const atsc_mpeg_packet_rs_encoded* in =
        (const atsc_mpeg_packet_rs_encoded*)input_items[0];
    atsc_mpeg_packet_rs_encoded* out = (atsc_mpeg_packet_rs_encoded*)output_items[0];
    int p, n = ATSC_MPEG_RS_ENCODED_LENGTH;

    for (int i = 0; i < noutput_items; i++) {
        assert(in[i].pli.regular_seg_p());

        out[i].pli = in[i].pli;                // copy pipeline info
        if (in[i].pli.first_regular_seg_p()) { // reset commutator if required
            commutator = 0;
        }
        for (int j = 0; j < n; j++) {
            if (commutator == 0) {
                out[i].data[j] = in[i].data[j];
            } else {
                p = pointers[commutator];

                out[i].data[j] = registers[commutator * (I - 1) * J + p];
                registers[commutator * (I - 1) * J + p] = in[i].data[j];

                pointers[commutator] = (p + 1) % (commutator * J);
            }

            commutator = (commutator + 1) % I;
        }
    }

    // Tell runtime system how many output items we produced.
    return noutput_items;
}

} /* namespace dtv */
} /* namespace gr */

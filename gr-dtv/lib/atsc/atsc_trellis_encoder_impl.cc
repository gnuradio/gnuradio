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

#include "atsc_trellis_encoder_impl.h"
#include "gnuradio/dtv/atsc_consts.h"
#include <cstdio>

namespace gr {
namespace dtv {

static constexpr int DIBITS_PER_BYTE = 4;

#define SEGOF(x) ((x) / ((SEGMENT_SIZE + 1) * DIBITS_PER_BYTE))
#define SYMOF(x) (((x) % ((SEGMENT_SIZE + 1) * DIBITS_PER_BYTE)) - 4)

#define ENCODER_SEG_BUMP 4

/* A Segment sync symbol is an 8VSB +5,-5,-5,+5 sequence that occurs at
   the start of each 207-byte segment (including field sync segments).  */
#define DSEG_SYNC_SYM1 0x06 /* +5 */
#define DSEG_SYNC_SYM2 0x01 /* -5 */
#define DSEG_SYNC_SYM3 0x01 /* -5 */
#define DSEG_SYNC_SYM4 0x06 /* +5 */

/* Shift counts to bit numbers (high order, low order); 9x entries unused */
static const int bit1[8] = { 1, 99, 3, 98, 5, 97, 7, 96 };
static const int bit2[8] = { 0, 99, 2, 98, 4, 97, 6, 96 };

atsc_trellis_encoder::sptr atsc_trellis_encoder::make()
{
    return gnuradio::make_block_sptr<atsc_trellis_encoder_impl>();
}

atsc_trellis_encoder_impl::atsc_trellis_encoder_impl()
    : gr::sync_block("atsc_trellis_encoder",
                     gr::io_signature::make(1, 1, sizeof(atsc_mpeg_packet_rs_encoded)),
                     gr::io_signature::make(1, 1, sizeof(atsc_data_segment)))
{
    reset();
    debug = false;
    set_output_multiple(NCODERS);
}

atsc_trellis_encoder_impl::~atsc_trellis_encoder_impl() {}

void atsc_trellis_encoder_impl::reset()
{
    for (int i = 0; i < NCODERS; i++) {
        enc[i].reset();
    }
}

void atsc_trellis_encoder_impl::encode(atsc_data_segment out[NCODERS],
                                       const atsc_mpeg_packet_rs_encoded in[NCODERS])
{
    unsigned char out_copy[OUTPUT_SIZE];
    unsigned char in_copy[INPUT_SIZE];

    static_assert(sizeof(in_copy) == sizeof(in[0].data) * NCODERS, "wrong type size");
    static_assert(sizeof(out_copy) == sizeof(out[0].data) * NCODERS, "wrong type size");

    // copy input into contiguous temporary buffer
    for (int i = 0; i < NCODERS; i++) {
        assert(in[i].pli.regular_seg_p());
        memcpy(&in_copy[i * INPUT_SIZE / NCODERS],
               &in[i].data[0],
               ATSC_MPEG_RS_ENCODED_LENGTH * sizeof(in_copy[0]));
    }

    memset(out_copy, 0, sizeof(out_copy)); // FIXME, sanity check

    // do the deed...
    encode_helper(out_copy, in_copy);

    // copy output from contiguous temp buffer into final output
    for (int i = 0; i < NCODERS; i++) {
        memcpy(&out[i].data[0],
               &out_copy[i * OUTPUT_SIZE / NCODERS],
               ATSC_DATA_SEGMENT_LENGTH * sizeof(out_copy[0]));

        // copy pipeline info
        out[i].pli = in[i].pli;

        assert(out[i].pli.regular_seg_p());
    }
}

/*
 * This code expects contiguous arrays. Use it as is, it computes
 * the correct answer. Maybe someday, when we've run out of better
 * things to do, rework to avoid the copying in encode.
 */
void atsc_trellis_encoder_impl::encode_helper(unsigned char output[OUTPUT_SIZE],
                                              const unsigned char input[INPUT_SIZE])
{
    int i;
    int encoder;
    unsigned char trellis_buffer[NCODERS];
    int trellis_wherefrom[NCODERS];
    unsigned char *out, *next_out_seg;
    int chunk;
    int shift;
    unsigned char dibit;
    unsigned char symbol;
    int skip_encoder_bump;

    /* FIXME, we may want special processing here for a flag
     * byte to keep track of which part of the field we're in?
     */

    encoder = NCODERS - ENCODER_SEG_BUMP;
    skip_encoder_bump = 0;
    out = output;
    next_out_seg = out;

    for (chunk = 0; chunk < INPUT_SIZE; chunk += NCODERS) {
        /* Load a new chunk of bytes into the Trellis encoder buffers.
         * They get loaded in an order that depends on where we are in
         * the segment sync progress (sigh).
         * GRR! When the chunk reload happens at the same time as the
         * segment boundary, we should bump the encoder NOW for the reload,
         * rather than LATER during the bitshift transition!!!
         */
        if (out >= next_out_seg) {
            encoder = (encoder + ENCODER_SEG_BUMP) % NCODERS;
            skip_encoder_bump = 1;
        }

        for (i = 0; i < NCODERS; i++) {
            /* for debug */
            trellis_wherefrom[encoder] = chunk + i;
            trellis_buffer[encoder] = input[chunk + i];
            encoder++;
            if (encoder >= NCODERS) {
                encoder = 0;
            }
        }

        for (shift = 6; shift >= 0; shift -= 2) {
            /* Segment boundaries happen to occur on some bitshift transitions. */
            if (out >= next_out_seg) {
                /* Segment transition. Output a data segment sync symbol,
                 * and mess with the trellis encoder mux.
                 */
                *out++ = DSEG_SYNC_SYM1;
                *out++ = DSEG_SYNC_SYM2;
                *out++ = DSEG_SYNC_SYM3;
                *out++ = DSEG_SYNC_SYM4;
                if (debug) {
                    printf("SYNC SYNC SYNC SYNC\n");
                }

                next_out_seg = out + (SEGMENT_SIZE * DIBITS_PER_BYTE);

                if (!skip_encoder_bump) {
                    encoder = (encoder + ENCODER_SEG_BUMP) % NCODERS;
                }
                skip_encoder_bump = 0;
            }

            /* Now run each of the 12 Trellis encoders to spit out 12 symbols.
             * Each encoder takes input from the same byte of the chunk, but
             * the outputs of the encoders come out in various orders.
             * NOPE -- this is false. The encoders take input from various
             * bytes of the chunk (which changes at segment sync time), AND
             * they also come out in various orders. You really do have to
             * keep separate track of: the input bytes, the encoders, and
             * the output bytes -- because they're all moving with respect
             * to each other!!!
             */
            for (i = 0; i < NCODERS; i++) {
                dibit = 0x03 & (trellis_buffer[encoder] >> shift);
                if (debug) {
                    printf("Seg %ld Symb %3ld Trell %2d Byte %6d Bits %d-%d = dibit %d ",
                           (long)SEGOF(out - output),
                           (long)SYMOF(out - output),
                           encoder,
                           trellis_wherefrom[encoder],
                           bit1[shift],
                           bit2[shift],
                           dibit);
                }

                symbol = enc[encoder].encode(dibit);
                *out++ = symbol;
                encoder++;
                if (encoder >= NCODERS) {
                    encoder = 0;
                }
                if (debug) {
                    printf("sym %d\n", symbol);
                }

            } /* Encoders */
        }     /* Bit shifts */
    }         /* Chunks */

    /* Check up on ourselves */
    static_assert(0 == (INPUT_SIZE * DIBITS_PER_BYTE) % NCODERS,
                  "size not divisible by NCODERS");
    assert(OUTPUT_SIZE == out - output);
    assert(NCODERS - ENCODER_SEG_BUMP == encoder);
}

int atsc_trellis_encoder_impl::work(int noutput_items,
                                    gr_vector_const_void_star& input_items,
                                    gr_vector_void_star& output_items)
{
    const atsc_mpeg_packet_rs_encoded* in =
        (const atsc_mpeg_packet_rs_encoded*)input_items[0];
    atsc_data_segment* out = (atsc_data_segment*)output_items[0];

    for (int i = 0; i < noutput_items; i += NCODERS) {
        encode(&out[i], &in[i]);
    }

    // Tell runtime system how many output items we produced.
    return noutput_items;
}

} /* namespace dtv */
} /* namespace gr */

/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "simple_correlator_impl.h"
#include <gnuradio/blocks/count_bits.h>
#include <gnuradio/digital/simple_framer_sync.h>
#include <gnuradio/io_signature.h>
#include <cstring>
#include <stdexcept>

namespace gr {
namespace digital {

static const int THRESHOLD = 3;

simple_correlator::sptr simple_correlator::make(int payload_bytesize)
{
    return gnuradio::make_block_sptr<simple_correlator_impl>(payload_bytesize);
}

simple_correlator_impl::simple_correlator_impl(int payload_bytesize)
    : block("simple_correlator",
            io_signature::make(1, 1, sizeof(float)),
            io_signature::make(1, 1, sizeof(unsigned char))),
      d_payload_bytesize(payload_bytesize),
      d_bblen((payload_bytesize + GRSF_PAYLOAD_OVERHEAD) * GRSF_BITS_PER_BYTE),
      d_bitbuf(d_bblen),
      d_pktbuf(d_bblen / GRSF_BITS_PER_BYTE)
{
    for (int i = 0; i < AVG_PERIOD; i++) {
        d_avgbuf[i] = 0.0;
    }

    enter_looking();
}

simple_correlator_impl::~simple_correlator_impl() {}

void simple_correlator_impl::enter_looking()
{
    fflush(stdout);
    // fprintf(stderr, ">>> enter_looking\n");
    d_state = ST_LOOKING;
    for (int i = 0; i < OVERSAMPLE; i++)
        d_shift_reg[i] = 0;
    d_osi = 0;

    d_avbi = 0;
    d_avg = d_avg * 0.5;
    d_accum = 0;
    for (int i = 0; i < AVG_PERIOD; i++)
        d_avgbuf[i] = 0.0;
}

void simple_correlator_impl::enter_under_threshold()
{
    fflush(stdout);
    // fprintf(stderr, ">>> enter_under_threshold\n");
    d_state = ST_UNDER_THRESHOLD;
    d_transition_osi = d_osi;
}

void simple_correlator_impl::enter_locked()
{
    d_state = ST_LOCKED;
    int delta = sub_index(d_osi, d_transition_osi);
    d_center_osi = add_index(d_transition_osi, delta / 2);
    // d_center_osi = add_index(d_center_osi, 3);   // FIXME
    d_bbi = 0;
    fflush(stdout);
    // fprintf(stderr, ">>> enter_locked  d_center_osi = %d\n", d_center_osi);

    d_avg = std::max(-1.0, std::min(1.0, d_accum * (1.0 / AVG_PERIOD)));
    // fprintf(stderr, ">>> enter_locked  d_avg = %g\n", d_avg);
}

static void packit(unsigned char* pktbuf, const unsigned char* bitbuf, int bitcount)
{
    for (int i = 0; i < bitcount; i += 8) {
        int t = bitbuf[i + 0] & 0x1;
        t = (t << 1) | (bitbuf[i + 1] & 0x1);
        t = (t << 1) | (bitbuf[i + 2] & 0x1);
        t = (t << 1) | (bitbuf[i + 3] & 0x1);
        t = (t << 1) | (bitbuf[i + 4] & 0x1);
        t = (t << 1) | (bitbuf[i + 5] & 0x1);
        t = (t << 1) | (bitbuf[i + 6] & 0x1);
        t = (t << 1) | (bitbuf[i + 7] & 0x1);
        *pktbuf++ = t;
    }
}

void simple_correlator_impl::update_avg(float x)
{
    d_accum -= d_avgbuf[d_avbi];
    d_avgbuf[d_avbi] = x;
    d_accum += x;
    d_avbi = (d_avbi + 1) & (AVG_PERIOD - 1);
}

int simple_correlator_impl::general_work(int noutput_items,
                                         gr_vector_int& ninput_items,
                                         gr_vector_const_void_star& input_items,
                                         gr_vector_void_star& output_items)
{
    const float* in = (const float*)input_items[0];
    unsigned char* out = (unsigned char*)output_items[0];

    int n = 0;
    int nin = ninput_items[0];
    int decision;
    int hamming_dist;

#ifdef DEBUG_SIMPLE_CORRELATOR
    struct debug_data {
        float raw_data;
        bool sampled;
        bool enter_locked;
    } debug_data;
#endif

    while (n < nin) {

#ifdef DEBUG_SIMPLE_CORRELATOR
        debug_data.raw_data = in[n];
        debug_data.sampled = false;
        debug_data.enter_locked = false;
#endif

        switch (d_state) {
        case ST_LOCKED:
            if (d_osi == d_center_osi) {

#ifdef DEBUG_SIMPLE_CORRELATOR
                debug_data.sampled = true;
#endif
                decision = slice(in[n]);

                d_bitbuf[d_bbi] = decision;
                d_bbi++;
                if (d_bbi >= d_bblen) {
                    // printf("got whole packet\n");
                    packit(d_pktbuf.data(), d_bitbuf.data(), d_bbi);
                    // printf("seqno %3d\n", d_pktbuf[0]);
                    memcpy(out, &d_pktbuf[GRSF_PAYLOAD_OVERHEAD], d_payload_bytesize);
                    enter_looking();
                    consume_each(n + 1);
                    return d_payload_bytesize;
                }
            }
            break;

        case ST_LOOKING:
        case ST_UNDER_THRESHOLD:
            update_avg(in[n]);
            decision = slice(in[n]);
            d_shift_reg[d_osi] = (d_shift_reg[d_osi] << 1) | decision;

            hamming_dist = gr::blocks::count_bits64(d_shift_reg[d_osi] ^ GRSF_SYNC);
            // printf("%2d  %d\n", hamming_dist, d_osi);

            if (d_state == ST_LOOKING && hamming_dist <= THRESHOLD) {
                // We're seeing a good PN code, remember location
                enter_under_threshold();
            } else if (d_state == ST_UNDER_THRESHOLD && hamming_dist > THRESHOLD) {
                // no longer seeing good PN code, compute center of goodness
                enter_locked();
#ifdef DEBUG_SIMPLE_CORRELATOR
                debug_data.enter_locked = true;
#endif
            }
            break;
        default:
            GR_LOG_ERROR(d_logger, "Unknown simple correlator state encountered");
            throw std::runtime_error("unknown simple correlator state");
        }

#ifdef DEBUG_SIMPLE_CORRELATOR
        GR_LOG_TRACE(d_debug_logger,
                     (boost::format("%e %c%c") % debug_data.raw_data %
                      (debug_data.sampled ? 'S' : ' ') %
                      (debug_data.enter_locked ? 'L' : ' ')));
#endif

        d_osi = add_index(d_osi, 1);
        n++;
    }

    consume_each(n);
    return 0;
}

} /* namespace digital */
} /* namespace gr */

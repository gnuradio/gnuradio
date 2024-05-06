/* -*- c++ -*- */
/*
 * Copyright 2013-2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <array>
#include <stdexcept>
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "cc_decoder_impl.h"
#include <volk/volk.h>
#include <cmath>
#include <cstdio>
#include <sstream>
#include <vector>

namespace gr {
namespace fec {
namespace code {

generic_decoder::sptr cc_decoder::make(int frame_size,
                                       int k,
                                       int rate,
                                       std::vector<int> polys,
                                       int start_state,
                                       int end_state,
                                       cc_mode_t mode,
                                       bool padded)
{
    return generic_decoder::sptr(new cc_decoder_impl(
        frame_size, k, rate, polys, start_state, end_state, mode, padded));
}

cc_decoder_impl::cc_decoder_impl(int frame_size,
                                 int k,
                                 int rate,
                                 std::vector<int> polys,
                                 int start_state,
                                 int end_state,
                                 cc_mode_t mode,
                                 bool padded)
    : generic_decoder("cc_decoder"),
      d_parity_table(initial_parity_table()),
      d_max_frame_size(frame_size),
      d_frame_size(frame_size),
      d_polys(polys),
      d_mode(mode),
      d_padding(
          /* set up a padding factor. If padding, the encoded frame was extended by this
           * many bits to fit into a full byte.
           */
          padded && mode == CC_TERMINATED
              ? static_cast<int>(8.0f * ceilf(s_rate * (s_k - 1) / 8.0f) -
                                 (s_rate * (s_k - 1)))
              : 0),
      d_start_state_chaining(start_state),
      d_start_state_nonchaining(start_state),
      d_end_state_nonchaining(end_state)
{
    if (k != 7 || rate != 2) {
        // used to throw std::runtime_error("cc_decoder: parameters not supported");
        throw std::invalid_argument(
            "cc_decoder: Only k=7, rate=2 convolutional codes are supported");
    }

    switch (d_mode) {
    case (CC_TAILBITING):
        d_end_state = &d_end_state_chaining;
        d_veclen = d_frame_size + (6 * (s_k - 1));
        d_managed_in.resize(d_veclen * s_rate);
        break;

    case (CC_TRUNCATED):
        d_veclen = d_frame_size;
        d_end_state = &d_end_state_chaining;
        break;

    case (CC_TERMINATED):
        d_veclen = d_frame_size + s_k - 1;
        d_end_state =
            (end_state == -1) ? &d_end_state_chaining : &d_end_state_nonchaining;
        break;

    case (CC_STREAMING):
        d_veclen = d_frame_size + s_k - 1;
        d_end_state = &d_end_state_chaining;
        break;

    default:
        throw std::runtime_error("cc_decoder: mode not recognized");
    }

    d_vp.metrics.resize(2 * s_numstates);
    d_vp.metrics1.t = d_vp.metrics.data();
    d_vp.metrics2.t = d_vp.metrics.data() + s_numstates;

    d_vp.decisions.resize(d_veclen * s_decision_t_size);

    create_viterbi();
}

cc_decoder_impl::~cc_decoder_impl() {}

int cc_decoder_impl::get_output_size()
{
    // unpacked bits
    return d_frame_size;
}

int cc_decoder_impl::get_input_size()
{
    if (d_mode == CC_TERMINATED) {
        return s_rate * (d_frame_size + s_k - 1) + d_padding;
    } else {
        return s_rate * d_frame_size;
    }
}

int cc_decoder_impl::get_input_item_size() { return 1; }

int cc_decoder_impl::get_history()
{
    if (d_mode == CC_STREAMING) {
        return s_rate * (s_k - 1);
    } else {
        return 0;
    }
}

float cc_decoder_impl::get_shift() { return 128.0f; }

const char* cc_decoder_impl::get_input_conversion() { return "uchar"; }

void cc_decoder_impl::create_viterbi()
{
    for (unsigned int state = 0; state < s_numstates / 2; state++) {
        for (unsigned int i = 0; i < s_rate; i++) {
            d_branchtab[i * s_numstates / 2 + state] =
                (d_polys[i] < 0) ^ parity((2 * state) & abs(d_polys[i])) ? 255 : 0;
        }
    }

    switch (d_mode) {
    case (CC_STREAMING):
        d_start_state = &d_start_state_chaining;
        init_viterbi_unbiased(&d_vp);
        break;

    case (CC_TAILBITING):
        d_start_state = &d_start_state_nonchaining;
        init_viterbi_unbiased(&d_vp);
        break;

    case (CC_TRUNCATED):
    case (CC_TERMINATED):
        d_start_state = &d_start_state_nonchaining;
        init_viterbi(&d_vp, *d_start_state);
        break;

    default:
        throw std::runtime_error("cc_decoder: mode not recognized");
    }

    return;
}

int cc_decoder_impl::parity(int x)
{
    x ^= (x >> 16);
    x ^= (x >> 8);
    return d_parity_table[x];
}


constexpr std::array<unsigned char, 256> cc_decoder_impl::initial_parity_table()
{
    std::array<unsigned char, 256> parity_table{};
    /* Initialize parity lookup table */
    for (unsigned int i = 0; i < 256; i++) {
        unsigned char count = 0;
        auto value = i;
        for (count = 0; value; value >>= 1) {
            count += value & 1;
        }
        parity_table[i] = count & 1;
    }
    return parity_table;
}

int cc_decoder_impl::init_viterbi(v* vp, int starting_state)
{
    if (vp == NULL)
        return -1;
    for (unsigned int i = 0; i < s_numstates; i++) {
        vp->metrics1.t[i] = 63;
    }

    vp->old_metrics = vp->metrics1;
    vp->new_metrics = vp->metrics2;
    vp->old_metrics.t[starting_state & (s_numstates - 1)] =
        0; /* Bias known start state */
    return 0;
}

int cc_decoder_impl::init_viterbi_unbiased(struct v* vp)
{
    if (vp == NULL)
        return -1;
    for (unsigned i = 0; i < s_numstates; i++)
        vp->metrics1.t[i] = 31;

    vp->old_metrics = vp->metrics1;
    vp->new_metrics = vp->metrics2;
    // no bias step
    return 0;
}

int cc_decoder_impl::find_endstate()
{
    unsigned char* met =
        ((s_k + d_veclen) % 2 == 0) ? d_vp.new_metrics.t : d_vp.old_metrics.t;

    unsigned char min = met[0];
    int state = 0;
    for (unsigned int i = 1; i < s_numstates; ++i) {
        if (met[i] < min) {
            min = met[i];
            state = i;
        }
    }
    // printf("min %d\n", state);
    return state;
}

int cc_decoder_impl::update_viterbi_blk(const unsigned char* syms, int nbits)
{
    unsigned char* d = d_vp.decisions.data();

    memset(d, 0, s_decision_t_size * nbits);

    volk_8u_x4_conv_k7_r2_8u(d_vp.new_metrics.t,
                             d_vp.old_metrics.t,
                             const_cast<unsigned char*>(syms),
                             d,
                             nbits - (s_k - 1),
                             s_k - 1,
                             d_branchtab.data());

    return 0;
}

int cc_decoder_impl::chainback_viterbi(unsigned char* data,
                                       unsigned int nbits,
                                       unsigned int endstate,
                                       unsigned int tailsize)
{
    /* ADDSHIFT and SUBSHIFT make sure that the thing returned is a byte. */
    unsigned char* d = d_vp.decisions.data();
    /* Make room beyond the end of the encoder register so we can
     * accumulate a full byte of decoded data
     */

    endstate = (endstate % s_numstates) << s_ADDSHIFT;

    /* The store into data[] only needs to be done every 8 bits.
     * But this avoids a conditional branch, and the writes will
     * combine in the cache anyway
     */

    d += tailsize * s_decision_t_size; /* Look past tail */
    int retval;
    int dif = tailsize - (s_k - 1);
    decision_t dec;
    while (nbits-- > d_frame_size - (s_k - 1)) {
        int k;
        dec.t = &d[nbits * s_decision_t_size];
        k = (dec.w[(endstate >> s_ADDSHIFT) / 32] >> ((endstate >> s_ADDSHIFT) % 32)) & 1;

        endstate = (endstate >> 1) | (k << (s_k - 2 + s_ADDSHIFT));
        data[((nbits + dif) % d_frame_size)] = k;

        retval = endstate;
    }
    nbits += 1;

    while (nbits-- != 0) {
        int k;

        dec.t = &d[nbits * s_decision_t_size];

        k = (dec.w[(endstate >> s_ADDSHIFT) / 32] >> ((endstate >> s_ADDSHIFT) % 32)) & 1;

        endstate = (endstate >> 1) | (k << (s_k - 2 + s_ADDSHIFT));
        data[((nbits + dif) % d_frame_size)] = k;
    }

    return retval >> s_ADDSHIFT;
}

bool cc_decoder_impl::set_frame_size(unsigned int frame_size)
{
    bool ret = true;
    if (frame_size > d_max_frame_size) {
        d_logger->info("cc_decoder: tried to set frame to {:d}; max possible is {:d}",
                       frame_size,
                       d_max_frame_size);
        frame_size = d_max_frame_size;
        ret = false;
    }

    d_frame_size = frame_size;

    switch (d_mode) {
    case (CC_TAILBITING):
        d_veclen = d_frame_size + (6 * (s_k - 1));
        if (d_veclen * s_rate > d_managed_in.size()) {
            throw std::runtime_error(
                "cc_decoder: attempt to resize beyond d_managed_in buffer size!");
        }
        break;

    case (CC_TRUNCATED):
        d_veclen = d_frame_size;
        break;

    case (CC_STREAMING):
        d_veclen = d_frame_size + s_k - 1;
        break;

    case (CC_TERMINATED):
        // If the input is being padded out to a byte, we know the
        // real frame size is without the padding.
        d_frame_size -= d_padding * s_rate;
        d_veclen = d_frame_size + s_k - 1;
        break;

    default:
        throw std::runtime_error("cc_decoder: mode not recognized");
    }

    return ret;
}

double cc_decoder_impl::rate() { return 1.0 / static_cast<double>(s_rate); }

void cc_decoder_impl::generic_work(const void* inbuffer, void* outbuffer)
{
    const unsigned char* in = (const unsigned char*)inbuffer;
    unsigned char* out = (unsigned char*)outbuffer;

    switch (d_mode) {

    case (CC_TAILBITING):
        memcpy(d_managed_in.data(), in, d_frame_size * s_rate * sizeof(unsigned char));
        memcpy(d_managed_in.data() + d_frame_size * s_rate * sizeof(unsigned char),
               in,
               (d_veclen - d_frame_size) * s_rate * sizeof(unsigned char));
        update_viterbi_blk(d_managed_in.data(), d_veclen);
        d_end_state_chaining = find_endstate();
        chainback_viterbi(&out[0], d_frame_size, *d_end_state, d_veclen - d_frame_size);
        init_viterbi_unbiased(&d_vp);
        break;


    case (CC_TRUNCATED):
        update_viterbi_blk((const unsigned char*)(&in[0]), d_veclen);
        d_end_state_chaining = find_endstate();
        for (unsigned int i = 0; i < s_k - 1; ++i) {
            out[d_veclen - 1 - i] = ((*d_end_state) >> i) & 1;
        }
        d_start_state_chaining =
            chainback_viterbi(&out[0], d_frame_size - (s_k - 1), *d_end_state, s_k - 1);
        init_viterbi(&d_vp, *d_start_state);
        break;

    case (CC_STREAMING):
    case (CC_TERMINATED):
        update_viterbi_blk((const unsigned char*)(&in[0]), d_veclen);
        d_end_state_chaining = find_endstate();
        d_start_state_chaining = chainback_viterbi(
            &out[0], d_frame_size, *d_end_state, d_veclen - d_frame_size);

        init_viterbi(&d_vp, *d_start_state);
        break;

    default:
        throw std::runtime_error("cc_decoder: mode not recognized");
    }
}

} /* namespace code */
} /* namespace fec */
} /* namespace gr */

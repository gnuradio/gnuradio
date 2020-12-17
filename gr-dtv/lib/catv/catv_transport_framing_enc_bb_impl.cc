/* -*- c++ -*- */
/*
 * Copyright 2016 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "catv_transport_framing_enc_bb_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace dtv {

catv_transport_framing_enc_bb::sptr catv_transport_framing_enc_bb::make()
{
    return gnuradio::make_block_sptr<catv_transport_framing_enc_bb_impl>();
}

/*
 * The private constructor
 */
catv_transport_framing_enc_bb_impl::catv_transport_framing_enc_bb_impl()
    : gr::sync_block("catv_transport_framing_enc_bb",
                     gr::io_signature::make(1, 1, sizeof(unsigned char)),
                     gr::io_signature::make(1, 1, sizeof(unsigned char)))
{
    set_output_multiple(188);
}

/*
 * Our virtual destructor.
 */
catv_transport_framing_enc_bb_impl::~catv_transport_framing_enc_bb_impl() {}

unsigned char catv_transport_framing_enc_bb_impl::compute_sum(const unsigned char* bytes)
{
    unsigned char i, bit, out, out1, out2, out3;

    unsigned char tapsG = 0xB1; // 10110001
    unsigned char tapsB = 0x45; //  1000101

    unsigned char register1 = 0;
    unsigned char register2 = 0;
    unsigned char register3 = 0;

    unsigned char result = 0x67;

    unsigned char first7[] = { 0, 0, 0, 0, 0, 0, 0, 0 };

    for (i = 0; i < 8; i++) {
        bit = (bytes[0] >> (7 - i)) & 1;
        out = (register1 & 1) ^ bit;
        if (i < 7) {
            first7[i + 1] = out;
        }
        register1 >>= 1;
        if (out == 1) {
            register1 ^= tapsG;
        }
    }

    for (i = 1; i < 187; i++) {
        register1 = crctable[((register1) ^ BitReverseTable[bytes[i]]) & 0xff];
    }

    for (i = 0; i < 8; i++) {
        out1 = register1 & 1;
        register1 >>= 1;
        if (out1 == 1) {
            register1 ^= tapsG;
        }

        out2 = (register2 & 1) ^ first7[i];
        register2 >>= 1;
        if (first7[i] == 1) {
            register2 ^= tapsB;
        }

        out3 = (register3 & 1) ^ out1 ^ out2;
        register3 >>= 1;
        if ((out1 ^ out2) == 1) {
            register3 ^= tapsG;
        }

        result ^= (out3 << (7 - i));
    }

    return result;
}

int catv_transport_framing_enc_bb_impl::work(int noutput_items,
                                             gr_vector_const_void_star& input_items,
                                             gr_vector_void_star& output_items)
{
    const unsigned char* in = (const unsigned char*)input_items[0];
    unsigned char* out = (unsigned char*)output_items[0];

    for (int i = 0; i < noutput_items; i += 188) {
        memcpy(out + i, in + i + 1, 187);
        out[i + 187] = compute_sum(in + i + 1);
    }

    // Tell runtime system how many output items we produced.
    return noutput_items;
}

} /* namespace dtv */
} /* namespace gr */

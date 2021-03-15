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

#include "atsc_rs_decoder_impl.h"
#include "gnuradio/dtv/atsc_consts.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace dtv {

static const int rs_init_symsize = 8;
static const int rs_init_gfpoly = 0x11d;
static const int rs_init_fcr = 0;  // first consecutive root
static const int rs_init_prim = 1; // primitive is 1 (alpha)
static const int rs_init_nroots = 20;

static const int N = (1 << rs_init_symsize) - 1; // 255

static const int amount_of_pad = N - ATSC_MPEG_RS_ENCODED_LENGTH; // 48

atsc_rs_decoder::sptr atsc_rs_decoder::make()
{
    return gnuradio::make_block_sptr<atsc_rs_decoder_impl>();
}

atsc_rs_decoder_impl::atsc_rs_decoder_impl()
    : gr::sync_block(
          "dtv_atsc_rs_decoder",
          io_signature::make2(
              2, 2, sizeof(uint8_t) * ATSC_MPEG_RS_ENCODED_LENGTH, sizeof(plinfo)),
          io_signature::make2(
              2, 2, sizeof(uint8_t) * ATSC_MPEG_PKT_LENGTH, sizeof(plinfo)))
{
    d_rs = init_rs_char(
        rs_init_symsize, rs_init_gfpoly, rs_init_fcr, rs_init_prim, rs_init_nroots);
    assert(d_rs != 0);
    d_nerrors_corrected_count = 0;
    d_bad_packet_count = 0;
    d_total_packets = 0;
}

int atsc_rs_decoder_impl::decode(uint8_t* out, const uint8_t* in)
{
    unsigned char tmp[N];
    int ncorrections;

    // assert((int)(amount_of_pad + sizeof(in.data)) == N);
    assert((int)(amount_of_pad + ATSC_MPEG_RS_ENCODED_LENGTH) == N);

    // add missing prefix zero padding to message
    memset(tmp, 0, amount_of_pad);
    memcpy(&tmp[amount_of_pad], in, ATSC_MPEG_RS_ENCODED_LENGTH);

    // correct message...
    ncorrections = decode_rs_char(d_rs, tmp, 0, 0);

    // copy corrected message to output, skipping prefix zero padding
    memcpy(out, &tmp[amount_of_pad], ATSC_MPEG_PKT_LENGTH);

    return ncorrections;
}

atsc_rs_decoder_impl::~atsc_rs_decoder_impl()
{
    if (d_rs)
        free_rs_char(d_rs);
    d_rs = 0;
}

int atsc_rs_decoder_impl::num_errors_corrected() const
{
    return d_nerrors_corrected_count;
}

int atsc_rs_decoder_impl::num_bad_packets() const { return d_bad_packet_count; }

int atsc_rs_decoder_impl::num_packets() const { return d_total_packets; }

int atsc_rs_decoder_impl::work(int noutput_items,
                               gr_vector_const_void_star& input_items,
                               gr_vector_void_star& output_items)
{
    auto in = static_cast<const uint8_t*>(input_items[0]);
    auto out = static_cast<uint8_t*>(output_items[0]);
    auto plin = static_cast<const plinfo*>(input_items[1]);
    auto plout = static_cast<plinfo*>(output_items[1]);

    for (int i = 0; i < noutput_items; i++) {
        assert(plin[i].regular_seg_p());

        plout[i] = plin[i]; // copy pipeline info...

        int nerrors_corrected =
            decode(&out[i * ATSC_MPEG_PKT_LENGTH], &in[i * ATSC_MPEG_RS_ENCODED_LENGTH]);
        plout[i].set_transport_error(nerrors_corrected == -1);
        if (nerrors_corrected == -1) {
            d_bad_packet_count++;
            d_nerrors_corrected_count += 10; // lower bound estimate; most this RS can fix
        } else {
            d_nerrors_corrected_count += nerrors_corrected;
        }

        d_total_packets++;
#if 0
        if (d_total_packets > 1000) {
          GR_LOG_INFO(d_logger, boost::format("Error rate: %1%\tPacket error rate: %2%") \
                       % ((float)d_nerrors_corrected_count/(ATSC_MPEG_PKT_LENGTH*d_total_packets))
                       % ((float)d_bad_packet_count/d_total_packets));
        }
#endif
    }

    return noutput_items;
}


void atsc_rs_decoder_impl::setup_rpc()
{
#ifdef GR_CTRLPORT
    add_rpc_variable(rpcbasic_sptr(new rpcbasic_register_get<atsc_rs_decoder, int>(
        alias(),
        "num_errors_corrected",
        &atsc_rs_decoder::num_errors_corrected,
        pmt::from_long(0),
        pmt::from_long(10000000),
        pmt::from_long(0),
        "",
        "Number of errors corrected",
        RPC_PRIVLVL_MIN,
        DISPTIME)));

    add_rpc_variable(rpcbasic_sptr(
        new rpcbasic_register_get<atsc_rs_decoder, int>(alias(),
                                                        "num_bad_packets",
                                                        &atsc_rs_decoder::num_bad_packets,
                                                        pmt::from_long(0),
                                                        pmt::from_long(10000000),
                                                        pmt::from_long(0),
                                                        "",
                                                        "Number of bad packets",
                                                        RPC_PRIVLVL_MIN,
                                                        DISPTIME)));

    add_rpc_variable(rpcbasic_sptr(
        new rpcbasic_register_get<atsc_rs_decoder, int>(alias(),
                                                        "num_packets",
                                                        &atsc_rs_decoder::num_packets,
                                                        pmt::from_long(0),
                                                        pmt::from_long(10000000),
                                                        pmt::from_long(0),
                                                        "",
                                                        "Number of packets",
                                                        RPC_PRIVLVL_MIN,
                                                        DISPTIME)));
#endif /* GR_CTRLPORT */
}

} /* namespace dtv */
} /* namespace gr */

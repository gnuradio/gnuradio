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

#include "async_decoder_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>
#include <cstdio>

namespace gr {
namespace fec {

async_decoder::sptr
async_decoder::make(generic_decoder::sptr my_decoder, bool packed, bool rev_pack, int mtu)
{
    return gnuradio::make_block_sptr<async_decoder_impl>(
        my_decoder, packed, rev_pack, mtu);
}

async_decoder_impl::async_decoder_impl(generic_decoder::sptr my_decoder,
                                       bool packed,
                                       bool rev_pack,
                                       int mtu)
    : block("async_decoder", io_signature::make(0, 0, 0), io_signature::make(0, 0, 0)),
      d_pack(8)
{
    d_in_port = pmt::mp("in");
    d_out_port = pmt::mp("out");

    d_decoder = my_decoder;

    if (d_decoder->get_history() > 0) {
        throw std::runtime_error("async_decoder deploment does not support decoders with "
                                 "history requirements.");
    }

    d_packed = packed;
    d_rev_pack = rev_pack;
    d_mtu = mtu;

    message_port_register_in(d_in_port);
    message_port_register_out(d_out_port);

    if (d_packed) {
        set_msg_handler(d_in_port, [this](pmt::pmt_t msg) { this->decode_packed(msg); });
    } else {
        set_msg_handler(d_in_port,
                        [this](pmt::pmt_t msg) { this->decode_unpacked(msg); });
    }

    // The maximum frame size is set by the initial frame size of the decoder.
    d_max_bits_in = d_mtu * 8 * 1.0 / d_decoder->rate();
    d_tmp_f32.resize(d_max_bits_in);

    if (strncmp(d_decoder->get_input_conversion(), "uchar", 5) == 0) {
        d_tmp_u8.resize(d_max_bits_in);
    }

    if (d_packed) {
        int max_bits_out = d_mtu * 8;
        d_bits_out.resize(max_bits_out);
    }
}

async_decoder_impl::~async_decoder_impl() {}

void async_decoder_impl::decode_unpacked(pmt::pmt_t msg)
{
    // extract input pdu
    pmt::pmt_t meta(pmt::car(msg));
    pmt::pmt_t bits(pmt::cdr(msg));

    // Watch out for this diff. It might be over-specializing to the
    // CC decoder in terminated mode that has an extra rate(K-1)
    // bits added on to the transmitted frame.
    int diff =
        d_decoder->rate() * d_decoder->get_input_size() - d_decoder->get_output_size();

    size_t nbits_in = pmt::length(bits);
    size_t nbits_out = 0;
    size_t nblocks = 1;
    bool variable_frame_size =
        d_decoder->set_frame_size(nbits_in * d_decoder->rate() - diff);

    // Check here if the frame size is larger than what we've
    // allocated for in the constructor.
    if (variable_frame_size && (nbits_in > d_max_bits_in)) {
        throw std::runtime_error(
            "async_decoder: Received frame larger than max frame size.");
    }

    // set up nbits_out
    if (variable_frame_size) {
        nbits_out = nbits_in * d_decoder->rate() - diff;
    } else {
        nblocks = nbits_in / d_decoder->get_input_size();
        nbits_out = nblocks * d_decoder->get_output_size();
        if (nblocks * d_decoder->get_input_size() != nbits_in) {
            throw std::runtime_error("bad block multiple in!");
        }
    }

    size_t o0(0);
    const float* f32in = pmt::f32vector_elements(bits, o0);
    pmt::pmt_t outvec(pmt::make_u8vector(nbits_out, 0x00));
    uint8_t* u8out = pmt::u8vector_writable_elements(outvec, o0);

    if (strncmp(d_decoder->get_input_conversion(), "uchar", 5) == 0) {
        volk_32f_s32f_multiply_32f(d_tmp_f32.data(), f32in, 48.0f, nbits_in);
    } else {
        memcpy(d_tmp_f32.data(), f32in, nbits_in * sizeof(float));
    }

    if (d_decoder->get_shift() != 0) {
        for (size_t n = 0; n < nbits_in; n++)
            d_tmp_f32[n] += d_decoder->get_shift();
    }

    if (strncmp(d_decoder->get_input_conversion(), "uchar", 5) == 0) {
        // volk_32f_s32f_convert_8i(d_tmp_u8, d_tmp_f32, 1, nbits_in);
        for (size_t n = 0; n < nbits_in; n++)
            d_tmp_u8[n] = static_cast<uint8_t>(d_tmp_f32[n]);

        d_decoder->generic_work((void*)d_tmp_u8.data(), (void*)u8out);
    } else {
        for (size_t i = 0; i < nblocks; i++) {
            d_decoder->generic_work((void*)&d_tmp_f32[i * d_decoder->get_input_size()],
                                    (void*)&u8out[i * d_decoder->get_output_size()]);
        }
    }

    static const pmt::pmt_t iterations_key = pmt::mp("iterations");
    meta = pmt::dict_add(meta, iterations_key, pmt::mp(d_decoder->get_iterations()));
    message_port_pub(d_out_port, pmt::cons(meta, outvec));
}

void async_decoder_impl::decode_packed(pmt::pmt_t msg)
{
    // extract input pdu
    pmt::pmt_t meta(pmt::car(msg));
    pmt::pmt_t bits(pmt::cdr(msg));

    size_t o0 = 0;
    size_t nbits_in = pmt::length(bits);
    int nbits_out = nbits_in * d_decoder->rate();
    int nbytes_out = nbits_out / 8;

    // Check here if the frame size is larger than what we've
    // allocated for in the constructor.
    if (nbits_in > d_max_bits_in) {
        throw std::runtime_error(
            "async_decoder: Received frame larger than max frame size.");
    }

    d_decoder->set_frame_size(nbits_out);

    pmt::pmt_t outvec(pmt::make_u8vector(nbytes_out, 0x00));
    uint8_t* bytes_out = pmt::u8vector_writable_elements(outvec, o0);
    const float* f32in = pmt::f32vector_elements(bits, o0);

    if (strncmp(d_decoder->get_input_conversion(), "uchar", 5) == 0) {
        volk_32f_s32f_multiply_32f(d_tmp_f32.data(), f32in, 48.0f, nbits_in);
    } else {
        memcpy(d_tmp_f32.data(), f32in, nbits_in * sizeof(float));
    }

    if (d_decoder->get_shift() != 0) {
        for (size_t n = 0; n < nbits_in; n++)
            d_tmp_f32[n] += d_decoder->get_shift();
    }

    if (strncmp(d_decoder->get_input_conversion(), "uchar", 5) == 0) {
        // volk_32f_s32f_convert_8i(d_tmp_u8, d_tmp_f32, 1.0, nbits_in);
        for (size_t n = 0; n < nbits_in; n++)
            d_tmp_u8[n] = static_cast<uint8_t>(d_tmp_f32[n]);

        d_decoder->generic_work((void*)d_tmp_u8.data(), (void*)d_bits_out.data());
    } else {
        d_decoder->generic_work((void*)d_tmp_f32.data(), (void*)d_bits_out.data());
    }

    if (d_rev_pack)
        d_pack.pack_rev(bytes_out, d_bits_out.data(), nbytes_out);
    else
        d_pack.pack(bytes_out, d_bits_out.data(), nbytes_out);

    message_port_pub(d_out_port, pmt::cons(meta, outvec));
}

int async_decoder_impl::general_work(int noutput_items,
                                     gr_vector_int& ninput_items,
                                     gr_vector_const_void_star& input_items,
                                     gr_vector_void_star& output_items)
{
    return noutput_items;
}

} /* namespace fec */
} /* namespace gr */

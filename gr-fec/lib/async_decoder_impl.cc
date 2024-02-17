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
#include <cmath>
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

    set_msg_handler(d_in_port, [this](const pmt::pmt_t& msg) { this->decode(msg); });

    // The maximum frame size is set by the initial frame size of the decoder.
    d_max_bits_in = std::lround(d_mtu * 8 * 1.0 / d_decoder->rate());
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

// The volk_32f_s32f_x2_convert_8u kernel is only available since Volk 3.1.
// In earlier versions we use this ad-hoc function.
#if !(VOLK_VERSION >= 030100)
inline void async_decoder_impl::convert_32f_to_8u(uint8_t* output_vector,
                                                  const float* input_vector,
                                                  const float scale,
                                                  const float bias,
                                                  unsigned int num_points)
{
    volk_32f_s32f_multiply_32f(d_tmp_f32.data(), input_vector, scale, num_points);
    if (bias != 0.0) {
        for (size_t n = 0; n < num_points; n++) {
            d_tmp_f32[n] += bias;
        }
    }
    for (size_t n = 0; n < num_points; n++) {
        output_vector[n] = std::clamp<float>(d_tmp_f32[n], 0.0f, UINT8_MAX);
    }
}
#endif

void async_decoder_impl::decode(const pmt::pmt_t& msg)
{
    // extract input pdu
    pmt::pmt_t meta(pmt::car(msg));
    pmt::pmt_t bits(pmt::cdr(msg));

    // Watch out for this diff. It might be over-specializing to the
    // CC decoder in terminated mode that has an extra rate(K-1)
    // bits added on to the transmitted frame.
    int diff = std::lround(d_decoder->rate() * d_decoder->get_input_size()) -
               d_decoder->get_output_size();

    size_t nbits_in = pmt::length(bits);
    size_t nbits_out = 0;
    size_t nblocks = 1;
    bool variable_frame_size =
        d_decoder->set_frame_size(std::lround(nbits_in * d_decoder->rate()) - diff);

    // Check here if the frame size is larger than what we've
    // allocated for in the constructor.
    if (variable_frame_size && (nbits_in > d_max_bits_in)) {
        throw std::runtime_error(
            "async_decoder: Received frame larger than max frame size.");
    }

    // set up nbits_out
    if (variable_frame_size) {
        nbits_out = std::lround(nbits_in * d_decoder->rate()) - diff;
    } else {
        nblocks = nbits_in / d_decoder->get_input_size();
        nbits_out = nblocks * d_decoder->get_output_size();
        if (nblocks * d_decoder->get_input_size() != nbits_in) {
            throw std::runtime_error("bad block multiple in!");
        }
    }

    size_t vector_len = 0; // used only to get the length of PMT vectors
    const float* f32in = pmt::f32vector_elements(bits, vector_len);
    const size_t nitems_out = d_packed ? nbits_out / 8 : nbits_out;
    auto outvec = pmt::make_u8vector(nitems_out, 0x00);
    uint8_t* u8out = pmt::u8vector_writable_elements(outvec, vector_len);
    uint8_t* decoder_out = d_packed ? d_bits_out.data() : u8out;
    const float shift = d_decoder->get_shift();

    if (std::string_view(d_decoder->get_input_conversion()) == "uchar") {
#if VOLK_VERSION >= 030100
        volk_32f_s32f_x2_convert_8u(d_tmp_u8.data(), f32in, 48.0f, shift, nbits_in);
#else
        convert_32f_to_8u(d_tmp_u8.data(), f32in, 48.0f, shift, nbits_in);
#endif

        for (size_t i = 0; i < nblocks; i++) {
            d_decoder->generic_work(
                (void*)&d_tmp_u8[i * d_decoder->get_input_size()],
                (void*)&decoder_out[i * d_decoder->get_output_size()]);
        }
    } else {
        if (shift != 0.0) {
            volk_32f_s32f_add_32f(d_tmp_f32.data(), f32in, shift, nbits_in);
        } else {
            memcpy(d_tmp_f32.data(), f32in, nbits_in * sizeof(float));
        }

        for (size_t i = 0; i < nblocks; i++) {
            d_decoder->generic_work(
                (void*)&d_tmp_f32[i * d_decoder->get_input_size()],
                (void*)&decoder_out[i * d_decoder->get_output_size()]);
        }
    }

    // Note: when nblocks > 1, this only gets the iterations for the last
    // decoder block.
    meta = pmt::dict_add(meta, ITERATIONS_KEY, pmt::mp(d_decoder->get_iterations()));

    if (d_packed) {
        if (d_rev_pack) {
            d_pack.pack_rev(u8out, decoder_out, nitems_out);
        } else {
            d_pack.pack(u8out, decoder_out, nitems_out);
        }
    }

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

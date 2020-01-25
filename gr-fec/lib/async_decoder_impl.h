/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FEC_ASYNC_DECODER_IMPL_H
#define INCLUDED_FEC_ASYNC_DECODER_IMPL_H

#include <gnuradio/blocks/pack_k_bits.h>
#include <gnuradio/fec/async_decoder.h>

namespace gr {
namespace fec {

class FEC_API async_decoder_impl : public async_decoder
{
private:
    generic_decoder::sptr d_decoder;

    pmt::pmt_t d_in_port;
    pmt::pmt_t d_out_port;

    blocks::kernel::pack_k_bits* d_pack;

    bool d_packed;
    bool d_rev_pack;
    int d_mtu;

    size_t d_max_bits_in;
    float* d_tmp_f32;
    int8_t* d_tmp_u8;
    uint8_t* d_bits_out;

    void decode_packed(pmt::pmt_t msg);
    void decode_unpacked(pmt::pmt_t msg);

public:
    async_decoder_impl(generic_decoder::sptr my_decoder,
                       bool packed = false,
                       bool rev_pack = true,
                       int mtu = 1500);
    ~async_decoder_impl();

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
};

} /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_ASYNC_DECODER_IMPL_H */

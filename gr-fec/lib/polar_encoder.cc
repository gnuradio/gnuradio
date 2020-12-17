/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/fec/polar_encoder.h>
#include <gnuradio/io_signature.h>
#include <volk/volk.h>
#include <cmath>
#include <stdexcept>

#include <gnuradio/blocks/pack_k_bits.h>
#include <gnuradio/blocks/unpack_k_bits.h>

namespace gr {
namespace fec {
namespace code {

volk::vector<unsigned char> polar_encoder::make_prototype() const
{
    volk::vector<unsigned char> proto(block_size() >> 3);

    for (unsigned int i = 0; i < d_frozen_bit_positions.size(); i++) {
        int rev_pos = (int)bit_reverse((long)d_frozen_bit_positions.at(i), block_power());
        unsigned char frozen_bit = (unsigned char)d_frozen_bit_values.at(i);
        insert_unpacked_bit_into_packed_array_at_position(
            proto.data(), frozen_bit, rev_pos);
    }
    return proto;
}

generic_encoder::sptr polar_encoder::make(int block_size,
                                          int num_info_bits,
                                          std::vector<int> frozen_bit_positions,
                                          std::vector<uint8_t> frozen_bit_values,
                                          bool is_packed)
{
    return generic_encoder::sptr(new polar_encoder(
        block_size, num_info_bits, frozen_bit_positions, frozen_bit_values, is_packed));
}

polar_encoder::polar_encoder(int block_size,
                             int num_info_bits,
                             std::vector<int>& frozen_bit_positions,
                             std::vector<uint8_t>& frozen_bit_values,
                             bool is_packed)
    : polar_common(block_size, num_info_bits, frozen_bit_positions, frozen_bit_values),
      d_is_packed(is_packed),
      d_frozen_bit_prototype(make_prototype())
{
}

polar_encoder::~polar_encoder() {}

void polar_encoder::generic_work(void* in_buffer, void* out_buffer)
{
    const unsigned char* in = (const unsigned char*)in_buffer;
    unsigned char* out = (unsigned char*)out_buffer;

    if (d_is_packed) {
        insert_packed_frozen_bits_and_reverse(out, in);
        encode_vector_packed(out);
    } else {
        volk_encode(out, in);
    }
}

void polar_encoder::encode_vector_packed(unsigned char* target) const
{
    encode_vector_packed_subbyte(target);
    encode_vector_packed_interbyte(target);
}

void polar_encoder::encode_vector_packed_subbyte(unsigned char* target) const
{
    int num_bytes_per_block = block_size() >> 3;
    while (num_bytes_per_block) {
        encode_packed_byte(target);
        ++target;
        --num_bytes_per_block;
    }
}

void polar_encoder::encode_packed_byte(unsigned char* target) const
{
    // this method only produces correct results if block_size > 4.
    // this is assumed to be the case.
    *target ^= 0xaa & (*target << 1);
    *target ^= 0xcc & (*target << 2);
    *target ^= *target << 4;
}

void polar_encoder::encode_vector_packed_interbyte(unsigned char* target) const
{
    int branch_byte_size = 1;
    unsigned char* pos;
    int n_branches = block_size() >> 4;
    int byte = 0;
    for (int stage = 3; stage < block_power(); ++stage) {
        pos = target;

        for (int branch = 0; branch < n_branches; ++branch) {

            byte = 0;
            while (byte < branch_byte_size) {
                *pos ^= *(pos + branch_byte_size);
                ++pos;
                ++byte;
            }

            pos += branch_byte_size;
        }

        n_branches >>= 1;
        branch_byte_size <<= 1;
    }
}

void polar_encoder::insert_packed_frozen_bits_and_reverse(
    unsigned char* target, const unsigned char* input) const
{
    std::copy(
        std::begin(d_frozen_bit_prototype), std::end(d_frozen_bit_prototype), target);
    const int* info_bit_reversed_positions_ptr = &d_info_bit_positions_reversed[0];
    int bit_num = 0;
    unsigned char byte = *input;
    int bit_pos;
    while (bit_num < num_info_bits()) {
        bit_pos = *info_bit_reversed_positions_ptr++;
        insert_packet_bit_into_packed_array_at_position(
            target, byte, bit_pos, bit_num % 8);
        ++bit_num;
        if (bit_num % 8 == 0) {
            ++input;
            byte = *input;
        }
    }
}

void polar_encoder::insert_unpacked_bit_into_packed_array_at_position(
    unsigned char* target, const unsigned char bit, const int pos) const
{
    int byte_pos = pos >> 3;
    int bit_pos = pos & 0x7;
    *(target + byte_pos) ^= bit << (7 - bit_pos);
}

void polar_encoder::insert_packet_bit_into_packed_array_at_position(
    unsigned char* target,
    const unsigned char bit,
    const int target_pos,
    const int bit_pos) const
{
    insert_unpacked_bit_into_packed_array_at_position(
        target, (bit >> (7 - bit_pos)) & 0x01, target_pos);
}

} /* namespace code */
} /* namespace fec */
} /* namespace gr */

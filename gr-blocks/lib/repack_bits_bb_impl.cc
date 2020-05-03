/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "repack_bits_bb_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace blocks {

repack_bits_bb::sptr repack_bits_bb::make(int k,
                                          int l,
                                          const std::string& len_tag_key,
                                          bool align_output,
                                          endianness_t endianness)
{
    return gnuradio::make_block_sptr<repack_bits_bb_impl>(
        k, l, len_tag_key, align_output, endianness);
}

repack_bits_bb_impl::repack_bits_bb_impl(int k,
                                         int l,
                                         const std::string& len_tag_key,
                                         bool align_output,
                                         endianness_t endianness)
    : tagged_stream_block("repack_bits_bb",
                          io_signature::make(1, 1, sizeof(char)),
                          io_signature::make(1, 1, sizeof(char)),
                          len_tag_key),
      d_k(k),
      d_l(l),
      d_packet_mode(!len_tag_key.empty()),
      d_in_index(0),
      d_out_index(0),
      d_align_output(align_output),
      d_endianness(endianness)
{
    if (d_k > 8 || d_k < 1 || d_l > 8 || d_l < 1) {
        throw std::invalid_argument("k and l must be in [1, 8]");
    }

    set_relative_rate((uint64_t)d_k, (uint64_t)d_l);
}

void repack_bits_bb_impl::set_k_and_l(int k, int l)
{
    gr::thread::scoped_lock guard(d_setlock);
    d_k = k;
    d_l = l;
    set_relative_rate((uint64_t)d_k, (uint64_t)d_l);
}

repack_bits_bb_impl::~repack_bits_bb_impl() {}

int repack_bits_bb_impl::calculate_output_stream_length(const gr_vector_int& ninput_items)
{
    int n_out_bytes_required = (ninput_items[0] * d_k) / d_l;
    if ((ninput_items[0] * d_k) % d_l &&
        (!d_packet_mode || (d_packet_mode && !d_align_output))) {
        n_out_bytes_required++;
    }

    return n_out_bytes_required;
}

int repack_bits_bb_impl::work(int noutput_items,
                              gr_vector_int& ninput_items,
                              gr_vector_const_void_star& input_items,
                              gr_vector_void_star& output_items)
{
    gr::thread::scoped_lock guard(d_setlock);
    const unsigned char* in = (const unsigned char*)input_items[0];
    unsigned char* out = (unsigned char*)output_items[0];
    int bytes_to_write = noutput_items;

    if (d_packet_mode) { // noutput_items could be larger than necessary
        int bytes_to_read = ninput_items[0];
        bytes_to_write = bytes_to_read * d_k / d_l;
        if (!d_align_output && (((bytes_to_read * d_k) % d_l) != 0)) {
            bytes_to_write++;
        }
    }

    int n_read = 0;
    int n_written = 0;
    switch (d_endianness) {
    case GR_LSB_FIRST:
        while (n_written < bytes_to_write && n_read < ninput_items[0]) {
            if (d_out_index == 0) { // Starting a fresh byte
                out[n_written] = 0;
            }
            out[n_written] |= ((in[n_read] >> d_in_index) & 0x01) << d_out_index;

            d_in_index = (d_in_index + 1) % d_k;
            d_out_index = (d_out_index + 1) % d_l;
            if (d_in_index == 0) {
                n_read++;
                d_in_index = 0;
            }
            if (d_out_index == 0) {
                n_written++;
                d_out_index = 0;
            }
        }

        if (d_packet_mode) {
            if (d_out_index) {
                n_written++;
                d_out_index = 0;
            }
        } else {
            consume_each(n_read);
        }
        break;


    case GR_MSB_FIRST:
        while (n_written < bytes_to_write && n_read < ninput_items[0]) {
            if (d_out_index == 0) { // Starting a fresh byte
                out[n_written] = 0;
            }
            out[n_written] |= ((in[n_read] >> (d_k - 1 - d_in_index)) & 0x01)
                              << (d_l - 1 - d_out_index);

            d_in_index = (d_in_index + 1) % d_k;
            d_out_index = (d_out_index + 1) % d_l;
            if (d_in_index == 0) {
                n_read++;
                d_in_index = 0;
            }
            if (d_out_index == 0) {
                n_written++;
                d_out_index = 0;
            }
        }

        if (d_packet_mode) {
            if (d_out_index) {
                n_written++;
                d_out_index = 0;
            }
        } else {
            consume_each(n_read);
        }
        break;

    default:
        throw std::runtime_error("repack_bits_bb: unrecognized endianness value.");
    }

    return n_written;
}

} /* namespace blocks */
} /* namespace gr */

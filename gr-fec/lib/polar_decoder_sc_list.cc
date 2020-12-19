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

#include "scl_list.h"
#include <gnuradio/fec/polar_decoder_sc_list.h>
#include <gnuradio/io_signature.h>
#include <volk/volk.h>
#include <algorithm>
#include <cmath>
#include <memory>

namespace gr {
namespace fec {
namespace code {

generic_decoder::sptr polar_decoder_sc_list::make(int max_list_size,
                                                  int block_size,
                                                  int num_info_bits,
                                                  std::vector<int> frozen_bit_positions,
                                                  std::vector<uint8_t> frozen_bit_values)
{
    return generic_decoder::sptr(new polar_decoder_sc_list(max_list_size,
                                                           block_size,
                                                           num_info_bits,
                                                           frozen_bit_positions,
                                                           frozen_bit_values));
}

polar_decoder_sc_list::polar_decoder_sc_list(int max_list_size,
                                             int block_size,
                                             int num_info_bits,
                                             std::vector<int> frozen_bit_positions,
                                             std::vector<uint8_t> frozen_bit_values)
    : polar_decoder_common(
          block_size, num_info_bits, frozen_bit_positions, frozen_bit_values),
      d_scl(std::make_unique<polar::scl_list>(max_list_size, block_size, block_power()))
{
}

polar_decoder_sc_list::~polar_decoder_sc_list() {}

void polar_decoder_sc_list::generic_work(void* in_buffer, void* out_buffer)
{
    const float* in = (const float*)in_buffer;
    unsigned char* out = (unsigned char*)out_buffer;

    initialize_list(in);
    const unsigned char* temp = decode_list();
    extract_info_bits(out, temp);
}

void polar_decoder_sc_list::initialize_list(const float* in_buf)
{
    polar::path* init_path = d_scl->initial_path();
    initialize_decoder(init_path->u_vec, init_path->llr_vec, in_buf);
}

const unsigned char* polar_decoder_sc_list::decode_list()
{
    for (int u_num = 0; u_num < block_size(); u_num++) {
        decode_bit(u_num);
    }
    return d_scl->optimal_path()->u_vec;
}

void polar_decoder_sc_list::decode_bit(const int u_num)
{
    calculate_llrs_for_list(u_num);
    set_bit_in_list(u_num);
}

void polar_decoder_sc_list::calculate_llrs_for_list(const int u_num)
{
    for (unsigned int i = 0; i < d_scl->active_size(); i++) {
        polar::path* current_path = d_scl->next_active_path();
        butterfly(current_path->llr_vec, current_path->u_vec, 0, u_num, u_num);
    }
}

void polar_decoder_sc_list::set_bit_in_list(const int u_num)
{
    // 1. if frozen bit, update with known value
    if (is_frozen_bit(u_num)) {
        const unsigned char frozen_bit = next_frozen_bit();
        d_scl->set_frozen_bit(frozen_bit, u_num);
    }
    // 2. info bit
    else {
        d_scl->set_info_bit(u_num);
    }
}

} /* namespace code */
} /* namespace fec */
} /* namespace gr */

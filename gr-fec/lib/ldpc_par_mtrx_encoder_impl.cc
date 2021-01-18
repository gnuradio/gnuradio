/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "ldpc_par_mtrx_encoder_impl.h"
#include <volk/volk.h>
#include <boost/assign/list_of.hpp>
#include <algorithm> // for std::reverse
#include <cmath>
#include <cstdio>
#include <cstring> // for memcpy
#include <sstream>
#include <vector>

namespace gr {
namespace fec {
namespace code {

generic_encoder::sptr ldpc_par_mtrx_encoder::make(std::string alist_file,
                                                  unsigned int gap)
{
    code::ldpc_H_matrix::sptr H_obj = code::ldpc_H_matrix::make(alist_file, gap);
    return make_H(H_obj);
}

generic_encoder::sptr ldpc_par_mtrx_encoder::make_H(const code::ldpc_H_matrix::sptr H_obj)
{
    return generic_encoder::sptr(new ldpc_par_mtrx_encoder_impl(H_obj));
}

ldpc_par_mtrx_encoder_impl::ldpc_par_mtrx_encoder_impl(
    const code::ldpc_H_matrix::sptr H_obj)
    : generic_encoder("ldpc_par_mtrx_encoder")
{
    // LDPC parity check matrix to use for encoding
    d_H = H_obj;

    d_rate = static_cast<double>(d_H->n()) / static_cast<double>(d_H->k());

    // Set frame size to k, the # of bits in the information word
    // All buffers and settings will be based on this value.
    set_frame_size(d_H->k());
}

ldpc_par_mtrx_encoder_impl::~ldpc_par_mtrx_encoder_impl() {}

int ldpc_par_mtrx_encoder_impl::get_output_size()
{
    // return outputSize;
    return d_output_size;
}

int ldpc_par_mtrx_encoder_impl::get_input_size()
{
    // return inputSize;
    return d_frame_size;
}

bool ldpc_par_mtrx_encoder_impl::set_frame_size(unsigned int frame_size)
{
    bool ret = true;

    if (frame_size % d_H->k() != 0) {
        GR_LOG_ERROR(d_logger,
                     boost::format("Frame size (%1% bits) must be a "
                                   "multiple of the information word "
                                   "size of the LDPC matrix (%2%).") %
                         frame_size % (d_H->k()));
        throw std::runtime_error("ldpc_par_mtrx_encoder: cannot use frame size.");
    }

    d_frame_size = frame_size;

    d_output_size = static_cast<int>(d_rate * d_frame_size);

    return ret;
}

double ldpc_par_mtrx_encoder_impl::rate() { return d_rate; }

void ldpc_par_mtrx_encoder_impl::generic_work(void* inbuffer, void* outbuffer)
{
    // Populate the information word
    const unsigned char* in = (const unsigned char*)inbuffer;
    unsigned char* out = (unsigned char*)outbuffer;

    int j = 0;
    for (int i = 0; i < get_input_size(); i += d_H->k()) {
        d_H->encode(&out[j], &in[i]);
        j += d_H->n();
    }
}

} /* namespace code */
} /* namespace fec */
} /* namespace gr */

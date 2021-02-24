/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "ldpc_encoder_impl.h"
#include <volk/volk.h>
#include <boost/assign/list_of.hpp>
#include <algorithm> // for std::reverse
#include <cstdio>
#include <cstring> // for memcpy
#include <filesystem>
#include <sstream>

namespace gr {
namespace fec {

generic_encoder::sptr ldpc_encoder::make(std::string alist_file)
{
    return generic_encoder::sptr(new ldpc_encoder_impl(alist_file));
}

ldpc_encoder_impl::ldpc_encoder_impl(std::string alist_file)
{
    if (!std::filesystem::exists(alist_file)) {
        throw std::runtime_error("Bad AList file name!");
    }

    d_list.read(alist_file.c_str());
    d_code.set_alist(d_list);
    inputSize = d_code.dimension();
    outputSize = d_code.get_N();

    // printf("ENCODER: inputSize = %d, outputSize = %d\n",inputSize, outputSize);
}

int ldpc_encoder_impl::get_output_size() { return outputSize; }

int ldpc_encoder_impl::get_input_size() { return inputSize; }

void ldpc_encoder_impl::generic_work(void* inBuffer, void* outBuffer)
{
    const unsigned char* in = (const unsigned char*)inBuffer;
    unsigned char* out = (unsigned char*)outBuffer;
    std::vector<uint8_t> inbuf(inputSize);
    memcpy(&inbuf[0], in, inputSize);
    std::vector<uint8_t> coded(d_code.encode(inbuf));
    memcpy(&out[0], &coded[0], coded.size());
}

ldpc_encoder_impl::~ldpc_encoder_impl() {}

} // namespace fec
} // namespace gr

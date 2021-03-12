/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <gnuradio/fec/decoder.h>
#include <gnuradio/fec/ldpc_decoder.h>
#include <gnuradio/fec/maxstar.h>
#include <volk/volk.h>
#include <boost/assign/list_of.hpp>
#include <algorithm> // for std::reverse
#include <cmath>
#include <cstdio>
#include <cstring> // for memcpy
#include <filesystem>
#include <sstream>
#include <vector>


namespace gr {
namespace fec {

generic_decoder::sptr
ldpc_decoder::make(std::string alist_file, float sigma, int max_iterations)
{
    return generic_decoder::sptr(new ldpc_decoder(alist_file, sigma, max_iterations));
}

ldpc_decoder::ldpc_decoder(std::string alist_file, float sigma, int max_iterations)
    : generic_decoder("ldpc_decoder")
{
    if (!std::filesystem::exists(alist_file))
        throw std::runtime_error("Bad AList file name!");

    d_list.read(alist_file.c_str());
    d_code.set_alist(d_list);
    d_spa.set_alist_sigma(d_list, sigma);

    d_rate =
        static_cast<double>(d_code.dimension()) / static_cast<double>(d_code.get_N());
    set_frame_size(d_code.dimension());

    d_spa.set_K(d_output_size);
    d_spa.set_max_iterations(max_iterations);
}

int ldpc_decoder::get_output_size() { return d_output_size; }

int ldpc_decoder::get_input_size() { return d_input_size; }

double ldpc_decoder::rate() { return d_rate; }

bool ldpc_decoder::set_frame_size(unsigned int frame_size)
{
    if (frame_size % d_code.dimension() != 0) {
        GR_LOG_ERROR(d_logger,
                     boost::format("Frame size (%1% bits) must be a "
                                   "multiple of the information word "
                                   "size of the LDPC matrix, %2%") %
                         frame_size % (d_code.dimension()));
        throw std::runtime_error("ldpc_decoder: cannot use frame size.");
    }

    d_output_size = frame_size;
    d_input_size = static_cast<int>(round(frame_size / d_rate));

    return true;
}

void ldpc_decoder::generic_work(void* inBuffer, void* outBuffer)
{
    const float* in = (const float*)inBuffer;
    unsigned char* out = (unsigned char*)outBuffer;

    int j = 0;
    std::vector<float> rx(d_code.get_N());
    for (int i = 0; i < d_input_size; i += d_code.get_N()) {
        for (int k = 0; k < d_code.get_N(); k++) {
            rx[k] = in[i + k] * (-1);
        }

        int n_iterations = 0;
        std::vector<uint8_t> estimate(d_spa.decode(rx, &n_iterations));
        std::vector<uint8_t> data(d_code.get_systematic_bits(estimate));
        memcpy(&out[j], &data[0], d_code.dimension());
        d_iterations = n_iterations;

        j += d_code.dimension();
    }
}

int ldpc_decoder::get_input_item_size() { return sizeof(INPUT_DATATYPE); }

int ldpc_decoder::get_output_item_size() { return sizeof(OUTPUT_DATATYPE); }

int ldpc_decoder::get_history() { return 0; }

float ldpc_decoder::get_shift() { return 0.0; }

const char* ldpc_decoder::get_conversion() { return "none"; }

ldpc_decoder::~ldpc_decoder() {}

} // namespace fec
} // namespace gr

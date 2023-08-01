/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_LDPC_DECODER_H
#define INCLUDED_LDPC_DECODER_H

typedef float INPUT_DATATYPE;
typedef unsigned char OUTPUT_DATATYPE;

#include <gnuradio/fec/decoder.h>
#include <map>
#include <string>
#include <vector>

#include <gnuradio/fec/alist.h>
#include <gnuradio/fec/awgn_bp.h>
#include <gnuradio/fec/cldpc.h>

namespace gr {
namespace fec {


#define MAXLOG 1e7

class FEC_API ldpc_decoder : public generic_decoder
{
private:
    // private constructor
    ldpc_decoder(std::string alist_file, int max_iterations);

    // plug into the generic fec api
    int get_history() override;
    float get_shift() override;
    const char* get_conversion();
    void generic_work(void* inBuffer, void* outbuffer) override;
    float d_iterations;
    int d_input_size, d_output_size;
    double d_rate;

    alist d_list;
    cldpc d_code;
    awgn_bp d_spa;

public:
    ~ldpc_decoder() override;

    double rate() override;
    bool set_frame_size(unsigned int frame_size) override;

    static generic_decoder::sptr make(std::string alist_file, int max_iterations = 50);

    int get_output_size() override;
    int get_input_size() override;
    int get_input_item_size() override;
    int get_output_item_size() override;
    float get_iterations() override { return d_iterations; }
};

} // namespace fec
} // namespace gr

#endif /* INCLUDED_LDPC_DECODER_H */

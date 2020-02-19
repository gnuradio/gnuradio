/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_LDPC_ENCODER_IMPL_H
#define INCLUDED_LDPC_ENCODER_IMPL_H

#include <gnuradio/fec/alist.h>
#include <gnuradio/fec/cldpc.h>
#include <gnuradio/fec/ldpc_encoder.h>
#include <map>
#include <string>
#include <vector>

namespace gr {
namespace fec {

class ldpc_encoder_impl : public ldpc_encoder
{
private:
    // plug into the generic fec api
    void generic_work(void* inBuffer, void* outbuffer);

    // memory allocated for processing
    int outputSize;
    int inputSize;
    alist d_list;
    cldpc d_code;

public:
    ldpc_encoder_impl(std::string alist_file);
    ~ldpc_encoder_impl();

    double rate() { return (1.0 * get_input_size() / get_output_size()); }
    bool set_frame_size(unsigned int frame_size) { return false; }
    int get_output_size();
    int get_input_size();
};

} // namespace fec
} // namespace gr

#endif /* INCLUDED_LDPC_ENCODER_IMPL_H */

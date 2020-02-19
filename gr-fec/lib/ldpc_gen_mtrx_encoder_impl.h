/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FEC_LDPC_GEN_MTRX_ENCODER_IMPL_H
#define INCLUDED_FEC_LDPC_GEN_MTRX_ENCODER_IMPL_H

#include <gnuradio/fec/ldpc_G_matrix.h>
#include <gnuradio/fec/ldpc_gen_mtrx_encoder.h>
#include <map>
#include <string>

namespace gr {
namespace fec {
namespace code {

class FEC_API ldpc_gen_mtrx_encoder_impl : public ldpc_gen_mtrx_encoder
{
private:
    void generic_work(void* inbuffer, void* outbuffer);
    int get_output_size();
    int get_input_size();

    // Number of bits in the information word
    unsigned int d_frame_size;

    // Number of output bits after coding
    int d_output_size;

    // Rate of the code, n/k
    double d_rate;

    // Matrix object to use for encoding
    ldpc_G_matrix::sptr d_G;

public:
    ldpc_gen_mtrx_encoder_impl(const ldpc_G_matrix::sptr G_obj);
    ~ldpc_gen_mtrx_encoder_impl();

    bool set_frame_size(unsigned int frame_size);
    double rate();
};

} /* namespace code */
} /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_LDPC_GEN_MTRX_ENCODER_IMPL_H */

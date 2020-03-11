/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_LDPC_PAR_MTRX_ENCODER_IMPL_H
#define INCLUDED_LDPC_PAR_MTRX_ENCODER_IMPL_H

#include <gnuradio/fec/ldpc_par_mtrx_encoder.h>

namespace gr {
namespace fec {
namespace code {

class ldpc_par_mtrx_encoder_impl : public ldpc_par_mtrx_encoder
{
private:
    // plug into the generic fec api
    void generic_work(void* inbuffer, void* outbuffer);

    // Number of bits in the frame to be encoded
    unsigned int d_frame_size;

    // Number of output bits after coding
    int d_output_size;

    // Rate of the code, n/k
    double d_rate;

    // LDPC parity check matrix object
    code::ldpc_H_matrix::sptr d_H;

public:
    ldpc_par_mtrx_encoder_impl(const code::ldpc_H_matrix::sptr H_obj);
    ~ldpc_par_mtrx_encoder_impl();

    double rate();
    bool set_frame_size(unsigned int frame_size);
    int get_output_size();
    int get_input_size();
};

} /* namespace code */
} /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_LDPC_PAR_MTRX_ENCODER_IMPL_H */

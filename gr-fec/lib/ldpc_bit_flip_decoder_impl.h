/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FEC_LDPC_BIT_FLIP_DECODER_IMPL_H
#define INCLUDED_FEC_LDPC_BIT_FLIP_DECODER_IMPL_H

#include <gnuradio/fec/ldpc_bit_flip_decoder.h>

namespace gr {
namespace fec {
namespace code {

class FEC_API ldpc_bit_flip_decoder_impl : public ldpc_bit_flip_decoder
{
private:
    // Plug into the generic FEC API:
    int get_input_size() override;  // n, # of bits in the received block
    int get_output_size() override; // k, # of bits in the info word
    int d_input_size;
    int d_output_size;

    double d_rate;

    // FEC matrix object to use for decoding
    fec_mtrx_sptr d_mtrx;

    // Maximum number of iterations to do in decoding algorithm
    unsigned int d_max_iterations;

public:
    ldpc_bit_flip_decoder_impl(const fec_mtrx_sptr mtrx_obj, unsigned int max_iter = 100);
    ~ldpc_bit_flip_decoder_impl() override;

    void generic_work(const void* inbuffer, void* outbuffer) override;
    bool set_frame_size(unsigned int frame_size) override;
    double rate() override;
};

} /* namespace code */
} /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_LDPC_BIT_FLIP_DECODER_IMPL_H */

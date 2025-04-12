/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FEC_REPETITION_DECODER_IMPL_H
#define INCLUDED_FEC_REPETITION_DECODER_IMPL_H

#include <gnuradio/fec/repetition_decoder.h>
#include <algorithm>
#include <string>
#include <vector>

namespace gr {
namespace fec {
namespace code {

class FEC_API repetition_decoder_impl : public repetition_decoder
{
private:
    // plug into the generic fec api
    void generic_work(const void* inbuffer, void* outbuffer) override;
    int get_output_size() override;
    int get_input_size() override;
    int get_input_item_size() override;
    float get_shift() override;
    const char* get_input_conversion() override;
    // const char* get_output_conversion();

    unsigned int d_max_frame_size;
    unsigned int d_frame_size;
    unsigned int d_rep;
    float d_ap_prob;

    std::vector<float> d_trials;

public:
    repetition_decoder_impl(int frame_size, int rep, float ap_prob = 0.5);
    ~repetition_decoder_impl() override;

    bool set_frame_size(unsigned int frame_size) override;
    double rate() override;
};

} /* namespace code */
} /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_REPETITION_DECODER_IMPL_H */

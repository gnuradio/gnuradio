/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FEC_REPETITION_ENCODER_IMPL_H
#define INCLUDED_FEC_REPETITION_ENCODER_IMPL_H

#include <gnuradio/fec/repetition_encoder.h>
#include <map>
#include <string>

namespace gr {
namespace fec {
namespace code {

class FEC_API repetition_encoder_impl : public repetition_encoder
{
private:
    // plug into the generic fec api
    void generic_work(void* inbuffer, void* outbuffer);
    int get_output_size();
    int get_input_size();

    unsigned int d_max_frame_size;
    unsigned int d_frame_size;
    unsigned int d_rep;

public:
    repetition_encoder_impl(int frame_size, int rep);
    ~repetition_encoder_impl();

    bool set_frame_size(unsigned int frame_size);
    double rate();
};

} /* namespace code */
} /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_REPETITION_ENCODER_IMPL_H */

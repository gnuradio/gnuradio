/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FEC_DUMMY_ENCODER_IMPL_H
#define INCLUDED_FEC_DUMMY_ENCODER_IMPL_H

#include <gnuradio/fec/dummy_encoder.h>
#include <map>
#include <string>

namespace gr {
namespace fec {
namespace code {

class FEC_API dummy_encoder_impl : public dummy_encoder
{
private:
    // plug into the generic fec api
    void generic_work(void* inbuffer, void* outbuffer);
    int get_output_size();
    int get_input_size();
    const char* get_input_conversion();
    const char* get_output_conversion();
    bool d_pack_input;
    bool d_packed_bits_output;

    unsigned int d_max_frame_size;
    unsigned int d_frame_size;

public:
    dummy_encoder_impl(int frame_size, bool pack = false, bool packed_bits = false);
    ~dummy_encoder_impl();

    bool set_frame_size(unsigned int frame_size);
    double rate();
};

} /* namespace code */
} /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_DUMMY_ENCODER_IMPL_H */

/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FEC_DUMMY_DECODER_IMPL_H
#define INCLUDED_FEC_DUMMY_DECODER_IMPL_H

#include <gnuradio/fec/dummy_decoder.h>
#include <map>
#include <string>

namespace gr {
namespace fec {
namespace code {

class FEC_API dummy_decoder_impl : public dummy_decoder
{
private:
    // plug into the generic fec api
    void generic_work(void* inbuffer, void* outbuffer) override;
    int get_output_size() override;
    int get_input_size() override;
    int get_input_item_size() override;
    const char* get_input_conversion() override;
    // const char* get_output_conversion();

    unsigned int d_max_frame_size;
    unsigned int d_frame_size;

public:
    dummy_decoder_impl(int frame_size);
    ~dummy_decoder_impl() override;

    bool set_frame_size(unsigned int frame_size) override;
    double rate() override;
};

} /* namespace code */
} /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_DUMMY_DECODER_IMPL_H */

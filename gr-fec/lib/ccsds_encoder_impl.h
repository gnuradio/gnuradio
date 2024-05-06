/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FEC_CCSDS_ENCODER_IMPL_H
#define INCLUDED_FEC_CCSDS_ENCODER_IMPL_H

#include <gnuradio/fec/ccsds_encoder.h>
#include <map>
#include <string>

namespace gr {
namespace fec {
namespace code {

class FEC_API ccsds_encoder_impl : public ccsds_encoder
{
private:
    // plug into the generic fec api
    void generic_work(const void* inbuffer, void* outbuffer) override;
    int get_output_size() override;
    int get_input_size() override;
    const char* get_input_conversion() override;

    unsigned int d_max_frame_size;
    unsigned int d_frame_size;

    unsigned char d_start_state;
    cc_mode_t d_mode;

    int d_output_size;

public:
    ccsds_encoder_impl(int frame_size,
                       int start_state = 0,
                       cc_mode_t mode = CC_STREAMING);
    ~ccsds_encoder_impl() override;

    bool set_frame_size(unsigned int frame_size) override;
    double rate() override;
};

} /* namespace code */
} /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_CCSDS_ENCODER_IMPL_H */

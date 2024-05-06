/* -*- c++ -*- */
/*
 * Copyright 2013-2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FEC_CC_ENCODER_IMPL_H
#define INCLUDED_FEC_CC_ENCODER_IMPL_H

#include <gnuradio/fec/cc_encoder.h>
#include <map>
#include <string>

namespace gr {
namespace fec {
namespace code {

class FEC_API cc_encoder_impl : public cc_encoder
{
private:
    // plug into the generic fec api
    void generic_work(const void* inbuffer, void* outbuffer) override;
    int get_output_size() override;
    int get_input_size() override;

    // everything else...
    unsigned char Partab[256];
    unsigned int d_frame_size;
    unsigned int d_max_frame_size;
    unsigned int d_rate;
    unsigned int d_k;
    std::vector<int> d_polys;
    unsigned int d_start_state;
    cc_mode_t d_mode;
    int d_padding;
    int d_output_size;

    int parity(int x);
    int parityb(unsigned char x);
    void partab_init(void);

public:
    cc_encoder_impl(int frame_size,
                    int k,
                    int rate,
                    std::vector<int> polys,
                    int start_state = 0,
                    cc_mode_t mode = CC_STREAMING,
                    bool padded = false);
    ~cc_encoder_impl() override;

    bool set_frame_size(unsigned int frame_size) override;
    double rate() override;
};

} /* namespace code */
} /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_CC_ENCODER_IMPL_H */

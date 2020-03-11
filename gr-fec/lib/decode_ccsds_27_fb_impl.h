/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FEC_DECODE_CCSDS_27_FB_IMPL_H
#define INCLUDED_FEC_DECODE_CCSDS_27_FB_IMPL_H

#include <gnuradio/fec/decode_ccsds_27_fb.h>
#include <gnuradio/fec/viterbi.h>

namespace gr {
namespace fec {

class FEC_API decode_ccsds_27_fb_impl : public decode_ccsds_27_fb
{
private:
    // Viterbi state
    int d_mettab[2][256];
    struct viterbi_state d_state0[64];
    struct viterbi_state d_state1[64];
    unsigned char d_viterbi_in[16];

    int d_count;

public:
    decode_ccsds_27_fb_impl();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_DECODE_CCSDS_27_FB_IMPL_H */

/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_ATSC_TRELLIS_ENCODER_IMPL_H
#define INCLUDED_DTV_ATSC_TRELLIS_ENCODER_IMPL_H

#include "atsc_basic_trellis_encoder.h"
#include "atsc_types.h"
#include <gnuradio/dtv/atsc_trellis_encoder.h>

namespace gr {
namespace dtv {

class atsc_trellis_encoder_impl : public atsc_trellis_encoder
{
private:
    bool debug;

    /* How many separate Trellis encoders / Viterbi decoders run in parallel */
    static constexpr int NCODERS = 12;
    static constexpr int SEGMENT_SIZE = ATSC_MPEG_RS_ENCODED_LENGTH;
    static constexpr int INPUT_SIZE = (SEGMENT_SIZE * 12);
    static constexpr int OUTPUT_SIZE = (ATSC_DATA_SEGMENT_LENGTH * 12);

    void reset();
    void encode(atsc_data_segment out[NCODERS],
                const atsc_mpeg_packet_rs_encoded in[NCODERS]);
    void encode_helper(unsigned char output[OUTPUT_SIZE],
                       const unsigned char input[INPUT_SIZE]);

    atsc_basic_trellis_encoder enc[NCODERS];

public:
    atsc_trellis_encoder_impl();
    ~atsc_trellis_encoder_impl();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_ATSC_TRELLIS_ENCODER_IMPL_H */

/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_ATSC_VITERBI_DECODER_IMPL_H
#define INCLUDED_DTV_ATSC_VITERBI_DECODER_IMPL_H

#include "atsc_interleaver_fifo.h"
#include "atsc_syminfo_impl.h"
#include <gnuradio/dtv/atsc_consts.h>
#include <gnuradio/dtv/atsc_viterbi_decoder.h>

#define USE_SIMPLE_SLICER 0
#define NCODERS 12

#if USE_SIMPLE_SLICER
#include "atsc_fake_single_viterbi.h"
#else
#include "atsc_single_viterbi.h"
#endif

namespace gr {
namespace dtv {

#if USE_SIMPLE_SLICER
typedef atsc_fake_single_viterbi single_viterbi_t;
#else
typedef atsc_single_viterbi single_viterbi_t;
#endif

class atsc_viterbi_decoder_impl : public atsc_viterbi_decoder
{
private:
    typedef interleaver_fifo<unsigned char> fifo_t;

    static constexpr int SEGMENT_SIZE = ATSC_MPEG_RS_ENCODED_LENGTH; // 207
    static constexpr int OUTPUT_SIZE = (SEGMENT_SIZE * 12);
    static constexpr int INPUT_SIZE = (ATSC_DATA_SEGMENT_LENGTH * 12);

    single_viterbi_t viterbi[NCODERS];
    fifo_t* fifo[NCODERS];

public:
    atsc_viterbi_decoder_impl();
    ~atsc_viterbi_decoder_impl();

    void setup_rpc();

    void reset();

    std::vector<float> decoder_metrics() const;

    virtual int work(int noutput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
};

} /* namespace dtv */
} /* namespace gr */

#endif /* INCLUDED_DTV_ATSC_VITERBI_DECODER_IMPL_H */

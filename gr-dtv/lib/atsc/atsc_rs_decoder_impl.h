/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_ATSC_RS_DECODER_IMPL_H
#define INCLUDED_DTV_ATSC_RS_DECODER_IMPL_H

#include "atsc_types.h"
#include <gnuradio/dtv/atsc_rs_decoder.h>

extern "C" {
#include <gnuradio/fec/rs.h>
}

namespace gr {
namespace dtv {

class atsc_rs_decoder_impl : public atsc_rs_decoder
{
private:
    int d_nerrors_corrected_count;
    int d_bad_packet_count;
    int d_total_packets;
    void* d_rs;

public:
    atsc_rs_decoder_impl();
    ~atsc_rs_decoder_impl() override;

    void setup_rpc() override;

    int num_errors_corrected() const override;
    int num_bad_packets() const override;
    int num_packets() const override;

    /*!
     * Decode RS encoded packet.
     * \returns a count of corrected symbols, or -1 if the block was uncorrectible.
     */
    int decode(uint8_t* out, const uint8_t* in);

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace dtv */
} /* namespace gr */

#endif /* INCLUDED_DTV_ATSC_RS_DECODER_IMPL_H */

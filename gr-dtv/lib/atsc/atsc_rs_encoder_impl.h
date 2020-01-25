/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_ATSC_RS_ENCODER_IMPL_H
#define INCLUDED_DTV_ATSC_RS_ENCODER_IMPL_H

#include "atsc_types.h"
#include <gnuradio/dtv/atsc_rs_encoder.h>

extern "C" {
#include <gnuradio/fec/rs.h>
}

namespace gr {
namespace dtv {

class atsc_rs_encoder_impl : public atsc_rs_encoder
{
private:
    void* d_rs;
    void encode(atsc_mpeg_packet_rs_encoded& out, const atsc_mpeg_packet_no_sync& in);

public:
    atsc_rs_encoder_impl();
    ~atsc_rs_encoder_impl();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_ATSC_RS_ENCODER_IMPL_H */

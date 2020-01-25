/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "dummy_encoder_impl.h"
#include <gnuradio/fec/generic_encoder.h>
#include <volk/volk.h>
#include <sstream>

namespace gr {
namespace fec {
namespace code {

generic_encoder::sptr dummy_encoder::make(int frame_size, bool pack, bool packed_bits)
{
    return generic_encoder::sptr(new dummy_encoder_impl(frame_size, pack, packed_bits));
}

dummy_encoder_impl::dummy_encoder_impl(int frame_size, bool pack, bool packed_bits)
    : generic_encoder("dummy_encoder"),
      d_pack_input(pack),
      d_packed_bits_output(packed_bits)
{
    d_max_frame_size = frame_size;
    set_frame_size(frame_size);
}

dummy_encoder_impl::~dummy_encoder_impl() {}

int dummy_encoder_impl::get_output_size() { return d_frame_size; }

int dummy_encoder_impl::get_input_size() { return d_frame_size; }

const char* dummy_encoder_impl::get_input_conversion()
{
    return d_pack_input ? "pack" : "none";
}

const char* dummy_encoder_impl::get_output_conversion()
{
    return d_packed_bits_output ? "packed_bits" : "none";
}

bool dummy_encoder_impl::set_frame_size(unsigned int frame_size)
{
    bool ret = true;
    if (frame_size > d_max_frame_size) {
        GR_LOG_INFO(d_logger,
                    boost::format("tried to set frame to %1%; max possible is %2%") %
                        frame_size % d_max_frame_size);
        frame_size = d_max_frame_size;
        ret = false;
    }

    d_frame_size = frame_size;

    return ret;
}

double dummy_encoder_impl::rate() { return 1.0; }

void dummy_encoder_impl::generic_work(void* inbuffer, void* outbuffer)
{
    const unsigned char* in = (const unsigned char*)inbuffer;
    unsigned char* out = (unsigned char*)outbuffer;

    memcpy(out, in, d_frame_size * sizeof(char));
}

} /* namespace code */
} /* namespace fec */
} /* namespace gr */

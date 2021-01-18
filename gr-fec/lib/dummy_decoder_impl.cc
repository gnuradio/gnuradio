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

#include "dummy_decoder_impl.h"
#include <volk/volk.h>
#include <boost/assign/list_of.hpp>
#include <cmath>
#include <cstdio>
#include <sstream>
#include <vector>

namespace gr {
namespace fec {
namespace code {

generic_decoder::sptr dummy_decoder::make(int frame_size)
{
    return generic_decoder::sptr(new dummy_decoder_impl(frame_size));
}

dummy_decoder_impl::dummy_decoder_impl(int frame_size) : generic_decoder("dummy_decoder")
{
    // Set max frame size here; all buffers and settings will be
    // based on this value.
    d_max_frame_size = frame_size;
    set_frame_size(frame_size);
}

dummy_decoder_impl::~dummy_decoder_impl() {}

int dummy_decoder_impl::get_output_size()
{
    // unpacked bits
    return d_frame_size;
}

int dummy_decoder_impl::get_input_size() { return d_frame_size; }

int dummy_decoder_impl::get_input_item_size() { return sizeof(float); }

const char* dummy_decoder_impl::get_input_conversion() { return "none"; }

bool dummy_decoder_impl::set_frame_size(unsigned int frame_size)
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

double dummy_decoder_impl::rate() { return 1.0; }

void dummy_decoder_impl::generic_work(void* inbuffer, void* outbuffer)
{
    const float* in = (const float*)inbuffer;
    int8_t* out = (int8_t*)outbuffer;

    // memcpy(out, in, d_frame_size*sizeof(char));
    volk_32f_binary_slicer_8i(out, in, d_frame_size);
    // volk_32f_s32f_convert_8i(out, in, 1.0/2.0, d_frame_size);
}

} /* namespace code */
} /* namespace fec */
} /* namespace gr */

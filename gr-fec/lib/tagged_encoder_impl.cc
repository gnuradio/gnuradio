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

#include "tagged_encoder_impl.h"
#include <gnuradio/io_signature.h>
#include <cstdio>

namespace gr {
namespace fec {

tagged_encoder::sptr tagged_encoder::make(generic_encoder::sptr my_encoder,
                                          size_t input_item_size,
                                          size_t output_item_size,
                                          const std::string& lengthtagname,
                                          int mtu)
{
    return gnuradio::make_block_sptr<tagged_encoder_impl>(
        my_encoder, input_item_size, output_item_size, lengthtagname, mtu);
}

tagged_encoder_impl::tagged_encoder_impl(generic_encoder::sptr my_encoder,
                                         size_t input_item_size,
                                         size_t output_item_size,
                                         const std::string& lengthtagname,
                                         int mtu)
    : tagged_stream_block("fec_tagged_encoder",
                          io_signature::make(1, 1, input_item_size),
                          io_signature::make(1, 1, output_item_size),
                          lengthtagname),
      d_mtu(mtu)
{
    d_encoder = my_encoder;

    d_encoder->set_frame_size(d_mtu * 8);
    set_relative_rate(d_encoder->rate());
}

tagged_encoder_impl::~tagged_encoder_impl() {}

int tagged_encoder_impl::calculate_output_stream_length(const gr_vector_int& ninput_items)
{
    if (ninput_items[0] > (d_mtu * 8)) {
        throw std::runtime_error("tagged_encoder: received frame is larger than MTU.");
    }
    d_encoder->set_frame_size(ninput_items[0]);
    return d_encoder->get_output_size();
}

int tagged_encoder_impl::work(int noutput_items,
                              gr_vector_int& ninput_items,
                              gr_vector_const_void_star& input_items,
                              gr_vector_void_star& output_items)
{
    char* inbuffer = (char*)input_items[0];
    char* outbuffer = (char*)output_items[0];

    GR_LOG_DEBUG(d_debug_logger,
                 boost::format("nout: %1%   nin: %2%   ret: %3%") % noutput_items %
                     ninput_items[0] % d_encoder->get_output_size());

    d_encoder->generic_work((void*)(inbuffer), (void*)(outbuffer));

    return d_encoder->get_output_size();
}

} /* namespace fec */
} /* namespace gr */

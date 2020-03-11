/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FEC_TAGGED_ENCODER_IMPL_H
#define INCLUDED_FEC_TAGGED_ENCODER_IMPL_H

#include <gnuradio/fec/tagged_encoder.h>

namespace gr {
namespace fec {

class FEC_API tagged_encoder_impl : public tagged_encoder
{
private:
    generic_encoder::sptr d_encoder;
    int d_mtu;

public:
    tagged_encoder_impl(generic_encoder::sptr my_encoder,
                        size_t input_item_size,
                        size_t output_item_size,
                        const std::string& lengthtagname = "packet_len",
                        int mtu = 1500);
    ~tagged_encoder_impl();

    int work(int noutput_items,
             gr_vector_int& ninput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
    int calculate_output_stream_length(const gr_vector_int& ninput_items);
};

} /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_TAGGED_ENCODER_IMPL_H */

/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FEC_TAGGED_DECODER_IMPL_H
#define INCLUDED_FEC_TAGGED_DECODER_IMPL_H

#include <gnuradio/fec/tagged_decoder.h>

namespace gr {
namespace fec {

class FEC_API tagged_decoder_impl : public tagged_decoder
{
private:
    generic_decoder::sptr d_decoder;
    int d_mtu;

public:
    tagged_decoder_impl(generic_decoder::sptr my_decoder,
                        size_t input_item_size,
                        size_t output_item_size,
                        const std::string& lengthtagname = "packet_len",
                        int mtu = 1500);
    ~tagged_decoder_impl();

    int work(int noutput_items,
             gr_vector_int& ninput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
    int calculate_output_stream_length(const gr_vector_int& ninput_items);
};

} /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_TAGGED_DECODER_IMPL_H */

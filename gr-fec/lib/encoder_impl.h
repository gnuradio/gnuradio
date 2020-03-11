/* -*- c++ -*- */
/*
 * Copyright 2013-2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FEC_ENCODER_IMPL_H
#define INCLUDED_FEC_ENCODER_IMPL_H

#include <gnuradio/fec/encoder.h>

namespace gr {
namespace fec {

class FEC_API encoder_impl : public encoder
{
private:
    generic_encoder::sptr d_encoder;
    size_t d_input_item_size;
    size_t d_output_item_size;
    size_t d_input_size;
    size_t d_output_size;

public:
    encoder_impl(generic_encoder::sptr my_encoder,
                 size_t input_item_size,
                 size_t output_item_size);
    ~encoder_impl();

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
    int fixed_rate_ninput_to_noutput(int ninput);
    int fixed_rate_noutput_to_ninput(int noutput);
    void forecast(int noutput_items, gr_vector_int& ninput_items_required);
};

} /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_ENCODER_IMPL_H */

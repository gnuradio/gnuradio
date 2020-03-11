/* -*- c++ -*- */
/*
 * Copyright 2013-2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FEC_DECODER_IMPL_H
#define INCLUDED_FEC_DECODER_IMPL_H

#include <gnuradio/fec/decoder.h>

namespace gr {
namespace fec {

class FEC_API decoder_impl : public decoder
{
private:
    generic_decoder::sptr d_decoder;
    size_t d_input_item_size;
    size_t d_output_item_size;

public:
    decoder_impl(generic_decoder::sptr my_decoder,
                 size_t input_item_size,
                 size_t output_item_size);

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

#endif /* INCLUDED_FEC_DECODER_IMPL_H */

/* -*- c++ -*- */
/*
 * Copyright 2004,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_SIMPLE_FRAMER_IMPL_H
#define INCLUDED_GR_SIMPLE_FRAMER_IMPL_H

#include <gnuradio/digital/simple_framer.h>

namespace gr {
namespace digital {

class simple_framer_impl : public simple_framer
{
private:
    int d_seqno;
    int d_input_block_size;  // bytes
    int d_output_block_size; // bytes

public:
    simple_framer_impl(int payload_bytesize);
    ~simple_framer_impl();

    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_GR_SIMPLE_FRAMER_IMPL_H */

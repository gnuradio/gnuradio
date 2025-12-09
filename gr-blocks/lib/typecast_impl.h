/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2013 Free Software Foundation, Inc.
 * Copyright 2025 Sam Lane
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_TYPECAST_IMPL_H
#define INCLUDED_GR_TYPECAST_IMPL_H

#include <gnuradio/blocks/typecast.h>

namespace gr {
namespace blocks {

class typecast_impl : public typecast
{
private:
    size_t d_sizeof_in_stream_item;
    size_t d_sizeof_out_stream_item;
    size_t d_copy_modulo;

public:
    typecast_impl(size_t sizeof_in_stream_item, size_t sizeof_out_stream_item);
    ~typecast_impl() override;

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_TYPECAST_IMPL_H */

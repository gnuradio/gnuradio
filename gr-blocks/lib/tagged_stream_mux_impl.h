/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_TAGGED_STREAM_MUX_IMPL_H
#define INCLUDED_TAGGED_STREAM_MUX_IMPL_H

#include <gnuradio/blocks/tagged_stream_mux.h>
#include <vector>

namespace gr {
namespace blocks {

class tagged_stream_mux_impl : public tagged_stream_mux
{
private:
    const size_t d_itemsize;
    const unsigned int d_tag_preserve_head_pos;

protected:
    int calculate_output_stream_length(const gr_vector_int& ninput_items) override;

public:
    tagged_stream_mux_impl(size_t itemsize,
                           const std::string& lengthtagname,
                           unsigned int d_tag_preserve_head_pos);
    ~tagged_stream_mux_impl() override;

    int work(int noutput_items,
             gr_vector_int& ninput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} // namespace blocks
} // namespace gr

#endif

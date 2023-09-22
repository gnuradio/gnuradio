/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_TAGGED_STREAM_MULTIPLY_LENGTH_IMPL_H
#define INCLUDED_TAGGED_STREAM_MULTIPLY_LENGTH_IMPL_H

#include <gnuradio/blocks/tagged_stream_multiply_length.h>
#include <vector>

namespace gr {
namespace blocks {

class tagged_stream_multiply_length_impl : public tagged_stream_multiply_length
{
private:
    const pmt::pmt_t d_lengthtag;
    double d_scalar;
    const size_t d_itemsize;

public:
    tagged_stream_multiply_length_impl(size_t itemsize,
                                       const std::string& lengthtagname,
                                       double scalar);
    ~tagged_stream_multiply_length_impl() override;

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;

    void set_scalar(double scalar) override { d_scalar = scalar; }

    void set_scalar_pmt(pmt::pmt_t msg) { set_scalar(pmt::to_double(msg)); }
};

} // namespace blocks
} // namespace gr

#endif

/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_TAGGED_STREAM_ALIGN_IMPL_H
#define INCLUDED_TAGGED_STREAM_ALIGN_IMPL_H

#include <gnuradio/blocks/tagged_stream_align.h>
#include <vector>

namespace gr {
namespace blocks {

class tagged_stream_align_impl : public tagged_stream_align
{
private:
    const size_t d_itemsize;
    const pmt::pmt_t d_lengthtag;
    bool d_have_sync;

public:
    tagged_stream_align_impl(size_t itemsize, const std::string& lengthtagname);
    ~tagged_stream_align_impl() override;

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;
};

} // namespace blocks
} // namespace gr

#endif

/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2012,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_HEAD_IMPL_H
#define INCLUDED_GR_HEAD_IMPL_H

#include <gnuradio/blocks/head.h>

namespace gr {
namespace blocks {

class head_impl : public head
{
private:
    uint64_t d_nitems;
    uint64_t d_ncopied_items;

public:
    head_impl(size_t sizeof_stream_item, uint64_t nitems);
    ~head_impl() override;

    void reset() override { d_ncopied_items = 0; }
    void set_length(uint64_t nitems) override { d_nitems = nitems; }

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_HEAD_IMPL_H */

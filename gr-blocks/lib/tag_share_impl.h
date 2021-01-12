/* -*- c++ -*- */
/*
 * Copyright 2017 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_TAG_SHARE_IMPL_H
#define INCLUDED_BLOCKS_TAG_SHARE_IMPL_H

#include <gnuradio/blocks/tag_share.h>

namespace gr {
namespace blocks {

class tag_share_impl : public tag_share
{
private:
    const size_t d_sizeof_io_item;
    const size_t d_vlen;

public:
    tag_share_impl(size_t sizeof_io_item, size_t sizeof_share_item, size_t vlen);
    ~tag_share_impl() override;

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} // namespace blocks
} // namespace gr

#endif /* INCLUDED_BLOCKS_TAG_SHARE_IMPL_H */

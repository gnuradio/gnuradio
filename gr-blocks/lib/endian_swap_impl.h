/* -*- c++ -*- */
/*
 * Copyright 2004,2012,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_ENDIAN_SWAP_IMPL_H
#define INCLUDED_GR_ENDIAN_SWAP_IMPL_H

#include <gnuradio/blocks/endian_swap.h>

namespace gr {
namespace blocks {

class endian_swap_impl : public endian_swap
{
private:
public:
    endian_swap_impl(size_t item_size_bytes);
    ~endian_swap_impl() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_ENDIAN_SWAP_IMPL_H */

/* -*- c++ -*- */
/*
 * Copyright 2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef NOT_BLK_IMPL_H
#define NOT_BLK_IMPL_H

#include <gnuradio/blocks/not_blk.h>

namespace gr {
namespace blocks {

template <class T>
class BLOCKS_API not_blk_impl : public not_blk<T>
{
    const size_t d_vlen;

public:
    not_blk_impl(size_t vlen);

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* NOT_BLK_IMPL_H */

/* -*- c++ -*- */
/*
 * Copyright 2014,2015,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef MIN_BLK_IMPL_H
#define MIN_BLK_IMPL_H

#include <gnuradio/blocks/min_blk.h>

namespace gr {
namespace blocks {

template <class T>
class min_blk_impl : public min_blk<T>
{
private:
    const size_t d_vlen, d_vlen_out;

public:
    min_blk_impl(size_t vlen, size_t vlen_out);
    ~min_blk_impl() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* MIN_BLK_IMPL_H */

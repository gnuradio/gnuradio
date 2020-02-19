/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_REPEAT_IMPL_H
#define INCLUDED_REPEAT_IMPL_H

#include <gnuradio/blocks/repeat.h>

namespace gr {
namespace blocks {

class BLOCKS_API repeat_impl : public repeat
{
    size_t d_itemsize;
    int d_interp;

public:
    repeat_impl(size_t itemsize, int d_interp);

    int interpolation() const { return d_interp; }
    void set_interpolation(int interp);


    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);

private:
    void msg_set_interpolation(pmt::pmt_t msg);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_REPEAT_IMPL_H */

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
public:
    repeat_impl(size_t itemsize, int d_interp);

    int interpolation() const override { return d_interp; }
    void set_interpolation(int interp) override;

    void forecast(int noutput_items, gr_vector_int& ninput_items_required) override;
    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;

private:
    const size_t d_itemsize;
    size_t d_interp;
    size_t d_left_to_copy;

    void msg_set_interpolation(const pmt::pmt_t& msg);
    const pmt::pmt_t c_msg_port;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_REPEAT_IMPL_H */

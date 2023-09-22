/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_CORRECTIQ_SWAPIQ_IMPL_H
#define INCLUDED_CORRECTIQ_SWAPIQ_IMPL_H

#include <gnuradio/blocks/correctiq_swapiq.h>

#define SWAPTYPE_FLOATCOMPLEX 1
#define SWAPTYPE_SHORTCOMPLEX 2
#define SWAPTYPE_BYTECOMPLEX 3

namespace gr {
namespace blocks {

class swap_iq_impl : public swap_iq
{
private:
    const int d_datatype;

public:
    swap_iq_impl(int datatype, int datasize);
    ~swap_iq_impl() override;

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} // namespace blocks
} // namespace gr

#endif /* INCLUDED_CORRECTIQ_SWAPIQ_IMPL_H */

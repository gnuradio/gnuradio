/* -*- c++ -*- */
/*
 * Copyright 2007,2013,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef SAMPLE_AND_HOLD_IMPL_H
#define SAMPLE_AND_HOLD_IMPL_H

#include <gnuradio/blocks/sample_and_hold.h>

namespace gr {
namespace blocks {

template <class T>
class sample_and_hold_impl : public sample_and_hold<T>
{
private:
    T d_data;

public:
    sample_and_hold_impl();
    ~sample_and_hold_impl() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* SAMPLE_AND_HOLD_IMPL_H */

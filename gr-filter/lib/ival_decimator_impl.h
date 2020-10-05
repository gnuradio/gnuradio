/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_IVAL_DECIMATOR_IMPL_H
#define INCLUDED_IVAL_DECIMATOR_IMPL_H

#include <gnuradio/filter/ival_decimator.h>

namespace gr {
namespace filter {

class FILTER_API ival_decimator_impl : public ival_decimator
{
private:
    int d_doubledecimation;
    int d_data_size;

public:
    ival_decimator_impl(int decimation, int data_size);

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} // namespace filter
} // namespace gr

#endif /* INCLUDED_IVAL_DECIMATOR_IMPL_H */

/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef INTEGRATE_IMPL_H
#define INTEGRATE_IMPL_H

#include <gnuradio/blocks/integrate.h>

namespace gr {
namespace blocks {

template <class T>
class BLOCKS_API integrate_impl : public integrate<T>
{
    const int d_decim;
    const unsigned int d_vlen;

public:
    integrate_impl(int decim, unsigned int vlen);

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INTEGRATE_IMPL_H */


/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_KEEP_ONE_IN_N_IMPL_H
#define INCLUDED_KEEP_ONE_IN_N_IMPL_H

#include <gnuradio/blocks/keep_one_in_n.h>

namespace gr {
namespace blocks {

class BLOCKS_API keep_one_in_n_impl : public keep_one_in_n
{
    int d_n;
    int d_count;
    float d_decim_rate;

public:
    keep_one_in_n_impl(size_t itemsize, int n);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;

    void set_n(int n) override;
};

} /* namespace blocks */
} /* namespace gr */


#endif /* INCLUDED_KEEP_ONE_IN_N_IMPL_H */

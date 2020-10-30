/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_KEEP_M_IN_N_IMPL_H
#define INCLUDED_KEEP_M_IN_N_IMPL_H

#include <gnuradio/blocks/keep_m_in_n.h>

namespace gr {
namespace blocks {

class BLOCKS_API keep_m_in_n_impl : public keep_m_in_n
{
    int d_m;
    int d_n;
    int d_offset;
    const int d_itemsize;

    void forecast(int noutput_items, gr_vector_int& ninput_items_required) override;

public:
    keep_m_in_n_impl(size_t itemsize, int m, int n, int offset);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;

    void set_m(int m) override;
    void set_n(int n) override;
    void set_offset(int offset) override;
};

} /* namespace blocks */
} /* namespace gr */


#endif /* INCLUDED_KEEP_M_IN_N_IMPL_H */

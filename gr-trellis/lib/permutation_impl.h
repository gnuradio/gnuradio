/* -*- c++ -*- */
/*
 * Copyright 2004,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_TRELLIS_PERMUTATION_IMPL_H
#define INCLUDED_TRELLIS_PERMUTATION_IMPL_H

#include <gnuradio/trellis/api.h>
#include <gnuradio/trellis/permutation.h>

namespace gr {
namespace trellis {

class permutation_impl : public permutation
{
private:
    int d_K;
    std::vector<int> d_TABLE;
    int d_SYMS_PER_BLOCK;
    size_t d_BYTES_PER_SYMBOL;

public:
    permutation_impl(int K,
                     const std::vector<int>& TABLE,
                     int SYMS_PER_BLOCK,
                     size_t NBYTES);
    ~permutation_impl();

    int K() const { return d_K; }
    std::vector<int> TABLE() const { return d_TABLE; }
    int SYMS_PER_BLOCK() const { return d_SYMS_PER_BLOCK; }
    size_t BYTES_PER_SYMBOL() const { return d_BYTES_PER_SYMBOL; }

    void set_K(int K);
    void set_TABLE(const std::vector<int>& table);
    void set_SYMS_PER_BLOCK(int spb);

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace trellis */
} /* namespace gr */

#endif /* INCLUDED_TRELLIS_PERMUTATION_IMPL_H */

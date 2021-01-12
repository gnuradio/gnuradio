/* -*- c++ -*- */
/*
 * Copyright 2017 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_EXPONENTIATE_CONST_CCI_IMPL_H
#define INCLUDED_BLOCKS_EXPONENTIATE_CONST_CCI_IMPL_H

#include <gnuradio/blocks/exponentiate_const_cci.h>

namespace gr {
namespace blocks {

class exponentiate_const_cci_impl : public exponentiate_const_cci
{
private:
    int d_exponent;
    const int d_vlen;

public:
    exponentiate_const_cci_impl(int exponent, size_t vlen);
    ~exponentiate_const_cci_impl() override;
    bool check_topology(int ninputs, int noutputs) override;
    void set_exponent(int exponent) override;

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} // namespace blocks
} // namespace gr

#endif /* INCLUDED_BLOCKS_EXPONENTIATE_CONST_CCI_IMPL_H */

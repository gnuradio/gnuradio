/* -*- c++ -*- */
/*
 * Copyright 2014,2017-2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_MULTIPLY_MATRIX_IMPL_H
#define INCLUDED_BLOCKS_MULTIPLY_MATRIX_IMPL_H

#include <gnuradio/blocks/multiply_matrix.h>

namespace gr {
namespace blocks {

template <class T>
class multiply_matrix_impl : public multiply_matrix<T>
{
private:
    std::vector<std::vector<T>> d_A;

    void
    propagate_tags_by_A(int noutput_items, size_t ninput_ports, size_t noutput_ports);

    void msg_handler_A(pmt::pmt_t A);

public:
    multiply_matrix_impl(std::vector<std::vector<T>> A,
                         gr::block::tag_propagation_policy_t tag_propagation_policy);
    ~multiply_matrix_impl() override;

    const std::vector<std::vector<T>>& get_A() const override { return d_A; };
    bool set_A(const std::vector<std::vector<T>>& new_A) override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} // namespace blocks
} // namespace gr

#endif /* INCLUDED_BLOCKS_MULTIPLY_MATRIX_IMPL_H */

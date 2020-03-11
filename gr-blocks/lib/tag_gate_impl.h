/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_TAG_GATE_IMPL_H
#define INCLUDED_BLOCKS_TAG_GATE_IMPL_H

#include <gnuradio/blocks/tag_gate.h>

namespace gr {
namespace blocks {

class tag_gate_impl : public tag_gate
{
private:
    size_t d_item_size;
    bool d_propagate_tags;
    bool d_single_key_set;

    pmt::pmt_t d_single_key;

public:
    tag_gate_impl(size_t item_size, bool propagate_tags);
    ~tag_gate_impl();

    void set_propagation(bool propagate_tags);

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);

    void set_single_key(const std::string& single_key);
    std::string single_key() const;
};

} // namespace blocks
} // namespace gr

#endif /* INCLUDED_BLOCKS_TAG_GATE_IMPL_H */

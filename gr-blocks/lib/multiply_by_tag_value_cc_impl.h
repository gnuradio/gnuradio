/* -*- c++ -*- */
/*
 * Copyright 2016 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_MULTIPLY_BY_TAG_VALUE_CC_IMPL_H
#define INCLUDED_MULTIPLY_BY_TAG_VALUE_CC_IMPL_H

#include <gnuradio/blocks/multiply_by_tag_value_cc.h>

namespace gr {
namespace blocks {

class BLOCKS_API multiply_by_tag_value_cc_impl : public multiply_by_tag_value_cc
{
private:
    const size_t d_vlen;
    pmt::pmt_t d_tag_key;
    gr_complex d_k;

public:
    multiply_by_tag_value_cc_impl(const std::string& tag_name, size_t vlen);
    ~multiply_by_tag_value_cc_impl() override;

    gr_complex k() const override;

    void setup_rpc() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_MULTIPLY_BY_TAG_VALUE_CC_IMPL_H */

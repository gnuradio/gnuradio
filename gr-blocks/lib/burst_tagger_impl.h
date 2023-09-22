/* -*- c++ -*- */
/*
 * Copyright 2010,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_BURST_TAGGER_IMPL_H
#define INCLUDED_GR_BURST_TAGGER_IMPL_H

#include <gnuradio/blocks/burst_tagger.h>

namespace gr {
namespace blocks {

class burst_tagger_impl : public burst_tagger
{
private:
    const size_t d_itemsize;
    bool d_state;
    pmt::pmt_t d_true_key;
    pmt::pmt_t d_true_value;

    pmt::pmt_t d_false_key;
    pmt::pmt_t d_false_value;

    pmt::pmt_t d_id;

public:
    burst_tagger_impl(size_t itemsize);
    ~burst_tagger_impl() override;

    void set_true_tag(const std::string& key, bool value) override;
    void set_false_tag(const std::string& key, bool value) override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_BURST_TAGGER_IMPL_H */

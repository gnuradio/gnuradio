/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_PATTERNED_INTERLEAVER_IMPL_H
#define INCLUDED_PATTERNED_INTERLEAVER_IMPL_H

#include <gnuradio/blocks/patterned_interleaver.h>
#include <algorithm>

namespace gr {
namespace blocks {

class BLOCKS_API patterned_interleaver_impl : public patterned_interleaver
{
public:
    patterned_interleaver_impl(size_t itemsize, std::vector<int> pattern);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;

    static int pattern_max(std::vector<int> pattern)
    {
        if (pattern.empty()) {
            return 0;
        }
        return *std::max_element(pattern.begin(), pattern.end());
    }

    void forecast(int noutput_items, gr_vector_int& ninput_items_required) override;

    std::vector<int> d_pattern;
    std::vector<int> d_counts;
    size_t d_itemsize;
};

} /* namespace blocks */
} /* namespace gr */

#endif

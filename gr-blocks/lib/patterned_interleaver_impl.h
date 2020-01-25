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
#include <boost/foreach.hpp>

namespace gr {
namespace blocks {

class BLOCKS_API patterned_interleaver_impl : public patterned_interleaver
{
public:
    patterned_interleaver_impl(size_t itemsize, std::vector<int> pattern);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);

    int pattern_max(std::vector<int> pattern)
    {
        int mval(0);
        BOOST_FOREACH (int i, pattern) {
            mval = std::max(mval, i);
        }
        return mval;
    }

    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    std::vector<int> d_pattern;
    std::vector<int> d_counts;
    size_t d_itemsize;
};

} /* namespace blocks */
} /* namespace gr */

#endif

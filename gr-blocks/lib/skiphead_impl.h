/* -*- c++ -*- */
/*
 * Copyright 2005,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_SKIPHEAD_IMPL_H
#define INCLUDED_GR_SKIPHEAD_IMPL_H

#include <gnuradio/blocks/skiphead.h>

namespace gr {
namespace blocks {

class skiphead_impl : public skiphead
{
private:
    const uint64_t d_nitems_to_skip;
    uint64_t d_nitems; // total items seen
    std::vector<tag_t> d_tags;

public:
    skiphead_impl(size_t itemsize, uint64_t nitems_to_skip);
    ~skiphead_impl() override;

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_SKIPHEAD_IMPL_H */

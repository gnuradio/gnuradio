/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_INTERLEAVE_IMPL_H
#define INCLUDED_INTERLEAVE_IMPL_H

#include <gnuradio/blocks/interleave.h>

namespace gr {
namespace blocks {

class BLOCKS_API interleave_impl : public interleave
{
    const size_t d_itemsize;
    const unsigned int d_blocksize;
    unsigned int d_ninputs;

public:
    interleave_impl(size_t itemsize, unsigned int blocksize);

    bool check_topology(int ninputs, int noutputs) override;

    int fixed_rate_ninput_to_noutput(int ninput) override;

    int fixed_rate_noutput_to_ninput(int noutput) override;

    void forecast(int noutput_items, gr_vector_int& ninput_items_required) override;

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */


#endif /* INCLUDED_INTERLEAVE_IMPL_H */

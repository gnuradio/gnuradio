/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DEINTERLEAVE_IMPL_H
#define INCLUDED_DEINTERLEAVE_IMPL_H

#include <gnuradio/blocks/deinterleave.h>

namespace gr {
namespace blocks {

class BLOCKS_API deinterleave_impl : public deinterleave
{

    const size_t d_itemsize;
    const unsigned int d_blocksize;
    unsigned int d_current_output;
    unsigned int d_noutputs;
    unsigned int d_size_bytes; // block size in bytes

public:
    deinterleave_impl(size_t itemsize, unsigned int blocksize);

    void forecast(int noutput_items, gr_vector_int& ninput_items_required) override;
    bool check_topology(int ninputs, int noutputs) override;

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */


#endif /* INCLUDED_DEINTERLEAVE_IMPL_H */

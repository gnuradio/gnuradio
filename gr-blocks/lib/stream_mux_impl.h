/* -*- c++ -*- */
/*
 * Copyright 2012,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_STREAM_MUX_IMPL_H
#define INCLUDED_STREAM_MUX_IMPL_H

#include <gnuradio/blocks/stream_mux.h>

namespace gr {
namespace blocks {

class BLOCKS_API stream_mux_impl : public stream_mux
{
private:
    const size_t d_itemsize;
    unsigned int d_stream{ 0 };    // index of currently selected stream
    int d_residual{ 0 };           // number of items left to put into current stream
    const gr_vector_int d_lengths; // number of items to pack per stream

    void forecast(int noutput_items, gr_vector_int& ninput_items_required) override;

public:
    stream_mux_impl(size_t itemsize, const std::vector<int>& lengths);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_STREAM_MUX_IMPL_H */

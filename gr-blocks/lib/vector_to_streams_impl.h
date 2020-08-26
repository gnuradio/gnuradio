/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_VECTOR_TO_STREAMS_IMPL_H
#define INCLUDED_VECTOR_TO_STREAMS_IMPL_H

#include <gnuradio/blocks/vector_to_streams.h>

namespace gr {
namespace blocks {

class BLOCKS_API vector_to_streams_impl : public vector_to_streams
{
public:
    vector_to_streams_impl(size_t itemsize, size_t nstreams);

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_VECTOR_TO_STREAMS_IMPL_H */

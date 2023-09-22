/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_STREAMS_TO_VECTOR_IMPL_H
#define INCLUDED_STREAMS_TO_VECTOR_IMPL_H

#include <gnuradio/blocks/streams_to_vector.h>

namespace gr {
namespace blocks {

class BLOCKS_API streams_to_vector_impl : public streams_to_vector
{
public:
    streams_to_vector_impl(size_t itemsize, size_t nstreams);

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_STREAMS_TO_VECTOR_IMPL_H */
